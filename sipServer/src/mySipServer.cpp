#include <thread>
#include <chrono>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySystemPjsip.h"
#include "manager/myAppManage.h"
#include "manager/myServManage.h"
#include "utils/mySipServerHelper.h"
#include "server/mySipServer.h"
using namespace MY_COMMON;
using MY_ENVIR::MySystemPjsip;
using MY_MANAGER::MyAppManage;
using MY_MANAGER::MyServManage;
using MY_UTILS::MySipServerHelper;

namespace MY_SERVER {

int MySipServer::SipServerThdFunc(MySipCbParamPtr arg)
{
    if (nullptr == arg) {
        LOG(ERROR) << "SipServerThdFunc() error. arg is invalid.";
        return -1;
    }

    // 获取sip服务地址
    MySipServAddrCfgPtr servAddrCfgPtr = static_cast<MySipServAddrCfgPtr>(arg);
    if (nullptr == servAddrCfgPtr) {
        LOG(ERROR) << "SipServerThdFunc() error. servAddrCfg is invalid.";
        return -1;
    }

    // new in: MySipServer::run()
    std::unique_ptr<MySipServAddrCfg_dt> memManagePtr(servAddrCfgPtr);

    // 获取sip服务
    MySipServer::SmtWkPtr servWkPtr;
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServer(servAddrCfgPtr->id, servWkPtr)) {
        LOG(ERROR) << "SipServerThdFunc() error. MyServManage::GetSipServer() error.";
        return -1;
    }

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "SipServerThdFunc() error. MySystemPjsip::GetPjsipEndptPtr() error.";
        return -1;
    }

    while(true) {
        // sip服务有效
        if (servWkPtr.expired()) {
            LOG(INFO) << "SipServerThdFunc() exit. sip server is invalid.";
            break;
        }

        // 获取sip服务状态成功
        MyStatus_t status = MyStatus_t::FAILED;
        if (MyStatus_t::FAILED == servWkPtr.lock()->getState(status)) {
            LOG(INFO) << "SipServerThdFunc() exit. sip server is shutdown.";
            break;
        }

        // sip服务状态有效
        if (MyStatus_t::SUCCESS != status) {
            LOG(INFO) << "SipServerThdFunc() exit. sip server is invalid.";
            break;
        }

        // 事件处理
        pj_time_val timeout = {0, 500};
        if(PJ_SUCCESS != pjsip_endpt_handle_events(endptPtr, &timeout)) {
            LOG(ERROR) << "SipServerThdFunc() exit. pjsip_endpt_handle_events failed.";
            break;
        }
    }
    return 0;
}

MySipServer::MySipServer() : m_status(MyStatus_t::FAILED), m_servThdPoolPtr(nullptr),
                             m_servEvThdPtr(nullptr), m_servEndptPtr(nullptr)
{
    
}

MySipServer::~MySipServer()
{
    if (MyStatus_t::SUCCESS == m_status.load()) {
        this->shutdown();
    }
}

