#include <string>
#include <Util/NoticeCenter.h>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "utils/myJsonHelper.h"
#include "utils/myServerHelper.h"
#include "event/myEvent.h"
#include "envir/mySystemThreadPool.h"
#include "manager/mySipCatalogManage.h"
#include "server/myHttpServer.h"
using namespace MY_COMMON;
using namespace MY_EVENT;
using toolkit::NoticeHelper;
using MY_UTILS::MyJsonHelper;
using MY_UTILS::MyServerHelper;
using MY_ENVIR::MySystemThdPool;
using MY_MANAGER::MySipCatalogManageView;

namespace MY_SERVER {

MyStatus_t MyHttpServer::ServThdFunc(MyFuncCbParamPtr arg)
{
    MyHttpServer::Ptr servPtr = (MyHttpServer::Ptr)arg;

    MyHttpServAddrCfg_dt addrCfg;
    if (MyStatus_t::SUCCESS != servPtr->getServAddrCfg(addrCfg)) {
        LOG(ERROR) << "HttpServer execute thread function failed. get server addr failed.";
        return MyStatus_t::FAILED;
    }

    MyHttpServer::SmtPtr servSmtPtr = servPtr->shared_from_this();

    net::io_context ioc;
    servSmtPtr->m_acceptPtr = std::make_shared<tcp::acceptor>(ioc, tcp::endpoint(net::ip::make_address(addrCfg.ipAddr), addrCfg.port));
    servSmtPtr->m_acceptPtr->set_option(net::socket_base::reuse_address(true));

    tcp::socket socket(ioc);
    while (true) {
        net::steady_timer timer(ioc, std::chrono::hours(1));
        timer.async_wait([&](beast::error_code errCode) {});

        // 等待客户端连接，超时后继续等待
        try {
            servSmtPtr->m_acceptPtr->async_accept(socket, [&](beast::error_code errCode) {
                if (errCode) {
                    socket.close();
                    throw std::runtime_error(errCode.message());
                }
            });
        }
        catch (std::exception& e) {
            LOG(ERROR) << "HttpServer execute thread function failed. accept client connection error: " << e.what();
            return MyStatus_t::FAILED;
        }

        if (nullptr == servSmtPtr) {
            LOG(WARNING) << "HttpServer execute thread function failed. server expired.";
            break;
        }

        MyStatus_t state;
        if (MyStatus_t::SUCCESS != servPtr->getState(state)) {
            LOG(WARNING) << "HttpServer execute thread function failed. get server state failed.";
            break;
        }

        // 等待 accept 完成或超时
        ioc.run_one();

        if (!socket.is_open()) {
            continue;
        }

        beast::error_code errCode;

        // 获取客户端信息
        auto endpoint = socket.remote_endpoint(errCode);
        if (errCode) {
            LOG(ERROR) << "HttpServer execute thread function failed. get client endpoint error: " << errCode.message();
            continue;
        }

        // 读取请求
        beast::flat_buffer buffer;
        http::request<http::dynamic_body> request;
        http::read(socket, buffer, request, errCode);

        if (errCode) {
            LOG(ERROR) << "HttpServer execute thread function failed. read request error: " << errCode.message();
            continue;
        }

        std::string  respBody;          
        http::status respState = http::status::ok;

        // 处理请求
        std::string url = request.target().to_string();
        if (http::verb::get == request.method()) {
            if (MyStatus_t::SUCCESS != servSmtPtr->onRecvHttpGetReq(url, respState, respBody)) {
                LOG(WARNING) << "HttpServer execute thread function failed. onRecvHttpGetReq failed.";
            } 
            else {
                LOG(INFO) << "HttpServer execute thread function success. proc client http get request. ip addr: " 
                          << endpoint.address().to_string() << ". port: " << endpoint.port()
                          << ". url: " << url;
            }
        }
        else if (http::verb::post == request.method()) {
            // 获取http请求数据内容
            std::string reqBody = boost::beast::buffers_to_string(request.body().data());

            if (MyStatus_t::SUCCESS != servSmtPtr->onRecvHttpPostReq(url, reqBody, respState, respBody)) {
                LOG(WARNING) << "HttpServer execute thread function failed. onRecvHttpPostReq failed.";
            } 
            else {
                LOG(INFO) << "HttpServer execute thread function success. proc client http post request. ip addr: " 
                          << endpoint.address().to_string() << ". port: " << endpoint.port()
                          << ". url: " << url;
            }
        }
        else {
            LOG(WARNING) << "HttpServer execute thread function failed. unsupport http method. ip addr: " 
                         << endpoint.address().to_string() << ". port: " << endpoint.port()
                         << ". url: " << url << ". method:" << http::to_string(request.method()).to_string();

            respState = http::status::bad_request;
            MyJsonHelper::GenerateHttpErrMsgBody("Unsupported HTTP method", respBody);
        }

        // 创建响应
        http::response<http::string_body> response(http::status::ok, request.version());
        response.set(http::field::content_type, "application/json");
        response.body() = respBody;
        response.prepare_payload();

        // 发送响应
        http::write(socket, response, errCode);

        // 重置
        socket.close();
        timer.cancel();
    }
    
    return MyStatus_t::SUCCESS;
}

MyHttpServer::MyHttpServer() : m_status(MyStatus_t::FAILED), m_acceptPtr(nullptr)
{

}      

MyHttpServer::~MyHttpServer()
{
    if (MyStatus_t::SUCCESS == m_status.load()) {
        this->shutdown();
    }
}  
        
MyStatus_t MyHttpServer::init(const MyHttpServAddrCfg_dt& addrCfg)
{
    if (MyStatus_t::SUCCESS == m_status.load()) {
        LOG(WARNING) << "HttpServer has been inited.";
        return MyStatus_t::SUCCESS;
    }

    m_servAddrCfg = addrCfg;
    m_status.store(MyStatus_t::SUCCESS);

    std::string httpServInfo;
    MyServerHelper::PrintHttpServInfo(m_servAddrCfg, httpServInfo);
    LOG(INFO) << "HttpServer init success. " << httpServInfo << ".";

    return MyStatus_t::SUCCESS;
}
    
MyStatus_t MyHttpServer::run()
{
    // 子线程处理http请求，不阻塞当前线程
    MySystemThdPool::ThreadPoolTaskFunc thdPoolFunc = [&]() {
        try {
            MyHttpServer::ServThdFunc(this);
        }
        catch (std::exception& e) {
            LOG(WARNING) << "HttpServer execute thread function exception. " << e.what();
        }
    };
    
    MySystemThdPool::ExecuteTask(thdPoolFunc);

    return MyStatus_t::SUCCESS;
}
    
MyStatus_t MyHttpServer::shutdown()
{
    if (MyStatus_t::FAILED == m_status.load()) {
        LOG(WARNING) << "HttpServer is not init.";
        return MyStatus_t::SUCCESS;
    }

    m_status.store(MyStatus_t::FAILED);

    if (nullptr != m_acceptPtr) {
        m_acceptPtr->close();

        // 等待线程退出
        std::this_thread::sleep_for(std::chrono::seconds(1));
        m_acceptPtr.reset(); 
    }

    std::string httpServInfo;
    MyServerHelper::PrintHttpServInfo(m_servAddrCfg, httpServInfo);
    LOG(INFO) << "HttpServer shutdown success. " << httpServInfo << ".";

    return MyStatus_t::SUCCESS;
}

MyStatus_t MyHttpServer::onRecvHttpGetReq(const std::string& url, http::status& statusCode, std::string& respBody)
{
    if (HTTP_URL_GET_DEVICE_LIST == url) {
        // 获取设备列表
        if (MyStatus_t::SUCCESS == MySipCatalogManageView::GetDeviceListInfo(respBody)) {
            statusCode = http::status::ok;
            return MyStatus_t::SUCCESS;
        }
        
        statusCode = http::status::internal_server_error;
        MyJsonHelper::GenerateHttpErrMsgBody("generate device list info failed.", respBody);
    }
    else if (std::string::npos != url.find(HTTP_URL_GET_DEVICE_INFO)) {
        // 解析设备id
        size_t lastSlashPos = url.find_last_of('/');
        
        if (std::string::npos != lastSlashPos && url.length() - 1 != lastSlashPos) {
            // 获取设备信息
            std::string deviceId = url.substr(lastSlashPos + 1);
            if (MyStatus_t::SUCCESS == MySipCatalogManageView::GetDeviceInfo(deviceId, respBody)) {
                statusCode = http::status::ok;
                return MyStatus_t::SUCCESS;
            }
        }

        statusCode = http::status::not_found;
        MyJsonHelper::GenerateHttpErrMsgBody(std::string("device not exists. url: " + url), respBody);
    }
    else {
        statusCode = http::status::not_found;
        MyJsonHelper::GenerateHttpErrMsgBody(std::string("invalid url: " + url), respBody);
    }

    return MyStatus_t::FAILED;
}

MyStatus_t MyHttpServer::onRecvHttpPostReq(const std::string& url, const std::string& reqBody, http::status& statusCode, std::string& respBody)
{
    // 验证http post请求
    MyHttpReqMediaInfo_dt reqInfo;
    if (MyStatus_t::SUCCESS != MyJsonHelper::ParseHttpReqMediaInfo(reqBody, reqInfo)) {
        statusCode = http::status::bad_request;

        MyJsonHelper::GenerateHttpErrMsgBody("parse http request media info failed: ", respBody);
        return MyStatus_t::FAILED;
    }

    if (std::string::npos == url.find(HTTP_URL_REQ_DEVICE_MEDIA)) {
        statusCode = http::status::bad_request;

        std::string errInfo = "invalid url: " + url;
        MyJsonHelper::GenerateHttpErrMsgBody(errInfo, respBody);
    }

    size_t lastSlashPos = url.find_last_of('/');
    if (std::string::npos == lastSlashPos || url.length() - 1 == lastSlashPos) {
        statusCode = http::status::bad_request;

        std::string errInfo = "parse http url failed: " + url;
        MyJsonHelper::GenerateHttpErrMsgBody(errInfo, respBody);
        return MyStatus_t::FAILED;
    }

    // 获取设备信息
    std::string deviceId = url.substr(lastSlashPos + 1);

    // 请求设备媒体流
    std::string procInfo;
    MyStatus_t procStatus = MyStatus_t::FAILED;

    // 触发事件通知，外部请求设备媒体流
    NOTICE_EMIT(MY_MEDIA_REQ_EV_ARGS, MY_EVENT::kMyMediaRequestEvent, deviceId, reqInfo, procStatus, procInfo);

    if (MyStatus_t::SUCCESS == procStatus) {
        statusCode = http::status::ok;
        return MyStatus_t::SUCCESS;
    }
    else {
        statusCode = http::status::internal_server_error;
        MyJsonHelper::GenerateHttpErrMsgBody(procInfo, respBody);
        return MyStatus_t::FAILED;
    }
}
    
MyStatus_t MyHttpServer::getState(MyStatus_t& status) const
{
    status = m_status.load();
    return MyStatus_t::SUCCESS;
}
    
MyStatus_t MyHttpServer::getServAddrCfg(MyHttpServAddrCfg_dt& cfg)
{
    if (MyStatus_t::FAILED == m_status.load()) {
        LOG(ERROR) << "HttpServer is not init.";
        return MyStatus_t::FAILED;
    }

    boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

    cfg = m_servAddrCfg;
    return MyStatus_t::SUCCESS;
}
    
}; // namespace MY_SERVER