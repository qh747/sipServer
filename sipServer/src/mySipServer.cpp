#include <mutex>
#include <functional>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include "envir/mySystemConfg.h"
#include "server/mySipServer.h"
using MY_COMMON::MySipStackConfig_dt;
using MY_ENVIR::MySystemConfig;

namespace MY_SERVER {

MySipServer::MySipServer(bool autoInit) : m_isStarted(false), m_cfgPtr(nullptr), m_endpointPtr(nullptr), m_mediaEndptPtr(nullptr)
{
    pj_bzero(&m_cachingPool, sizeof(pj_caching_pool));

    if (autoInit) {
        static std::once_flag SIP_SERVER_INIT_FLAG;
        std::call_once(SIP_SERVER_INIT_FLAG, std::mem_fn(&MySipServer::init), this);
    }
}

MySipServer::~MySipServer()
{
    if (m_isStarted.load()) {
        this->shutdown();
    }
}

bool MySipServer::init()
{
    if (m_isStarted.load()) {
        LOG(WARNING) << "SipServer has been inited.";
        return true;
    }

    pj_status_t status = PJ_SUCCESS;

    // 服务配置初始化
    status = this->initConfig();
    if (PJ_SUCCESS != status) {
        LOG(ERROR) << "SipServer init failed. initConfig() error: " << status;
        return false;
    }

    // pjsip库初始化
    status = this->initPjsipLib();
    if (PJ_SUCCESS != status) {
        LOG(ERROR) << "SipServer init failed. initPjsipLib() error: " << status;
        return false;
    }

    // pjsip模块初始化
    status = this->initModule();
    if (PJ_SUCCESS != status) {
        LOG(ERROR) << "SipServer init failed. initModule() error: " << status;
        return false;
    }

    // pjsip模块注册
    status = this->registerModule();
    if (PJ_SUCCESS != status) {
        LOG(ERROR) << "SipServer init failed. registerModule() error: " << status;
        return false;
    }

    m_isStarted.store(true);
    LOG(INFO) << "SipServer init success. name: " << m_cfgPtr->name << " domain: " << m_cfgPtr->domain
              << " ip: " << m_cfgPtr->ipAddr << " port: " << m_cfgPtr->port << " protocol: udp/tcp";
    return true;
}

bool MySipServer::shutdown()
{
    if (!m_isStarted.load()) {
        LOG(WARNING) << "SipServer is not init.";
        return true;
    }
    m_isStarted.store(false);

    if (nullptr != m_mediaEndptPtr) {
        pjmedia_endpt_destroy(m_mediaEndptPtr);
    }

    if (nullptr != m_endpointPtr) {
        pjsip_endpt_destroy(m_endpointPtr);
    }

    pj_caching_pool_destroy(&m_cachingPool);

    LOG(INFO) << "SipServer shurdown success. name: " << m_cfgPtr->name << " domain: " << m_cfgPtr->domain
              << " ip: " << m_cfgPtr->ipAddr << " port: " << m_cfgPtr->port << " protocol: udp/tcp";
    return true;
}

pj_status_t MySipServer::initConfig()
{
    m_cfgPtr = std::make_shared<MyServerAddrConfig_dt>(MySystemConfig::GetServerAddrConfig());
    if (nullptr == m_cfgPtr) {
        LOG(ERROR) << "SipServer init config failed. server config ptr is null";
        return -1;
    }
    else if (m_cfgPtr->ipAddr.empty() || m_cfgPtr->port <= 0 || m_cfgPtr->name.empty() || m_cfgPtr->domain.empty()) {
        LOG(ERROR) << "SipServer init config failed. server config is invalid";
        return -1;
    }
    return PJ_SUCCESS;
}

pj_status_t MySipServer::initPjsipLib()
{
    pj_status_t status = PJ_SUCCESS;

    // pjlib初始化
    status = pj_init();
    if (PJ_SUCCESS != status) {
        LOG(ERROR) << "SipServer init lib failed. pj_init() error: " << status;
        return status;
    }

    // pjlib util初始化
    status = pjlib_util_init();
    if (PJ_SUCCESS != status) {
        LOG(ERROR) << "SipServer init lib failed. pjlib_util_init() error: " << status;
        return status;
    }

    // pjsip内存池初始化
    const MySipStackConfig_dt& sipStackConfig = MySystemConfig::GetSipStackConfig();
    pj_caching_pool_init(&m_cachingPool, nullptr, sipStackConfig.sipStackSize);

    // pjsip endpoint初始化
    status = pjsip_endpt_create(&m_cachingPool.factory, sipStackConfig.sipStackName.c_str(), &m_endpointPtr);
    if (PJ_SUCCESS != status) {
        LOG(ERROR) << "SipServer init lib failed. pjsip_endpt_create() error: " << status;
        return status;
    }

    // pjsip media endpoint初始化
    status = pjmedia_endpt_create(&m_cachingPool.factory, pjsip_endpt_get_ioqueue(m_endpointPtr), 0, &m_mediaEndptPtr);
    if (PJ_SUCCESS != status) {
        LOG(ERROR) << "SipServer init lib failed. pjmedia_endpt_create() error: " << status;
        return status;
    }
    return status;
}

pj_status_t MySipServer::initModule()
{
    if (nullptr == m_endpointPtr) {
        LOG(ERROR) << "SipServer init module failed. m_endpointPtr is null.";
        return -1;
    }

    pj_status_t status = PJ_SUCCESS;

    // 事务层模块初始化
    status = pjsip_tsx_layer_init_module(m_endpointPtr);
    if (PJ_SUCCESS != status) {
        LOG(ERROR) << "SipServer init module failed. pjsip_tsx_layer_init_module() error: " << status;
        return status;
    }

    // 用户代理层模块初始化
    status = pjsip_ua_init_module(m_endpointPtr, nullptr);
    if (PJ_SUCCESS != status) {
        LOG(ERROR) << "SipServer init module failed. pjsip_ua_init_module() error: " << status;
        return status;
    }

    // 传输层模块初始化
    pj_sockaddr_in addr;
    pj_bzero(&addr, sizeof(addr));

    pj_str_t pjAddrstr = pj_str(const_cast<char *>(m_cfgPtr->ipAddr.c_str()));
    pj_inet_pton(PJ_AF_INET, &pjAddrstr, &addr.sin_addr);

    addr.sin_family = pj_AF_INET();
    addr.sin_port   = pj_htons(m_cfgPtr->port);

    status = pjsip_udp_transport_start(m_endpointPtr, &addr, nullptr, 1, nullptr);
    if(PJ_SUCCESS != status) {
        LOG(ERROR) << "SipServer init module failed. pjsip_udp_transport_start() error: " << status;
        return status;
    }

    status = pjsip_tcp_transport_start(m_endpointPtr, &addr, 1, nullptr);
    if(PJ_SUCCESS != status) {
        LOG(ERROR) << "SipServer init module failed. pjsip_tcp_transport_start() error: " << status;
        return status;
    }

    // sip 100 trying临时应答模块初始化
    status = pjsip_100rel_init_module(m_endpointPtr);
    if(PJ_SUCCESS != status) {
        LOG(ERROR) << "SipServer init module failed. pjsip_100rel_init_module() error: " << status;
        return status;
    }

    return status;
}

pj_status_t MySipServer::registerModule()
{
    if (nullptr == m_endpointPtr) {
        LOG(ERROR) << "SipServer init module failed. m_endpointPtr is null.";
        return -1;
    }

    pj_status_t status = PJ_SUCCESS;

    return status;
}

}; //namespace MY_SERVER