MyStatus_t MySipServer::init(const MySipServAddrCfg_dt& addrCfg, const MySipEvThdMemCfg_dt& evThdMemCfg)
{
    if (MyStatus_t::SUCCESS == m_status.load()) {
        LOG(WARNING) << "SipServer has been inited.";
        return MyStatus_t::SUCCESS;
    }

    // 服务配置初始化
    m_servAddrCfg = addrCfg;

    // pjsip模块初始化
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&m_servEndptPtr)) {
        LOG(ERROR) << "SipServer init failed. MySystemPjsip::GetPjsipEndptPtr() error.";
        return MyStatus_t::FAILED;
    }
    // 传输层模块初始化
    pj_sockaddr_in addr;
    pj_bzero(&addr, sizeof(addr));

    pj_str_t addrstr = pj_str(const_cast<char *>(m_servAddrCfg.ipAddr.c_str()));
    pj_inet_pton(PJ_AF_INET, &addrstr, &addr.sin_addr);

    addr.sin_family = pj_AF_INET();
    addr.sin_port   = pj_htons(m_servAddrCfg.port);

    // 用于本地消息发送和接收
    if(PJ_SUCCESS != pjsip_udp_transport_start(m_servEndptPtr, &addr, nullptr, 1, nullptr)) {
        LOG(ERROR) << "SipServer init failed. pjsip_udp_transport_start() error.";
        return MyStatus_t::FAILED;
    }

    if(PJ_SUCCESS != pjsip_tcp_transport_start(m_servEndptPtr, &addr, 1, nullptr)) {
        LOG(ERROR) << "SipServer init failed. pjsip_tcp_transport_start() error.";
        return MyStatus_t::FAILED;
    }

    if (m_servAddrCfg.regPort > 0) {
        pj_sockaddr_in regAddr;
        pj_bzero(&regAddr, sizeof(regAddr));
    
        pj_str_t regAddrstr = pj_str(const_cast<char *>(m_servAddrCfg.ipAddr.c_str()));
        pj_inet_pton(PJ_AF_INET, &regAddrstr, &addr.sin_addr);
    
        regAddr.sin_family = pj_AF_INET();
        regAddr.sin_port   = pj_htons(m_servAddrCfg.regPort);
    
        // 用于监听下级sip服务注册
        if(PJ_SUCCESS != pjsip_udp_transport_start(m_servEndptPtr, &regAddr, nullptr, 1, nullptr)) {
            LOG(ERROR) << "SipServer init failed. pjsip_udp_transport_start() for sip regist error.";
            return MyStatus_t::FAILED;
        }
    
        if(PJ_SUCCESS != pjsip_tcp_transport_start(m_servEndptPtr, &regAddr, 1, nullptr)) {
            LOG(ERROR) << "SipServer init failed. pjsip_tcp_transport_start() for sip regist error.";
            return MyStatus_t::FAILED;
        }
    }

    // 事件线程池初始化
    std::string thdPoolName;
    MySipServerHelper::GetSipServThdPoolName(m_servAddrCfg, thdPoolName);
    m_servThdPoolPtr = pjsip_endpt_create_pool(m_servEndptPtr, thdPoolName.c_str(), 
                                               evThdMemCfg.sipEvThdInitSize, evThdMemCfg.sipEvThdIncreSize);
    if (nullptr == m_servThdPoolPtr) {
        LOG(ERROR) << "SipServer init failed. pjsip_endpt_create_pool error.";
        return MyStatus_t::FAILED;
    }

    m_status.store(MyStatus_t::SUCCESS);

    std::string sipServInfo;
    MySipServerHelper::GetSipServInfo(m_servAddrCfg, sipServInfo);
    LOG(INFO) << "SipServer init success. " << sipServInfo << ".";

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::run()
{
    std::string sipServInfo;
    MySipServerHelper::GetSipServInfo(m_servAddrCfg, sipServInfo);

    if ( MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "SipServer is not init. " << sipServInfo << ".";
        return MyStatus_t::FAILED;
    }

    // delete in: MySipServer::SipServerThdFunc()
    MySipServAddrCfgPtr servAddrCfgPtr = new MySipServAddrCfg_dt(m_servAddrCfg);

    // 服务需要事件循环触发消息发送和接收回调
    if (PJ_SUCCESS != pj_thread_create(m_servThdPoolPtr, nullptr, &MySipServer::SipServerThdFunc, servAddrCfgPtr, 0, 0, &m_servEvThdPtr)) {
        LOG(ERROR) << "SipServer run failed. pj_thread_create() error. " << sipServInfo << ".";
        return MyStatus_t::FAILED;
    }

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::shutdown()
{
    if (MyStatus_t::FAILED == m_status.load()) {
        LOG(WARNING) << "SipServer is not init.";
        return MyStatus_t::SUCCESS;
    }
    m_status.store(MyStatus_t::FAILED);

    // 等待所有线程退出
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (nullptr != m_servEvThdPtr) {
        pj_thread_join(m_servEvThdPtr);
        pj_thread_destroy(m_servEvThdPtr);
        m_servEvThdPtr = nullptr;
    }

    if (nullptr != m_servThdPoolPtr) {
        pjsip_endpt_release_pool(m_servEndptPtr, m_servThdPoolPtr);
        m_servThdPoolPtr = nullptr;
    }

    m_servEndptPtr = nullptr;

    std::string sipServInfo;
    MySipServerHelper::GetSipServInfo(m_servAddrCfg, sipServInfo);
    LOG(INFO) << "SipServer shutdown success. " << sipServInfo << ".";
    
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::onSipServRecvSipRegisterReqMsg(MySipRxDataPtr regReqMsgPtr)
{
    MySipRegApp::SmtWkPtr sipRegAppWkPtr;
    if (MyStatus_t::SUCCESS != MyAppManage::GetSipRegApp(m_servAddrCfg.id, sipRegAppWkPtr)) {
        LOG(ERROR) << "SipServer recv sip register request failed. GetSipRegApp() error.";
        return MyStatus_t::FAILED;
    }
    return sipRegAppWkPtr.lock()->onRecvSipRegReqMsg(regReqMsgPtr);
}

MyStatus_t MySipServer::onSipServRecvSipKeepAliveReqMsg(MySipRxDataPtr keepAliveReqMsgPtr)
{
    MySipRegApp::SmtWkPtr sipRegAppWkPtr;
    if (MyStatus_t::SUCCESS != MyAppManage::GetSipRegApp(m_servAddrCfg.id, sipRegAppWkPtr)) {
        LOG(ERROR) << "SipServer recv sip keepAlive request failed. sipRegApp invalid.";
        return MyStatus_t::FAILED;
    }
    return sipRegAppWkPtr.lock()->onSipRegAppRecvSipKeepAliveReqMsg(keepAliveReqMsgPtr);
}

MyStatus_t MySipServer::onSipServRecvSipCatalogQueryReqMsg(MySipRxDataPtr catalogQueryReqMsgPtr)
{
    MySipCatalogApp::SmtWkPtr sipCatalogAppWkPtr;
    if (MyStatus_t::SUCCESS != MyAppManage::GetSipCatalogApp(m_servAddrCfg.id, sipCatalogAppWkPtr)) {
        LOG(ERROR) << "SipServer recv sip catalog query request failed. sipCatalogApp invalid.";
        return MyStatus_t::FAILED;
    }
    return sipCatalogAppWkPtr.lock()->onSipCatalogAppRecvSipCatalogQueryReqMsg(catalogQueryReqMsgPtr);
}
MyStatus_t MySipServer::onSipServRecvSipCatalogResponseReqMsg(MySipRxDataPtr catalogResponseReqMsgPtr)
{
    MySipCatalogApp::SmtWkPtr sipCatalogAppWkPtr;
    if (MyStatus_t::SUCCESS != MyAppManage::GetSipCatalogApp(m_servAddrCfg.id, sipCatalogAppWkPtr)) {
        LOG(ERROR) << "SipServer recv sip catalog response request failed. sipCatalogApp invalid.";
        return MyStatus_t::FAILED;
    }
    return sipCatalogAppWkPtr.lock()->onSipCatalogAppRecvSipCatalogResponseReqMsg(catalogResponseReqMsgPtr);
}

MyStatus_t MySipServer::onReqLowSipServCatalog(const MySipRegLowServCfg_dt& lowSipRegServAddrCfg)
{
    MySipCatalogApp::SmtWkPtr sipCatalogAppWkPtr;
    if (MyStatus_t::SUCCESS != MyAppManage::GetSipCatalogApp(m_servAddrCfg.id, sipCatalogAppWkPtr)) {
        LOG(ERROR) << "SipServer onReqLowSipServCatalog() failed. sipCatalogApp invalid.";
        return MyStatus_t::FAILED;
    }
    return sipCatalogAppWkPtr.lock()->onSipCatalogAppReqLowServCatalog(lowSipRegServAddrCfg, m_servAddrCfg);
}

MyStatus_t MySipServer::getState(MyStatus_t& status) const
{
    status = m_status.load();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::getSipServer(MySipServer::SmtWkPtr& sipServer)
{
    if (MyStatus_t::FAILED == m_status.load()) {
        LOG(ERROR) << "SipServer is not init.";
        return MyStatus_t::FAILED;
    }

    sipServer = this->shared_from_this();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::getSipServAddrCfg(MY_COMMON::MySipServAddrCfg_dt& cfg) const
{
    if (MyStatus_t::FAILED == m_status.load()) {
        LOG(ERROR) << "SipServer is not init.";
        return MyStatus_t::FAILED;
    }

    cfg = m_servAddrCfg;
    return MyStatus_t::SUCCESS;
}

}; //namespace MY_SERVER