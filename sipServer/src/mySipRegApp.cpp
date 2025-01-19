#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySystemConfg.h"
#include "utils/mySipServerHelper.h"
#include "utils/mySipAppHelper.h"
#include "utils/mySipMsgHelper.h"
#include "server/mySipServer.h"
#include "app/mySipRegApp.h"
using MY_COMMON::MyStatus_t;
using MY_COMMON::MySipAppIdCfg_dt;
using MY_UTILS::MySipAppHelper;
using MY_UTILS::MySipServerHelper;
using MY_UTILS::MySipMsgHelper;
using MY_UTILS::MyJsonHelper;
using MY_ENVIR::MySystemConfig;
using MY_SERVER::MySipServer;

namespace MY_APP {

void MySipRegApp::OnRegRespCb(SipAppRegCbParamPtr paramPtr)
{
    if (nullptr == paramPtr) {
        LOG(ERROR) << "Sip app reg module register response callback failed. invalid param.";
        return;
    }

}
    
MySipRegApp::MySipRegApp() : m_appIdPtr(nullptr), m_status(MyStatus_t::FAILED), m_servSmtWkPtr()
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

    // 获取本地sip服务地址配置
    SipServAddrCfg servAddrCfg = MySystemConfig::GetSipServAddrCfg();

    // 获取本地sip服务的endpoint
    if (m_servSmtWkPtr.expired()) {
        LOG(ERROR) << "Sip app module run failed. invalid sip server.";
        return MyStatus_t::FAILED;
    }

    SipServSmtPtr strongServPtr = m_servSmtWkPtr.lock();
    SipAppEndptPtr endptPtr = strongServPtr->getSipServEndptPtr();
    if (nullptr == endptPtr) {
        LOG(ERROR) << "Sip app module run failed. invalid endpoint.";
        return MyStatus_t::FAILED;
    }

    // 获取上级sip服务配置
    MyJsonHelper::SipRegUpServJsonMap regUpServMap = MySystemConfig::GetSipUpRegServCfgMap();

    // 向上级服务发起注册
    for (const auto& iter : regUpServMap) {
        this->startRegUpServ(iter.second, servAddrCfg, endptPtr);
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

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegApp::startRegUpServ(const SipRegUpServCfg& cfg, const SipServAddrCfg& servAddr, SipAppEndptPtr endptPtr)
{
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
    SipUpRegServInfoPtr upRegServInfoPtr = std::make_shared<SipUpRegServInfo>();
    upRegServInfoPtr->sipRegUpServCfg    = upServAddrCfg;
    upRegServInfoPtr->sipRegUpServPtr    = regcPtr;

    {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);
        m_regUpServMap.insert(std::make_pair(cfg.upSipServAddrCfg.id, upRegServInfoPtr));
    }
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_APP