#include <thread>
#include <chrono>
#include <sstream>
#include <functional>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySystemConfg.h"
#include "app/mySipApp.h"
#include "app/mySipReg.h"
#include "server/mySipServer.h"
using MY_APP::MySipAppWrapper;
using MY_APP::MySipRegWrapper;
using MY_ENVIR::MySystemConfig;
using MY_COMMON::MySipStackConfig_dt;

namespace MY_SERVER {

int MySipServer::OnSipServerEvCb(ServEvThreadCbParamPtr args)
{
    // 检查参数是否有效
    if (nullptr == args) {
        LOG(ERROR) << "SipServer start listen failed. input invalid param.";
        return static_cast<int>(MyStatus_t::FAILED);
    }

    ServPtr servPtr = static_cast<ServPtr>(args);
    
    // 检查服务是否已经初始化完成
    if (MyStatus_t::SUCCESS != servPtr->m_startState.load()) {
        LOG(ERROR) << "SipServer start listen failed. server is not init.";
        return static_cast<int>(MyStatus_t::FAILED);
    }
    else {
        LOG(INFO) << "SipServer start listen success. " << servPtr->getServerInfo() << ".";;
    }

    while (true) {
        // 服务如果停止则取消监听事件
        if (MyStatus_t::SUCCESS != servPtr->m_startState.load()) {
            LOG(INFO) << "SipServer stop listen.";
            break;
        }

        pj_time_val timeout = {0, 500};
        if(PJ_SUCCESS != pjsip_endpt_handle_events(servPtr->m_endpointPtr, &timeout)) {
            LOG(ERROR) << "SipServer listen failed, pjsip_endpt_handle_events() error.";
            return static_cast<int>(MyStatus_t::FAILED);
        }
    }

    return static_cast<int>(MyStatus_t::SUCCESS);
}

MySipServer::MySipServer(bool autoInit) : m_startState(MyStatus_t::FAILED), m_cfgPtr(nullptr), m_endpointPtr(nullptr), m_mediaEndptPtr(nullptr),
                                          m_servThreadPoolPtr(nullptr), m_evThreadPtr(nullptr)
{
    pj_bzero(&m_cachingPool, sizeof(pj_caching_pool));

    if (autoInit) {
        static std::once_flag SIP_SERVER_INIT_FLAG;
        std::call_once(SIP_SERVER_INIT_FLAG, std::mem_fn(&MySipServer::init), this);
    }
}

MySipServer::~MySipServer()
{
    if (MyStatus_t::SUCCESS == m_startState.load()) {
        this->shutdown();
    }
}

MyStatus_t MySipServer::init()
{
    if (MyStatus_t::SUCCESS == m_startState.load()) {
        LOG(WARNING) << "SipServer has been inited.";
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t status = MyStatus_t::SUCCESS;

    // 服务配置初始化
    status = this->initConfig();
    if (MyStatus_t::SUCCESS != status) {
        LOG(ERROR) << "SipServer init failed. initConfig() error.";
        return MyStatus_t::FAILED;
    }

    // pjsip库初始化
    status = this->initPjsipLib();
    if (MyStatus_t::SUCCESS != status) {
        LOG(ERROR) << "SipServer init failed. initPjsipLib() error.";
        return MyStatus_t::FAILED;
    }

    // pjsip模块初始化
    status = this->initModule();
    if (MyStatus_t::SUCCESS != status) {
        LOG(ERROR) << "SipServer init failed. initModule() error.";
        return MyStatus_t::FAILED;
    }

    // 服务线程池初始化
    m_servThreadPoolPtr = pjsip_endpt_create_pool(m_endpointPtr, "MySipServerThreadPool", MY_COMMON::SIPSERV_EVTRD_INIT_MEM_SIZE, MY_COMMON::SIPSERV_EVTRD_INCREM_MEM_SIZE);
    if (nullptr == m_servThreadPoolPtr) {
        LOG(ERROR) << "SipServer init thread failed. pjsip_endpt_create_pool error.";
        return MyStatus_t::FAILED;
    }

    m_startState.store(MyStatus_t::SUCCESS);
    LOG(INFO) << "SipServer init success.";
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::run()
{
    // 判断服务端线程池是否创建成功
    if (nullptr == m_servThreadPoolPtr) {
        LOG(ERROR) << "SipServer run failed. server threadPool is null.";
        return MyStatus_t::FAILED;
    }

    // 创建事件处理线程
    if (PJ_SUCCESS != pj_thread_create(m_servThreadPoolPtr, "MySipServerThread", &MySipServer::OnSipServerEvCb, this, PJ_THREAD_DEFAULT_STACK_SIZE, 0, &m_evThreadPtr)) {
        LOG(ERROR) << "SipServer run thread failed. pj_thread_create() error.";
        return MyStatus_t::FAILED;
    }

    // 运行应用注册模块
    if (MyStatus_t::SUCCESS != MySipRegWrapper::Run(m_servThreadPoolPtr)) {
        LOG(ERROR) << "SipServer run failed. MySipRegWrapper::Run() error.";
        return MyStatus_t::FAILED;
    }

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::shutdown()
{
    if (MyStatus_t::FAILED == m_startState.load()) {
        LOG(WARNING) << "SipServer is not init.";
        return MyStatus_t::SUCCESS;
    }
    m_startState.store(MyStatus_t::FAILED);

    // 等待所有线程退出
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 停止应用注册模块
    MySipRegWrapper::Shutdown();

    // 停止应用模块
    MySipAppWrapper::Shutdown(m_endpointPtr);

    if (nullptr != m_evThreadPtr) {
        pj_thread_join(m_evThreadPtr);
        pj_thread_destroy(m_evThreadPtr);
        m_evThreadPtr = nullptr;
    }

    if (nullptr != m_servThreadPoolPtr) {
        pjsip_endpt_release_pool(m_endpointPtr, m_servThreadPoolPtr);
        m_servThreadPoolPtr = nullptr;
    }

    if (nullptr != m_mediaEndptPtr) {
        pjmedia_endpt_destroy(m_mediaEndptPtr);
        m_mediaEndptPtr = nullptr;
    }

    if (nullptr != m_endpointPtr) {
        pjsip_endpt_destroy(m_endpointPtr);
        m_endpointPtr = nullptr;
    }

    pj_caching_pool_destroy(&m_cachingPool);

    LOG(INFO) << "SipServer shutdown success. " << this->getServerInfo() << ".";
    return MyStatus_t::SUCCESS;
}

std::string MySipServer::getServerInfo()
{
    std::stringstream ss;
    ss << "name: " << m_cfgPtr->name << " domain: " << m_cfgPtr->domain << " id: " << m_cfgPtr->id
       << " ip: " << m_cfgPtr->ipAddr << " port: " << m_cfgPtr->port << " protocol: udp/tcp";

    return ss.str();
}

MyStatus_t MySipServer::initConfig()
{
    m_cfgPtr = std::make_shared<MyServerAddrConfig_dt>(MySystemConfig::GetServerAddrConfig());
    if (nullptr == m_cfgPtr) {
        LOG(ERROR) << "SipServer init config failed. server config ptr is null.";
        return MyStatus_t::FAILED;
    }
    else if (m_cfgPtr->id.empty() || m_cfgPtr->ipAddr.empty() || m_cfgPtr->port <= 0 || m_cfgPtr->name.empty() || m_cfgPtr->domain.empty()) {
        LOG(ERROR) << "SipServer init config failed. server config is invalid.";
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::initPjsipLib()
{
    pj_status_t result = PJ_SUCCESS;

    // pjlib初始化
    result = pj_init();
    if (PJ_SUCCESS != result) {
        LOG(ERROR) << "SipServer init lib failed. pj_init() error. Code: " << result << ".";
        return MyStatus_t::FAILED;
    }

    // pjlib util初始化
    result = pjlib_util_init();
    if (PJ_SUCCESS != result) {
        LOG(ERROR) << "SipServer init lib failed. pjlib_util_init() error. Code: " << result << ".";
        return MyStatus_t::FAILED;
    }

    // pjsip内存池初始化
    const MySipStackConfig_dt& sipStackConfig = MySystemConfig::GetSipStackConfig();
    pj_caching_pool_init(&m_cachingPool, nullptr, sipStackConfig.sipStackSize);

    // pjsip endpoint初始化
    result = pjsip_endpt_create(&m_cachingPool.factory, sipStackConfig.sipStackName.c_str(), &m_endpointPtr);
    if (PJ_SUCCESS != result) {
        LOG(ERROR) << "SipServer init lib failed. pjsip_endpt_create() error. Code: " << result << ".";
        return MyStatus_t::FAILED;
    }

    // pjsip media endpoint初始化
    result = pjmedia_endpt_create(&m_cachingPool.factory, pjsip_endpt_get_ioqueue(m_endpointPtr), 0, &m_mediaEndptPtr);
    if (PJ_SUCCESS != result) {
        LOG(ERROR) << "SipServer init lib failed. pjmedia_endpt_create() error. Code: " << result << ".";
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::initModule()
{
    if (nullptr == m_endpointPtr) {
        LOG(ERROR) << "SipServer init module failed. m_endpointPtr is null.";
        return MyStatus_t::FAILED;
    }

    pj_status_t result = PJ_SUCCESS;

    // 传输层模块初始化
    pj_sockaddr_in addr;
    pj_bzero(&addr, sizeof(addr));

    pj_str_t pjAddrstr = pj_str(const_cast<char *>(m_cfgPtr->ipAddr.c_str()));
    pj_inet_pton(PJ_AF_INET, &pjAddrstr, &addr.sin_addr);

    addr.sin_family = pj_AF_INET();
    addr.sin_port   = pj_htons(m_cfgPtr->port);

    result = pjsip_udp_transport_start(m_endpointPtr, &addr, nullptr, 1, nullptr);
    if(PJ_SUCCESS != result) {
        LOG(ERROR) << "SipServer init module failed. pjsip_udp_transport_start() error. Code: " << result << ".";
        return MyStatus_t::FAILED;
    }

    result = pjsip_tcp_transport_start(m_endpointPtr, &addr, 1, nullptr);
    if(PJ_SUCCESS != result) {
        LOG(ERROR) << "SipServer init module failed. pjsip_tcp_transport_start() error. Code: " << result << ".";
        return MyStatus_t::FAILED;
    }

    // 事务层模块初始化
    result = pjsip_tsx_layer_init_module(m_endpointPtr);
    if (PJ_SUCCESS != result) {
        LOG(ERROR) << "SipServer init module failed. pjsip_tsx_layer_init_module() error. Code: " << result << ".";
        return MyStatus_t::FAILED;
    }

    // 用户代理层模块初始化
    result = pjsip_ua_init_module(m_endpointPtr, nullptr);
    if (PJ_SUCCESS != result) {
        LOG(ERROR) << "SipServer init module failed. pjsip_ua_init_module() error. Code: " << result << ".";
        return MyStatus_t::FAILED;
    }

    // sip 100 trying临时应答模块初始化
    result = pjsip_100rel_init_module(m_endpointPtr);
    if(PJ_SUCCESS != result) {
        LOG(ERROR) << "SipServer init module failed. pjsip_100rel_init_module() error. Code: " << result << ".";
        return MyStatus_t::FAILED;
    }

    // 应用层模块初始化
    if (MyStatus_t::SUCCESS != MySipAppWrapper::Init(m_endpointPtr, "MySipServerApp", PJSIP_MOD_PRIORITY_APPLICATION)) {
        LOG(ERROR) << "SipServer init module failed. MySipAppWrapper::Init() error.";
        return MyStatus_t::FAILED;
    }

    // 应用层注册模块初始化
    if (MyStatus_t::SUCCESS != MySipRegWrapper::Init(MySystemConfig::GetServerRegisterFile())) {
        LOG(ERROR) << "SipServer init module failed. MySipRegWrapper::Init() error.";
        return MyStatus_t::FAILED;
    }

    return MyStatus_t::SUCCESS;
}

}; //namespace MY_SERVER