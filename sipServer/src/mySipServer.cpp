#include <thread>
#include <chrono>
#include <sstream>
#include <functional>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySystemAppManage.h"
#include "utils/mySipServerHelper.h"
#include "app/mySipRegApp.h"
#include "server/mySipServer.h"
using MY_COMMON::MyStatus_t;
using MY_ENVIR::MySystemPjsip;
using MY_ENVIR::MySystemAppManage;
using MY_UTILS::MySipServerHelper;
using MY_APP::MySipRegApp;

namespace MY_SERVER {

int MySipServer::OnSipServerEvCb(SipServEvThreadCbParamPtr args)
{
    // 检查服务是否已经初始化完成
    SipServPtr servPtr = static_cast<SipServPtr>(args);
    if (MyStatus_t::SUCCESS != servPtr->getState()) {
        LOG(ERROR) << "SipServer start listen failed. server is not init.";
        return static_cast<int>(MyStatus_t::FAILED);
    }
    LOG(INFO) << "SipServer start listen success. " << MySipServerHelper::GetSipServInfo(servPtr->getSipServAddrCfg()) << ".";

    while (true) {
        // 服务如果停止则取消监听事件
        if (MyStatus_t::SUCCESS != servPtr->getState()) {
            LOG(INFO) << "SipServer stop listen. " << MySipServerHelper::GetSipServInfo(servPtr->getSipServAddrCfg()) << ".";
            break;
        }

        pj_time_val timeout = {0, 500};
        SipServEndptPtr endptPtr = servPtr->getSipServEndptPtr();
        
        if(nullptr == endptPtr || PJ_SUCCESS != pjsip_endpt_handle_events(endptPtr, &timeout)) {
            LOG(ERROR) << "SipServer listen failed, pjsip_endpt_handle_events() error.";
            return static_cast<int>(MyStatus_t::FAILED);
        }
    }
    return static_cast<int>(MyStatus_t::SUCCESS);
}

MySipServer::MySipServer() : m_status(MyStatus_t::FAILED), m_servThdPoolPtr(nullptr),
                             m_servEvThdPtr(nullptr), m_servEndptPtr(nullptr),
                             m_servAddrCfgPtr(nullptr)
{
    
}

MySipServer::~MySipServer()
{
    if (MyStatus_t::SUCCESS == m_status.load()) {
        this->shutdown();
    }
}

MyStatus_t MySipServer::init(const SipServAddrCfg& addrCfg, const SipServEvThdMemCfg& evThdMemCfg)
{
    if (MyStatus_t::SUCCESS == m_status.load()) {
        LOG(WARNING) << "SipServer has been inited.";
        return MyStatus_t::SUCCESS;
    }

    // 服务配置初始化
    m_servAddrCfgPtr = std::make_shared<SipServAddrCfg>(addrCfg);

    // pjsip模块初始化
    m_servEndptPtr = MySystemPjsip::GetPjsipEndptPtr();
    if (nullptr == m_servEndptPtr) {
        LOG(ERROR) << "SipServer init failed. m_endpointPtr is null.";
        return MyStatus_t::FAILED;
    }

    // 传输层模块初始化
    pj_sockaddr_in addr;
    pj_bzero(&addr, sizeof(addr));

    pj_str_t addrstr = pj_str(const_cast<char *>(m_servAddrCfgPtr->ipAddr.c_str()));
    pj_inet_pton(PJ_AF_INET, &addrstr, &addr.sin_addr);

    addr.sin_family = pj_AF_INET();
    addr.sin_port   = pj_htons(m_servAddrCfgPtr->port);

    // 用于本地消息发送和接收
    if(PJ_SUCCESS != pjsip_udp_transport_start(m_servEndptPtr, &addr, nullptr, 1, nullptr)) {
        LOG(ERROR) << "SipServer init failed. pjsip_udp_transport_start() error.";
        return MyStatus_t::FAILED;
    }

    if(PJ_SUCCESS != pjsip_tcp_transport_start(m_servEndptPtr, &addr, 1, nullptr)) {
        LOG(ERROR) << "SipServer init failed. pjsip_tcp_transport_start() error.";
        return MyStatus_t::FAILED;
    }

    if (m_servAddrCfgPtr->regPort > 0) {
        pj_sockaddr_in regAddr;
        pj_bzero(&regAddr, sizeof(regAddr));
    
        pj_str_t regAddrstr = pj_str(const_cast<char *>(m_servAddrCfgPtr->ipAddr.c_str()));
        pj_inet_pton(PJ_AF_INET, &regAddrstr, &addr.sin_addr);
    
        regAddr.sin_family = pj_AF_INET();
        regAddr.sin_port   = pj_htons(m_servAddrCfgPtr->regPort);
    
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
    m_servThdPoolPtr = pjsip_endpt_create_pool(m_servEndptPtr, MySipServerHelper::GetSipServThdPoolName(*m_servAddrCfgPtr).c_str(), 
                                               evThdMemCfg.sipEvThdInitSize, evThdMemCfg.sipEvThdIncreSize);
    if (nullptr == m_servThdPoolPtr) {
        LOG(ERROR) << "SipServer init failed. pjsip_endpt_create_pool error.";
        return MyStatus_t::FAILED;
    }

    m_status.store(MyStatus_t::SUCCESS);
    LOG(INFO) << "SipServer init success. " << MySipServerHelper::GetSipServInfo(*m_servAddrCfgPtr) << ".";

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::run()
{
    if (PJ_SUCCESS != pj_thread_create(m_servThdPoolPtr, MySipServerHelper::GetSipServEvThdName(*m_servAddrCfgPtr).c_str(), 
                                       &MySipServer::OnSipServerEvCb, this, PJ_THREAD_DEFAULT_STACK_SIZE, 0, &m_servEvThdPtr)) {
        LOG(ERROR) << "SipServer run failed. pj_thread_create() error.";
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

    LOG(INFO) << "SipServer shutdown success. " << MySipServerHelper::GetSipServInfo(*m_servAddrCfgPtr) << ".";
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::onRecvSipRegMsg(SipAppRxDataPtr regReqMsgPtr)
{
    auto sipRegAppWkPtr = MySystemAppManage::GetSipRegApp(m_servAddrCfgPtr->id);
    if (sipRegAppWkPtr.expired()) {
        LOG(ERROR) << "SipServer onRecvSipRegReq() failed. sipRegAppWkPtr expired.";
        return MyStatus_t::FAILED;
    }

    auto sipRegAppPtr = sipRegAppWkPtr.lock();
    return sipRegAppPtr->onRecvSipRegReqMsg(regReqMsgPtr);
}

}; //namespace MY_SERVER