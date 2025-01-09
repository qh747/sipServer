#include <mutex>
#include <functional>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include "envir/mySystemConfg.h"
#include "app/mySipApp.h"
#include "server/mySipServer.h"
using MY_COMMON::MySipStackConfig_dt;
using MY_ENVIR::MySystemConfig;
using MY_APP::MySipAppWrapper;

namespace MY_SERVER {

MySipServer::MySipServer(bool autoInit) : m_isStarted(MyStatus_t::FAILED), m_cfgPtr(nullptr), m_endpointPtr(nullptr), m_mediaEndptPtr(nullptr)
{
    pj_bzero(&m_cachingPool, sizeof(pj_caching_pool));

    if (autoInit) {
        static std::once_flag SIP_SERVER_INIT_FLAG;
        std::call_once(SIP_SERVER_INIT_FLAG, std::mem_fn(&MySipServer::init), this);
    }
}

MySipServer::~MySipServer()
{
    if (MyStatus_t::SUCCESS == m_isStarted.load()) {
        this->shutdown();
    }
}

MyStatus_t MySipServer::init()
{
    if (MyStatus_t::SUCCESS == m_isStarted.load()) {
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

    // pjsip模块注册
    status = this->registerModule();
    if (MyStatus_t::SUCCESS != status) {
        LOG(ERROR) << "SipServer init failed. registerModule() error.";
        return MyStatus_t::FAILED;
    }

    m_isStarted.store(MyStatus_t::SUCCESS);
    LOG(INFO) << "SipServer init success. name: " << m_cfgPtr->name << " domain: " << m_cfgPtr->domain
              << " ip: " << m_cfgPtr->ipAddr << " port: " << m_cfgPtr->port << " protocol: udp/tcp.";
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::shutdown()
{
    if (MyStatus_t::FAILED == m_isStarted.load()) {
        LOG(WARNING) << "SipServer is not init.";
        return MyStatus_t::SUCCESS;
    }
    m_isStarted.store(MyStatus_t::FAILED);

    MySipAppWrapper::Destory(m_endpointPtr);

    if (nullptr != m_mediaEndptPtr) {
        pjmedia_endpt_destroy(m_mediaEndptPtr);
    }

    if (nullptr != m_endpointPtr) {
        pjsip_endpt_destroy(m_endpointPtr);
    }

    pj_caching_pool_destroy(&m_cachingPool);

    LOG(INFO) << "SipServer shurdown success. name: " << m_cfgPtr->name << " domain: " << m_cfgPtr->domain
              << " ip: " << m_cfgPtr->ipAddr << " port: " << m_cfgPtr->port << " protocol: udp/tcp.";
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::initConfig()
{
    m_cfgPtr = std::make_shared<MyServerAddrConfig_dt>(MySystemConfig::GetServerAddrConfig());
    if (nullptr == m_cfgPtr) {
        LOG(ERROR) << "SipServer init config failed. server config ptr is null.";
        return MyStatus_t::FAILED;
    }
    else if (m_cfgPtr->ipAddr.empty() || m_cfgPtr->port <= 0 || m_cfgPtr->name.empty() || m_cfgPtr->domain.empty()) {
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
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::registerModule()
{
    if (nullptr == m_endpointPtr) {
        LOG(ERROR) << "SipServer register module failed. m_endpointPtr is null.";
        return MyStatus_t::FAILED;
    }

    MyStatus_t status = MyStatus_t::SUCCESS;

    // 模块注册
    status = MySipAppWrapper::Init(m_endpointPtr, "MySipServerApp", PJSIP_MOD_PRIORITY_APPLICATION);
    if (MyStatus_t::SUCCESS != status) {
        LOG(ERROR) << "SipServer register module failed. MySipAppWrapper::Init() error.";
        return status;
    }

    return status;
}

}; //namespace MY_SERVER