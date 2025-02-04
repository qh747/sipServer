#include <thread>
#include <chrono>
#include <cstdlib>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySystemConfg.h"
#include "envir/mySystemServManage.h"
#include "utils/mySipServerHelper.h"
#include "utils/mySipAppHelper.h"
#include "utils/mySipMsgHelper.h"
#include "utils/myTimeHelper.h"
#include "server/mySipServer.h"
#include "app/mySipRegApp.h"
using toolkit::Timer;
using toolkit::EventPollerPool;
using MY_COMMON::MyStatus_t;
using MY_COMMON::MySipMsgUri_dt;
using MY_COMMON::MySipAppIdCfg_dt;
using MY_COMMON::MySipMsgContactHdr_dt;
using MY_COMMON::MySipLowRegServInfo_dt;
using MY_UTILS::MyJsonHelper;
using MY_UTILS::MyTimeHelper;
using MY_UTILS::MySipAppHelper;
using MY_UTILS::MySipMsgHelper;
using MY_UTILS::MySipServerHelper;
using MY_ENVIR::MySystemConfig;
using MY_ENVIR::MySystemServManage;
using MY_SERVER::MySipServer;

namespace MY_APP {

void MySipRegApp::OnRegRespCb(SipAppRegCbParamPtr paramPtr)
{
    if (nullptr == paramPtr) {
        LOG(ERROR) << "Sip app reg module register response callback failed. invalid param.";
        return;
    }
    
}
    
MySipRegApp::MySipRegApp() : m_appIdPtr(nullptr), m_status(MyStatus_t::FAILED), m_servSmtWkPtr(), m_timePtr(nullptr)
{

}

MySipRegApp::~MySipRegApp()
{
    if (MyStatus_t::SUCCESS == m_status.load()) {
        this->shutdown();
    }
}

MyStatus_t MySipRegApp::init(SipServSmtWkPtr servPtr, const std::string& id, const std::string& name, pjsip_module_priority priority)
{
    // 状态检查
    if (MyStatus_t::SUCCESS == m_status.load()) {
        LOG(WARNING) << "Sip app module init failed. MySipRegApp is init already.";
        return MyStatus_t::SUCCESS;
    }

    // 输入参数检查
    SipServSmtPtr strongServPtr = servPtr.lock();
    if (nullptr == strongServPtr || name.empty()) {
        LOG(ERROR) << "Sip app module init failed. invalid input param.";
        return MyStatus_t::FAILED;
    }

    SipAppEndptPtr endptPtr = strongServPtr->getSipServEndptPtr();
    if (nullptr == endptPtr) {
        LOG(ERROR) << "Sip app module init failed. invalid endpoint.";
        return MyStatus_t::FAILED;
    }

    m_servSmtWkPtr = servPtr;

    // app标识初始化
    m_appIdPtr           = std::make_shared<MySipAppIdCfg_dt>();   
    m_appIdPtr->id       = id;
    m_appIdPtr->name     = name;   
    m_appIdPtr->priority = priority;

    LOG(INFO) << "Sip app module init success. " << MySipAppHelper::GetSipAppInfo(*m_appIdPtr) << ".";

    m_status.store(MyStatus_t::SUCCESS);
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegApp::run()
{
    // 状态检查
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "Sip app module run failed. MySipRegApp has not been initialized.";
        return MyStatus_t::FAILED;
    }

    // 定时器初启动
    if (nullptr == m_timePtr) {
        SipRegAppSmtWkPtr weakSelf = this->shared_from_this();
        m_timePtr = std::make_shared<Timer>(3.0f, [weakSelf]() -> bool {
            auto strongSelf = weakSelf.lock();
            if (nullptr == strongSelf) {
                return false;
            }
            return strongSelf->onTimer();
        }, EventPollerPool::Instance().getFirstPoller());
    }

    // 获取本地sip服务地址配置
    SipServSmtPtr strongServPtr = m_servSmtWkPtr.lock();
    if (nullptr == strongServPtr) {
        LOG(ERROR) << "Sip app module run failed. invalid sip server.";
        return MyStatus_t::FAILED;
    }
    SipServAddrCfg servAddrCfg = strongServPtr->getSipServAddrCfg();

    // 获取本地sip服务的endpoint
    SipAppEndptPtr endptPtr = strongServPtr->getSipServEndptPtr();
    if (nullptr == endptPtr) {
        LOG(ERROR) << "Sip app module run failed. invalid endpoint.";
        return MyStatus_t::FAILED;
    }

    // 获取上级sip服务配置
    MyJsonHelper::SipRegUpServJsonMap regUpServMap = MySystemConfig::GetSipUpRegServCfgMap();

    // 向上级服务发起注册
    for (const auto& iter : regUpServMap) {
        this->regUpServ(iter.second, servAddrCfg, endptPtr);
    }

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegApp::shutdown()
{
    // 状态检查
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "Sip app module shutdown failed. MySipRegApp is not init.";
        return MyStatus_t::SUCCESS;
    }

    // 定时器销毁
    if (nullptr != m_timePtr) {
        m_timePtr.reset();
    }

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegApp::regUpServ(const SipRegUpServCfg& cfg, const SipServAddrCfg& servAddr, SipAppEndptPtr endptPtr)
{
    // 减小临界区
    {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        if (m_regUpServMap.end() != m_regUpServMap.find(cfg.upSipServAddrCfg.id)) {
            LOG(WARNING) << "Sip app register up server failed. MySipRegApp has register up server. " 
                         << MySipServerHelper::GetSipUpRegServInfo(cfg) << ".";
            return MyStatus_t::FAILED;
        }
    }

    // sip register消息首部生成
    std::string sURL     = MySipMsgHelper::GenerateSipMsgURL(cfg.upSipServAddrCfg.id, cfg.upSipServAddrCfg.ipAddr, cfg.upSipServAddrCfg.port, cfg.proto);
    std::string sFromHdr = MySipMsgHelper::GenerateSipMsgFromHeader(servAddr.id, servAddr.ipAddr);
    std::string sToHdr   = MySipMsgHelper::GenerateSipMsgToHeader(servAddr.id, servAddr.ipAddr);
    std::string sContact = MySipMsgHelper::GenerateSipMsgContactHeader(servAddr.id, servAddr.ipAddr, servAddr.port);

    pj_str_t sipMsgURL     = pj_str(const_cast<char*>(sURL.c_str()));
    pj_str_t sipMsgFromHdr = pj_str(const_cast<char*>(sFromHdr.c_str()));
    pj_str_t sipMsgToHdr   = pj_str(const_cast<char*>(sToHdr.c_str()));
    pj_str_t sipMsgContact = pj_str(const_cast<char*>(sContact.c_str()));

    SipAppRegcPtr   regcPtr       = nullptr;
    SipRegUpServCfg upServAddrCfg = cfg;
    
    // sip register上下文创建
    if(PJ_SUCCESS != pjsip_regc_create(endptPtr, &upServAddrCfg, &MySipRegApp::OnRegRespCb, &regcPtr)) {
        LOG(ERROR) << "Sip app register up server failed. pjsip_regc_create() error. " << MySipServerHelper::GetSipUpRegServInfo(cfg) << ".";
        return MyStatus_t::FAILED;
    }

    // sip register上下文初始化
    if(PJ_SUCCESS != pjsip_regc_init(regcPtr, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, 1, &sipMsgContact, upServAddrCfg.expired)) {
        pjsip_regc_destroy(regcPtr);
        LOG(ERROR) << "Sip app register up server failed. pjsip_regc_init() error. " << MySipServerHelper::GetSipUpRegServInfo(cfg) << ".";
        return MyStatus_t::FAILED;
    }

    // sip register授权
    if (upServAddrCfg.upSipServAuthCfg.enableAuth) {
        pjsip_cred_info cred;
        pj_bzero(&cred, sizeof(pjsip_cred_info));

        cred.scheme    = pj_str(const_cast<char*>("digest"));
        cred.realm     = pj_str(const_cast<char*>(upServAddrCfg.upSipServAuthCfg.authRealm.c_str()));
        cred.username  = pj_str(const_cast<char*>(upServAddrCfg.upSipServAuthCfg.authName.c_str()));
        cred.data      = pj_str(const_cast<char*>(upServAddrCfg.upSipServAuthCfg.authPwd.c_str()));
        cred.data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;

        if(PJ_SUCCESS != pjsip_regc_set_credentials(regcPtr, 1, &cred)) {
            pjsip_regc_destroy(regcPtr);
            LOG(ERROR) << "Sip app register up server failed. pjsip_regc_set_credentials() error. " 
                       << MySipServerHelper::GetSipUpRegServInfo(cfg) << ".";
            return MyStatus_t::FAILED;
        }
    }

    // sip register消息创建
    SipAppTxDataPtr txDataPtr = nullptr;
    if(PJ_SUCCESS != pjsip_regc_register(regcPtr, PJ_TRUE, &txDataPtr)) {
        pjsip_regc_destroy(regcPtr);

        LOG(ERROR) << "Sip app register up server failed. pjsip_regc_destroy() error. " 
                   << MySipServerHelper::GetSipUpRegServInfo(cfg) << ".";
        return MyStatus_t::FAILED;
    }

    // sip register消息发送
    if(PJ_SUCCESS != pjsip_regc_send(regcPtr, txDataPtr)) {
        pjsip_regc_destroy(regcPtr);

        LOG(ERROR) << "Sip app register up server failed. pjsip_regc_send() error. " 
                   << MySipServerHelper::GetSipUpRegServInfo(cfg) << ".";
        return MyStatus_t::FAILED;
    }

    // 保存sip reg上下文
    SipUpRegServInfoPtr upRegServInfoPtr        = std::make_shared<SipUpRegServInfo>();
    upRegServInfoPtr->sipRegUpServCfg           = upServAddrCfg;
    upRegServInfoPtr->sipRegUpServPtr           = regcPtr;
    upRegServInfoPtr->sipRegUpServLastRegTime   = MyTimeHelper::GetCurrentFormatTime();

    // 减小临界区
    {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);
        m_regUpServMap.insert(std::make_pair(cfg.upSipServAddrCfg.id, upRegServInfoPtr));
    }
    return MyStatus_t::SUCCESS;
}

bool MySipRegApp::onTimer()
{
    if (MyStatus_t::SUCCESS != m_status.load()) {
        return false;
    }

    boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

    SipServSmtPtr strongServPtr = m_servSmtWkPtr.lock();
    if (nullptr == strongServPtr) {
        LOG(ERROR) << "Sip app module trigger timer failed. invalid sip server.";
        return false;
    }

    SipServAddrCfg servAddrCfg = strongServPtr->getSipServAddrCfg();
    SipAppEndptPtr endptPtr    = strongServPtr->getSipServEndptPtr();

    auto regUpServIter = m_regUpServMap.begin();
    while (m_regUpServMap.end() != regUpServIter) {
        int timeDiff = 0;
        if (MyStatus_t::SUCCESS != MyTimeHelper::CompareWithCurrentTime(regUpServIter->second->sipRegUpServLastRegTime, 
                                                                        (unsigned int)(regUpServIter->second->sipRegUpServCfg.expired), 
                                                                        timeDiff)) {
            LOG(ERROR) << "Sip app module erase invalid sip up server. invalid sip up server regist time format."
                       << " sip up server id: " << regUpServIter->second->sipRegUpServCfg.upSipServAddrCfg.id 
                       << " ip addr: " << regUpServIter->second->sipRegUpServCfg.upSipServAddrCfg.ipAddr
                       << " port: " << regUpServIter->second->sipRegUpServCfg.upSipServAddrCfg.port;
            regUpServIter = m_regUpServMap.erase(regUpServIter);
            continue;
        }

        // 上级sip服务注册即将超时，重新注册
        if (timeDiff < 5) {
            lock.unlock();
            this->regUpServ(regUpServIter->second->sipRegUpServCfg, servAddrCfg, endptPtr);
            lock.lock();
        }
        ++regUpServIter;
    }

    auto regLowServIter = m_regLowServMap.begin();
    while (m_regLowServMap.end() != regLowServIter) {
        // 移除超时的下级sip注册服务
        int timeDiff = 0;
        if (MyStatus_t::SUCCESS != MyTimeHelper::CompareWithCurrentTime(regLowServIter->second->sipRegLowServLastRegTime, 
                                                                        (unsigned int)(regLowServIter->second->sipRegLowServCfg.expired), 
                                                                        timeDiff)) {
            LOG(ERROR) << "Sip app module erase invalid sip low server. invalid sip regist time format."
                       << " sip low server id: " << regLowServIter->second->sipRegLowServCfg.lowSipServAddrCfg.id
                       << " ip addr: " << regLowServIter->second->sipRegLowServCfg.lowSipServAddrCfg.ipAddr
                       << " port: " << regLowServIter->second->sipRegLowServCfg.lowSipServAddrCfg.port;

            regLowServIter = m_regLowServMap.erase(regLowServIter);
            continue;
        }

        if (timeDiff <= 0) {
            LOG(ERROR) << "Sip app module erase invalid sip low server. invalid sip regist time format."
                       << " sip low server id: " << regLowServIter->second->sipRegLowServCfg.lowSipServAddrCfg.id
                       << " ip addr: " << regLowServIter->second->sipRegLowServCfg.lowSipServAddrCfg.ipAddr
                       << " port: " << regLowServIter->second->sipRegLowServCfg.lowSipServAddrCfg.port;

            regLowServIter = m_regLowServMap.erase(regLowServIter);
            continue;
        }
        ++regLowServIter;
    }
    return true;
}

MyStatus_t MySipRegApp::onRecvSipRegReqMsg(SipAppRxDataPtr rxDataPtr)
{
    // 解析下级sip服务id
    SipAppMsgHdrPtr contactHeader = (SipAppMsgHdrPtr)pjsip_msg_find_hdr(rxDataPtr->msg_info.msg, PJSIP_H_CONTACT, nullptr);
    if (nullptr == contactHeader) {
        LOG(ERROR) << "Sip app receive register request message failed. Can't find contact header. ";
        return MyStatus_t::FAILED;
    }

    char buf[256];
    pjsip_hdr_print_on(contactHeader, buf, sizeof(buf));

    MySipMsgContactHdr_dt sipContactHeader;
    if (MyStatus_t::SUCCESS != MySipMsgHelper::ParseSipMsgContactHdr(buf, sipContactHeader)) {
        LOG(ERROR) << "Sip app receive register request message failed. Can't parse contact header. ";
        return MyStatus_t::FAILED;
    }

    // 解析下级sip服务注册有效时间
    SipAppMsgHdrPtr expireHeader = (SipAppMsgHdrPtr)pjsip_msg_find_hdr(rxDataPtr->msg_info.msg, PJSIP_H_EXPIRES, nullptr);
    if (nullptr == expireHeader) {
        LOG(ERROR) << "Sip app receive register request message failed. Can't find expire header. "
                   << "id: " << sipContactHeader.id << " ip addr: " << sipContactHeader.ipAddr << " port: " << sipContactHeader.port << ".";
        return MyStatus_t::FAILED;
    }

    memset(buf, 0, sizeof(buf));
    pjsip_hdr_print_on(expireHeader, buf, sizeof(buf));
    
    double expire = 0.0;
    if (MyStatus_t::SUCCESS != MySipMsgHelper::ParseSipMsgExpireHdr(buf, expire)) {
        LOG(ERROR) << "Sip app receive register request message failed. Can't parse expire header. "
                   << "id: " << sipContactHeader.id << " ip addr: " << sipContactHeader.ipAddr << " port: " << sipContactHeader.port << ".";
        return MyStatus_t::FAILED;
    }

    // 获取下级sip服务配置
    MyJsonHelper::SipRegLowServJsonMap regLowServMap = MySystemConfig::GetSipLowRegServCfgMap();
    auto cfgIter = regLowServMap.find(sipContactHeader.id);
    if (regLowServMap.end() == cfgIter) {
        // 下级sip服务配置不存在
        LOG(ERROR) << "Sip app receive register request message failed. Can't find low register server config. "
                   << "id: " << sipContactHeader.id << " ip addr: " << sipContactHeader.ipAddr << " port: " << sipContactHeader.port << ".";
        return MyStatus_t::FAILED;
    }

    // 获取本地sip服务的endpoint
    auto sipServPtr = m_servSmtWkPtr.lock();
    if (nullptr == sipServPtr) {
        LOG(ERROR) << "Sip app receive register request message failed. can't get local sip server. "
                   << "id: " << sipContactHeader.id << " ip addr: " << sipContactHeader.ipAddr << " port: " << sipContactHeader.port << ".";
        return MyStatus_t::FAILED;
    }

    auto sipServEndptPtr = sipServPtr->getSipServEndptPtr();

    // 创建向下级sip服务响应的sip 200 ok消息
    SipAppTxDataPtr txDataPtr = nullptr;
    if (PJ_SUCCESS != pjsip_endpt_create_response(sipServEndptPtr, rxDataPtr, 200, nullptr, &txDataPtr)) {
        LOG(ERROR) << "Sip app receive register request message failed. can't create response message. "
                   << "id: " << sipContactHeader.id << " ip addr: " << sipContactHeader.ipAddr << " port: " << sipContactHeader.port << ".";
        return MyStatus_t::FAILED;
    }

    // 向sip 200 ok消息添加Date首部
    std::string curTime    = MyTimeHelper::GetCurrentSipHeaderTime();
    pj_str_t    valueTime  = pj_str(const_cast<char*>(curTime.c_str()));
    pj_str_t    keyTime    = pj_str("Date");

    SipAppMsgDateHdrPtr dateHdrPtr = (SipAppMsgDateHdrPtr)pjsip_date_hdr_create(rxDataPtr->tp_info.pool, &keyTime, &valueTime);
    pjsip_msg_add_hdr(txDataPtr->msg, (SipAppMsgHdrPtr)dateHdrPtr);

    // 获取发送sip 200 ok消息的地址
    pjsip_response_addr respAddr;
    if(PJ_SUCCESS != pjsip_get_response_addr(txDataPtr->pool, rxDataPtr, &respAddr)) {
        pjsip_tx_data_dec_ref(txDataPtr);
        LOG(ERROR) << "Sip app receive register request message failed. get response addr failed. "
                   << "id: " << sipContactHeader.id << " ip addr: " << sipContactHeader.ipAddr << " port: " << sipContactHeader.port << ".";
        return MyStatus_t::FAILED;
    }

    // 发送sip 200 ok消息
    if(PJ_SUCCESS != pjsip_endpt_send_response(sipServEndptPtr, &respAddr, txDataPtr, nullptr, nullptr)) {
        pjsip_tx_data_dec_ref(txDataPtr);
        LOG(ERROR) << "Sip app receive register request message failed. send response msg failed. "
                   << "id: " << sipContactHeader.id << " ip addr: " << sipContactHeader.ipAddr << " port: " << sipContactHeader.port << ".";
        return MyStatus_t::FAILED;
    }

    boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

    auto infoIter = m_regLowServMap.find(sipContactHeader.id);
    if (expire > 0) {
        // 下级sip服务注册
        if (m_regLowServMap.end() == infoIter) {
            // 添加下级sip服务配置
            SipLowRegServInfoPtr lowRegServInfoSmtPtr                       = std::make_shared<SipLowRegServInfo>();
            lowRegServInfoSmtPtr->sipRegLowServCfg                          = cfgIter->second;
            lowRegServInfoSmtPtr->sipRegLowServCfg.lowSipServAddrCfg.ipAddr = sipContactHeader.ipAddr;
            lowRegServInfoSmtPtr->sipRegLowServCfg.lowSipServAddrCfg.port   = sipContactHeader.port;
            lowRegServInfoSmtPtr->sipRegLowServCfg.expired                  = expire;
            lowRegServInfoSmtPtr->sipRegLowServLastRegTime                  = MyTimeHelper::GetCurrentFormatTime();
            m_regLowServMap.insert(std::make_pair(sipContactHeader.id, lowRegServInfoSmtPtr));
        }
        else {
            // 更新下级sip服务配置
            infoIter->second->sipRegLowServCfg.lowSipServAddrCfg.ipAddr     = sipContactHeader.ipAddr;
            infoIter->second->sipRegLowServCfg.lowSipServAddrCfg.port       = sipContactHeader.port;
            infoIter->second->sipRegLowServCfg.expired                      = expire;
            infoIter->second->sipRegLowServLastRegTime                      = MyTimeHelper::GetCurrentFormatTime();
        }
    }
    else {
        // 下级sip服务注销
        if (m_regLowServMap.end() == infoIter) {
            // 下级sip服务未注册
            LOG(ERROR) << "Sip app receive register request message failed. low sip serv not reg. "
                       << "id: " << sipContactHeader.id << " ip addr: " << sipContactHeader.ipAddr << " port: " << sipContactHeader.port << ".";
            return MyStatus_t::FAILED;
        }

        // 移除下级sip服务配置
        m_regLowServMap.erase(infoIter);
    }

    return MyStatus_t::SUCCESS;
}

}; // namespace MY_APP