#include <thread>
#include <chrono>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "event/myEventListener.h"
#include "envir/mySystemPjsip.h"
#include "manager/myAppManage.h"
#include "manager/myServManage.h"
#include "utils/myServerHelper.h"
#include "server/mySipServer.h"
using namespace MY_COMMON;
using MY_EVENT::MyMediaReqEvListener;
using MY_ENVIR::MySystemPjsip;
using MY_MANAGER::MyAppManage;
using MY_MANAGER::MyServManage;
using MY_UTILS::MyServerHelper;

namespace MY_SERVER {

int MySipServer::ServThdFunc(MyFuncCbParamPtr arg)
{
    if (nullptr == arg) {
        LOG(ERROR) << "ServThdFunc() error. arg is invalid.";
        return -1;
    }

    // 获取sip服务地址
    auto servAddrCfgPtr = static_cast<MySipServAddrCfgPtr>(arg);

    // new in: MySipServer::run()
    std::unique_ptr<MySipServAddrCfg_dt> memManagePtr(servAddrCfgPtr);

    // 获取sip服务
    MySipServer::SmtWkPtr servWkPtr;
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServer(servWkPtr)) {
        LOG(ERROR) << "ServThdFunc() error. MyServManage::GetSipServer() error.";
        return -1;
    }

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "ServThdFunc() error. MySystemPjsip::GetPjsipEndptPtr() error.";
        return -1;
    }

    while(true) {
        // sip服务有效
        if (servWkPtr.expired()) {
            LOG(INFO) << "ServThdFunc() exit. sip server is invalid.";
            break;
        }

        // 获取sip服务状态成功
        MyStatus_t status = MyStatus_t::FAILED;
        servWkPtr.lock()->getState(status);

        // sip服务状态有效
        if (MyStatus_t::SUCCESS != status) {
            LOG(INFO) << "ServThdFunc() exit. sip server is invalid.";
            break;
        }

        // 事件处理
        pj_time_val timeout = {0, 50};
        if(PJ_SUCCESS != pjsip_endpt_handle_events(endptPtr, &timeout)) {
            LOG(ERROR) << "ServThdFunc() exit. pjsip_endpt_handle_events failed.";
            break;
        }
    }
    return 0;
}

void MySipServer::ServMediaReqEvFunc(MY_MEDIA_REQ_EV_ARGS)
{
    // 获取sip服务
    MySipServer::SmtWkPtr servWkPtr;
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServer(servWkPtr)) {
        LOG(ERROR) << "ServMediaReqEvFunc() error. MyServManage::GetSipServer() error.";
        return;
    }

    if (servWkPtr.expired()) {
        LOG(ERROR) << "ServMediaReqEvFunc() error. servWkPtr is invalid.";
        return;
    }

    // 服务状态校验
    MySipServer::SmtPtr servPtr = servWkPtr.lock();

    MyStatus_t servState = MyStatus_t::FAILED;
    servPtr->getState(servState);

    if (MyStatus_t::SUCCESS != servState) {
        LOG(ERROR) << "ServMediaReqEvFunc() error. server state invalid.";
        return;
    }

    // 请求处理
    if (MyStatus_t::SUCCESS != servPtr->onReqDeviceMedia(deviceId, reqInfo, respInfo)) {
        status = MyStatus_t::FAILED;
        return;
    }
    else {
        status = MyStatus_t::SUCCESS;
    }
    
    LOG(INFO) << "ServMediaReqEvFunc() success. request media device id: " << deviceId << ".";
}

