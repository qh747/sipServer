#include <thread>
#include <chrono>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySystemPjsip.h"
#include "manager/myAppManage.h"
#include "utils/mySipServerHelper.h"
#include "server/mySipServer.h"
using namespace MY_COMMON;
using MY_ENVIR::MySystemPjsip;
using MY_MANAGER::MyAppManage;
using MY_UTILS::MySipServerHelper;

namespace MY_SERVER {

int MySipServer::SipServerThdFunc(MySipEvThdCbParamPtr arg)
{
    if (nullptr == arg) {
        LOG(ERROR) << "SipServerThdFunc() error. arg is invalid.";
        return -1;
    }

    MySipServer::Ptr sipServPtr = static_cast<MySipServer::Ptr>(arg);
    if (nullptr == sipServPtr) {
        LOG(ERROR) << "SipServerThdFunc() error. sipServPtr is invalid.";
        return -1;
    }

    MySipServer::SmtWkPtr sipServSmtWkPtr = sipServPtr->getSipServer();
    if (sipServSmtWkPtr.expired()) {
        LOG(ERROR) << "SipServerThdFunc() error. sipServSmtWkPtr is invalid.";
        return -1;
    }

    MySipServer::SmtPtr sipServSmtPtr = sipServSmtWkPtr.lock();
    MySipEndptPtr       endptPtr      = sipServSmtPtr->getSipEndptPtr();

    while(MyStatus_t::SUCCESS == sipServSmtPtr->getState()) {
        pj_time_val timeout = {0, 500};
        pj_status_t status = pjsip_endpt_handle_events(endptPtr, &timeout);
        if(PJ_SUCCESS != status) {
            LOG(ERROR)<<"pjsip_endpt_handle_events failed, code:" << status;
            return -1;
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
    m_servEndptPtr = MySystemPjsip::GetPjsipEndptPtr();
    if (nullptr == m_servEndptPtr) {
        LOG(ERROR) << "SipServer init failed. m_endpointPtr is invalid.";
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
    m_servThdPoolPtr = pjsip_endpt_create_pool(m_servEndptPtr, MySipServerHelper::GetSipServThdPoolName(m_servAddrCfg).c_str(), 
                                               evThdMemCfg.sipEvThdInitSize, evThdMemCfg.sipEvThdIncreSize);
    if (nullptr == m_servThdPoolPtr) {
        LOG(ERROR) << "SipServer init failed. pjsip_endpt_create_pool error.";
        return MyStatus_t::FAILED;
    }

    m_status.store(MyStatus_t::SUCCESS);
    LOG(INFO) << "SipServer init success. " << MySipServerHelper::GetSipServInfo(m_servAddrCfg) << ".";

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::run()
{
    if ( MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "SipServer is not init. " << MySipServerHelper::GetSipServInfo(m_servAddrCfg) << ".";
        return MyStatus_t::FAILED;
    }

    // 服务需要事件循环触发消息发送和接收回调
    if (PJ_SUCCESS != pj_thread_create(m_servThdPoolPtr, nullptr, &MySipServer::SipServerThdFunc, this, 0, 0, &m_servEvThdPtr)) {
        LOG(ERROR) << "SipServer run failed. pj_thread_create() error. " << MySipServerHelper::GetSipServInfo(m_servAddrCfg) << ".";
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

    LOG(INFO) << "SipServer shutdown success. " << MySipServerHelper::GetSipServInfo(m_servAddrCfg) << ".";
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::onRecvSipRegMsg(MySipRxDataPtr regReqMsgPtr)
{
    auto sipRegAppWkPtr = MyAppManage::GetSipRegApp(m_servAddrCfg.id);
    if (sipRegAppWkPtr.expired()) {
        LOG(ERROR) << "SipServer onRecvSipRegReq() failed. sipRegApp invalid.";
        return MyStatus_t::FAILED;
    }
    return sipRegAppWkPtr.lock()->onRecvSipRegReqMsg(regReqMsgPtr);
}

MyStatus_t MySipServer::onRecvSipKeepAliveMsg(MY_COMMON::MySipRxDataPtr keepAliveReqMsgPtr)
{
    auto sipRegAppWkPtr = MyAppManage::GetSipRegApp(m_servAddrCfg.id);
    if (sipRegAppWkPtr.expired()) {
        LOG(ERROR) << "SipServer onRecvSipKeepAliveMsg() failed. sipRegApp invalid.";
        return MyStatus_t::FAILED;
    }
    return sipRegAppWkPtr.lock()->onRecvSipKeepAliveReqMsg(keepAliveReqMsgPtr);
}

MyStatus_t MySipServer::onRecvSipCatalogMsg(MY_COMMON::MySipRxDataPtr catalogReqMsgPtr)
{
    auto sipCatalogAppWkPtr = MyAppManage::GetSipCatalogApp(m_servAddrCfg.id);
    if (sipCatalogAppWkPtr.expired()) {
        LOG(ERROR) << "SipServer onRecvSipCatalogMsg() failed. sipCatalogApp invalid.";
        return MyStatus_t::FAILED;
    }
    return sipCatalogAppWkPtr.lock()->onRecvSipCatalogReqMsg(catalogReqMsgPtr);
}

}; //namespace MY_SERVER