MySipServer::MySipServer() : m_status(MyStatus_t::FAILED), m_servThdPoolPtr(nullptr),
    m_servEvThdPtr(nullptr), m_servEndptPtr(nullptr), m_servUdpTpPtr(nullptr), m_servTcpTpFactoryPtr(nullptr),
    m_servUdpRegTpPtr(nullptr), m_servTcpRegTpFactoryPtr(nullptr)
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
    if(PJ_SUCCESS != pjsip_udp_transport_start(m_servEndptPtr, &addr, nullptr, 1, &m_servUdpTpPtr)) {
        LOG(ERROR) << "SipServer init failed. pjsip_udp_transport_start() error.";
        return MyStatus_t::FAILED;
    }

    if(PJ_SUCCESS != pjsip_tcp_transport_start(m_servEndptPtr, &addr, 1, &m_servTcpTpFactoryPtr)) {
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
        if(PJ_SUCCESS != pjsip_udp_transport_start(m_servEndptPtr, &regAddr, nullptr, 1, &m_servUdpRegTpPtr)) {
            LOG(ERROR) << "SipServer init failed. pjsip_udp_transport_start() for sip regist error.";
            return MyStatus_t::FAILED;
        }
    
        if(PJ_SUCCESS != pjsip_tcp_transport_start(m_servEndptPtr, &regAddr, 1, &m_servTcpRegTpFactoryPtr)) {
            LOG(ERROR) << "SipServer init failed. pjsip_tcp_transport_start() for sip regist error.";
            return MyStatus_t::FAILED;
        }
    }

    // 添加媒体请求事件监听(事件监听不影响服务正常运行)
    if (MyStatus_t::SUCCESS != MyMediaReqEvListener::AddListener(&MySipServer::ServMediaReqEvFunc)) {
        LOG(WARNING) << "SipServer init warning. listen media request event failed.";
    }

    // 事件线程池初始化
    std::string thdPoolName;
    MyServerHelper::PrintSipServThdPoolName(m_servAddrCfg, thdPoolName);
    m_servThdPoolPtr = pjsip_endpt_create_pool(m_servEndptPtr, thdPoolName.c_str(), 
                                               evThdMemCfg.sipEvThdInitSize, evThdMemCfg.sipEvThdIncreSize);
    if (nullptr == m_servThdPoolPtr) {
        LOG(ERROR) << "SipServer init failed. pjsip_endpt_create_pool error.";
        return MyStatus_t::FAILED;
    }

    m_status.store(MyStatus_t::SUCCESS);

    std::string sipServInfo;
    MyServerHelper::PrintSipServInfo(m_servAddrCfg, sipServInfo);
    LOG(INFO) << "SipServer init success. " << sipServInfo << ".";

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::run()
{
    std::string sipServInfo;
    MyServerHelper::PrintSipServInfo(m_servAddrCfg, sipServInfo);

    if ( MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "SipServer is not init. " << sipServInfo << ".";
        return MyStatus_t::FAILED;
    }

    // delete in: MySipServer::ServThdFunc()
    auto servAddrCfgPtr = new MySipServAddrCfg_dt(m_servAddrCfg);

    // 服务需要事件循环触发消息发送和接收回调
    if (PJ_SUCCESS != pj_thread_create(m_servThdPoolPtr, nullptr, &MySipServer::ServThdFunc,
        servAddrCfgPtr, 0, 0, &m_servEvThdPtr)) {
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
    MyServerHelper::PrintSipServInfo(m_servAddrCfg, sipServInfo);
    LOG(INFO) << "SipServer shutdown success. " << sipServInfo << ".";
    
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::onServRecvSipRegReqMsg(MySipRxDataPtr regReqMsgPtr)
{
    MySipRegApp::SmtWkPtr sipRegAppWkPtr;
    if (MyStatus_t::SUCCESS != MyAppManage::GetSipRegApp(sipRegAppWkPtr)) {
        LOG(ERROR) << "SipServer recv sip register request failed. GetSipRegApp() error.";
        return MyStatus_t::FAILED;
    }
    return sipRegAppWkPtr.lock()->onRecvSipRegReqMsg(regReqMsgPtr);
}

MyStatus_t MySipServer::onServRecvSipInviteReqMsg(MySipRxDataPtr invReqMsgPtr)
{
    MySipInviteApp::SmtWkPtr sipInvAppWkPtr;
    if (MyStatus_t::SUCCESS != MyAppManage::GetSipInviteApp(sipInvAppWkPtr)) {
        LOG(ERROR) << "SipServer recv sip invite request error. get sip invite app failed.";
        return MyStatus_t::FAILED;
    }

    return sipInvAppWkPtr.lock()->onRecvSipInviteReqMsg(invReqMsgPtr);
}

MyStatus_t MySipServer::onServRecvSipKeepAliveReqMsg(MySipRxDataPtr keepAliveReqMsgPtr)
{
    MySipRegApp::SmtWkPtr sipRegAppWkPtr;
    if (MyStatus_t::SUCCESS != MyAppManage::GetSipRegApp(sipRegAppWkPtr)) {
        LOG(ERROR) << "SipServer recv sip keepAlive request failed. sipRegApp invalid.";
        return MyStatus_t::FAILED;
    }
    return sipRegAppWkPtr.lock()->onRecvSipKeepAliveReqMsg(keepAliveReqMsgPtr);
}

MyStatus_t MySipServer::onServRecvSipCatalogQueryReqMsg(MySipRxDataPtr catalogQueryReqMsgPtr)
{
    MySipCatalogApp::SmtWkPtr sipCatalogAppWkPtr;
    if (MyStatus_t::SUCCESS != MyAppManage::GetSipCatalogApp(sipCatalogAppWkPtr)) {
        LOG(ERROR) << "SipServer recv sip catalog query request failed. sipCatalogApp invalid.";
        return MyStatus_t::FAILED;
    }
    return sipCatalogAppWkPtr.lock()->onRecvSipCatalogQueryReqMsg(catalogQueryReqMsgPtr);
}
MyStatus_t MySipServer::onServRecvSipCatalogRespMsg(MySipRxDataPtr catalogResponseReqMsgPtr)
{
    MySipCatalogApp::SmtWkPtr sipCatalogAppWkPtr;
    if (MyStatus_t::SUCCESS != MyAppManage::GetSipCatalogApp(sipCatalogAppWkPtr)) {
        LOG(ERROR) << "SipServer recv sip catalog response request failed. sipCatalogApp invalid.";
        return MyStatus_t::FAILED;
    }
    return sipCatalogAppWkPtr.lock()->onRecvSipCatalogResponseReqMsg(catalogResponseReqMsgPtr);
}

MyStatus_t MySipServer::onReqLowServCatalog(const MySipRegLowServCfg_dt& lowSipRegServAddrCfg)
{
    MySipCatalogApp::SmtWkPtr sipCatalogAppWkPtr;
    if (MyStatus_t::SUCCESS != MyAppManage::GetSipCatalogApp(sipCatalogAppWkPtr)) {
        LOG(ERROR) << "SipServer onReqLowServCatalog() failed. sipCatalogApp invalid.";
        return MyStatus_t::FAILED;
    }
    return sipCatalogAppWkPtr.lock()->onReqLowServCatalog(lowSipRegServAddrCfg, m_servAddrCfg);
}

MyStatus_t MySipServer::onReqDeviceMedia(const std::string& deviceId, const MyHttpReqMediaInfo_dt& reqInfo, std::string& respInfo)
{
    MySipInviteApp::SmtWkPtr sipInviteAppWkPtr;
    if (MyStatus_t::SUCCESS != MyAppManage::GetSipInviteApp(sipInviteAppWkPtr)) {
        LOG(ERROR) << "SipServer onReqDeviceMedia() failed. sipInviteApp invalid.";
        return MyStatus_t::FAILED;
    }

    if (MyMedaPlayWay_t::PLAY == reqInfo.playType) {
        return sipInviteAppWkPtr.lock()->onReqDevicePlayMedia(deviceId, reqInfo, respInfo);
    }
    else {
        LOG(ERROR) << "SipServer onReqDeviceMedia() failed. current not support play type.";
        return MyStatus_t::FAILED;
    }
}

MyStatus_t MySipServer::getState(MyStatus_t& status) const
{
    status = m_status.load();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::getServAddrCfg(MySipServAddrCfg_dt& cfg)
{
    if (MyStatus_t::FAILED == m_status.load()) {
        LOG(ERROR) << "SipServer is not init.";
        return MyStatus_t::FAILED;
    }

    boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

    cfg = m_servAddrCfg;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::getServPool(MySipPoolPtrAddr poolAddr)
{
    if (MyStatus_t::FAILED == m_status.load()) {
        LOG(ERROR) << "SipServer is not init.";
        return MyStatus_t::FAILED;
    }

    boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

    *poolAddr = m_servThdPoolPtr;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::getServUdpTp(MySipTransportPtrAddr udpTpPtrAddr)
{
    if (MyStatus_t::FAILED == m_status.load()) {
        LOG(ERROR) << "SipServer is not init.";
        return MyStatus_t::FAILED;
    }

    boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

    *udpTpPtrAddr = m_servUdpTpPtr;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::getServRegUdpTp(MySipTransportPtrAddr udpTpPtrAddr)
{
    if (MyStatus_t::FAILED == m_status.load()) {
        LOG(ERROR) << "SipServer is not init.";
        return MyStatus_t::FAILED;
    }

    boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

    *udpTpPtrAddr = m_servUdpRegTpPtr;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::getServTcpTp(MySipTransportPtrAddr tcpTpPtrAddr, const std::string& remoteIpAddr,
    uint16_t remotePort)
{
    if (MyStatus_t::FAILED == m_status.load()) {
        LOG(ERROR) << "SipServer is not init.";
        return MyStatus_t::FAILED;
    }

    boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

    std::string key = remoteIpAddr + ":" + std::to_string(remotePort);
    auto iter = m_servTcpTpMap.find(key);
    if (m_servTcpTpMap.end() == iter) {
        pj_sockaddr remoteAddr;
        pj_bzero(&remoteAddr, sizeof(remoteAddr));
    
        pj_str_t remoteAddrstr = pj_str(const_cast<char *>(remoteIpAddr.c_str()));
        pj_inet_pton(PJ_AF_INET, &remoteAddrstr, &remoteAddr.ipv4.sin_addr);
    
        remoteAddr.ipv4.sin_family = pj_AF_INET();
        remoteAddr.ipv4.sin_port   = pj_htons(remotePort);

        MySipTpmgrPtr tmpgrPtr = pjsip_endpt_get_tpmgr(m_servEndptPtr);

        MySipTransportPtr tpPtr = nullptr;
        m_servTcpTpFactoryPtr->create_transport(m_servTcpTpFactoryPtr, tmpgrPtr, m_servEndptPtr, &remoteAddr,
            sizeof(remoteAddr), &tpPtr);

        *tcpTpPtrAddr = tpPtr;
        m_servTcpTpMap.insert(std::make_pair(key, tpPtr));
    }
    else {
        *tcpTpPtrAddr = iter->second;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServer::getServRegTcpTp(MySipTransportPtrAddr tcpTpPtrAddr, const std::string& remoteIpAddr,
    uint16_t remotePort)
{
    if (MyStatus_t::FAILED == m_status.load()) {
        LOG(ERROR) << "SipServer is not init.";
        return MyStatus_t::FAILED;
    }

    boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);
    
    std::string key = remoteIpAddr + ":" + std::to_string(remotePort);
    auto iter = m_servTcpRegTpMap.find(key);
    if (m_servTcpRegTpMap.end() == iter) {
        pj_sockaddr remoteAddr;
        pj_bzero(&remoteAddr, sizeof(remoteAddr));
    
        pj_str_t remoteAddrstr = pj_str(const_cast<char *>(remoteIpAddr.c_str()));
        pj_inet_pton(PJ_AF_INET, &remoteAddrstr, &remoteAddr.ipv4.sin_addr);
    
        remoteAddr.ipv4.sin_family = pj_AF_INET();
        remoteAddr.ipv4.sin_port   = pj_htons(remotePort);

        MySipTpmgrPtr tmpgrPtr = pjsip_endpt_get_tpmgr(m_servEndptPtr);

        MySipTransportPtr tpPtr = nullptr;
        m_servTcpTpFactoryPtr->create_transport(m_servTcpRegTpFactoryPtr, tmpgrPtr, m_servEndptPtr, &remoteAddr,
            sizeof(remoteAddr), &tpPtr);

        *tcpTpPtrAddr = tpPtr;
        m_servTcpRegTpMap.insert(std::make_pair(key, tpPtr));
    }
    else {
        *tcpTpPtrAddr = iter->second;
    }
    return MyStatus_t::SUCCESS;
}

}; //namespace MY_SERVER