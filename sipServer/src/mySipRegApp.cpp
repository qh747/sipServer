#include <cstdlib>
#include <cstring>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySystemConfg.h"
#include "envir/mySystemPjsip.h"
#include "manager/myServManage.h"
#include "manager/mySipRegManage.h"
#include "manager/mySipCatalogManage.h"
#include "utils/myXmlHelper.h"
#include "utils/myTimeHelper.h"
#include "utils/mySipAppHelper.h"
#include "utils/mySipMsgHelper.h"
#include "utils/myRandomHelper.h"
#include "utils/myServerHelper.h"
#include "server/mySipServer.h"
#include "app/mySipRegApp.h"
using namespace toolkit;
using namespace MY_UTILS;
using namespace MY_COMMON;
using MY_SERVER::MySipServer;
using MY_ENVIR::MySystemPjsip;
using MY_ENVIR::MySystemConfig;
using MY_MANAGER::MyServManage;
using MY_MANAGER::MySipRegManage;
using MY_MANAGER::MySipCatalogManage;

namespace MY_APP {

void MySipRegApp::OnRegRespCb(MySipRegCbParamPtr regParamPtr)
{
    if (nullptr == regParamPtr) {
        LOG(ERROR) << "Sip reg app reg module register response callback failed. invalid param.";
        return;
    }

    MySipRegUpServCfgPtr sipRegUpServCfgPtr = (MySipRegUpServCfgPtr)(regParamPtr->token);
    std::unique_ptr<MySipRegUpServCfg_dt> thdMemManagePtr(sipRegUpServCfgPtr);

    if (200 == regParamPtr->code) {
        // 注册成功，保存sip reg上下文
        MySipUpRegServInfo_dt upRegServInfo;
        upRegServInfo.sipRegUpServCfg = *sipRegUpServCfgPtr;
        upRegServInfo.sipRegUpServKeepAliveIdx  = 0;

        MyTimeHelper::GetCurrentFormatTime(upRegServInfo.sipRegUpServLastRegTime);
        MySystemConfig::GetSipServRegExpiredTimeInterval(upRegServInfo.sipRegUpServExpired);

        if (MyStatus_t::SUCCESS != MySipRegManage::AddSipRegUpServInfo(upRegServInfo)) {
            LOG(ERROR) << "Sip reg app reg module register response callback failed. save reg info failed.";
        }
        else {
            std::string upRegServInfoStr;
            MyServerHelper::PrintSipRegServInfo(sipRegUpServCfgPtr->upSipServRegAddrCfg, upRegServInfoStr);

            LOG(INFO) << "Sip reg app reg module register response callback success. reg info: " << upRegServInfoStr;
        }
    }   
    
    regParamPtr->token = nullptr;
}

pj_status_t MySipRegApp::OnRegFillAuthInfoCb(MySipPoolPtr poolPtr, MySipStrCstPtr realmPtr, 
                                             MySipStrCstPtr namePtr, MySipCredInfoPtr credInfoPtr)
{
    std::string nameStr  = std::string(namePtr->ptr).substr(0, namePtr->slen);
    std::string realmStr = std::string(realmPtr->ptr).substr(0, realmPtr->slen);

    MySipServAuthCfg_dt authCfg;
    if (MyStatus_t::SUCCESS != MySipRegManage::GetSipRegLowAuthCfg(nameStr, realmStr, authCfg)) {
        LOG(ERROR) << "Sip reg app reg module fill auth info callback failed. invalid auth info: " << nameStr << " " << realmStr;
        return PJ_FALSE;
    }
    
    if (authCfg.authName.empty() || authCfg.authRealm.empty() || authCfg.authPwd.empty()) {
        LOG(ERROR) << "Auth reg sip low server failed. invalid auth info: " << namePtr->ptr << " " << realmPtr->ptr;
        return PJ_FALSE;
    }

    pj_str_t scheme = pj_str(const_cast<char*>("digest"));
    pj_str_t passwd = pj_str(const_cast<char*>(authCfg.authPwd.c_str()));

    // 鉴权信息使用深拷贝方式，否则造成信息丢失
    pj_strdup(poolPtr, &credInfoPtr->scheme, &scheme);
    pj_strdup(poolPtr, &credInfoPtr->realm, realmPtr);
    pj_strdup(poolPtr, &credInfoPtr->username, namePtr);
    pj_strdup(poolPtr, &credInfoPtr->data, &passwd);
    credInfoPtr->data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    return PJ_SUCCESS;
}

void MySipRegApp::OnKeepAliveRespCb(MyFuncCbParamPtr evParamPtr, MySipEvPtr evPtr)
{
    MySipRegUpServCfgPtr sipRegUpServCfgPtr = static_cast<MySipRegUpServCfgPtr>(evParamPtr); 

    // 内存管理, new in function: MySipRegApp::keepAliveUpServ()
    std::unique_ptr<MySipRegUpServCfg_dt> memManagePtr(sipRegUpServCfgPtr);

    // 查找本级sip服务id
    std::string servId;
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServId(servId)) {
        LOG(ERROR) << "Sip reg app reg module keep alive response callback failed. invalid sip server. id.";
        return;
    }

    if(200 != evPtr->body.tsx_state.tsx->status_code) {
        // 移除无效上级服务
        MySipRegManage::DelSipRegUpServInfo(sipRegUpServCfgPtr->upSipServRegAddrCfg.id);
    }
    else {
        // 更新有效上级服务
        uint32_t keepAliveIdx;
        if (MyStatus_t::SUCCESS != MySipRegManage::GetSipRegUpServKeepAliveIdx(sipRegUpServCfgPtr->upSipServRegAddrCfg.id, keepAliveIdx)) {
            LOG(ERROR) << "Sip reg app reg module keep alive response callback failed. get keep alive index failed. id: " << sipRegUpServCfgPtr->upSipServRegAddrCfg.id;
            return;
        }
        
        MySipRegManage::UpdateSipRegUpServKeepAliveIdx(sipRegUpServCfgPtr->upSipServRegAddrCfg.id, ++keepAliveIdx);

        std::string lastRegTime;
        MyTimeHelper::GetCurrentFormatTime(lastRegTime);
        MySipRegManage::UpdateSipRegUpServLastRegTime(sipRegUpServCfgPtr->upSipServRegAddrCfg.id, lastRegTime);
    }
}

MySipRegApp::MySipRegApp() : m_status(MyStatus_t::FAILED), m_timePtr(nullptr)
{
}

MySipRegApp::~MySipRegApp()
{
    if (MyStatus_t::SUCCESS == m_status.load()) {
        this->shutdown();
    }
}

MyStatus_t MySipRegApp::init(const std::string& servId, const std::string& id, const std::string& name, pjsip_module_priority priority)
{
    // 状态检查
    if (MyStatus_t::SUCCESS == m_status.load()) {
        LOG(WARNING) << "Sip reg app module init failed. MySipRegApp is init already.";
        return MyStatus_t::SUCCESS;
    }

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip reg app module init failed. invalid endpoint.";
        return MyStatus_t::FAILED;
    }

    // 服务id赋值
    m_servId = servId;

    // app标识初始化 
    m_appIdCfg.id       = id;
    m_appIdCfg.name     = name;   
    m_appIdCfg.priority = priority;

    std::string appInfo;
    MySipAppHelper::GetSipAppInfo(m_appIdCfg, appInfo);
    LOG(INFO) << "Sip reg app module init success. " << appInfo << ".";

    m_status.store(MyStatus_t::SUCCESS);
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegApp::run()
{
    // 状态检查
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "Sip reg app module run failed. MySipRegApp has not been initialized.";
        return MyStatus_t::FAILED;
    }

    // 定时器初启动
    if (nullptr == m_timePtr) {
        MySipRegApp::SmtWkPtr weakSelf;
        if (MyStatus_t::SUCCESS != this->getSipRegApp(weakSelf)) {
            LOG(ERROR) << "Sip reg app module run failed. get sip reg app failed.";
            return MyStatus_t::FAILED;
        }

        float timeInterval = 0;
        if (MyStatus_t::SUCCESS != MySystemConfig::GetSipServRegistJugdeTimeInterval(timeInterval)) {
            LOG(ERROR) << "Sip reg app module run failed. get sip serv regist jugde time interval failed.";
            return MyStatus_t::FAILED;
        }
       
        m_timePtr = std::make_shared<Timer>(timeInterval, [weakSelf]() -> bool {
            if (weakSelf.expired()) {
                return false;
            }

            // pjsip要求自定义线程进行注册才能使用
            pj_thread_desc desc;
            if(!pj_thread_is_registered()) {
                MySipThdPtr thdPtr = nullptr;
                pj_thread_register(nullptr, desc, &thdPtr);
            }

            return weakSelf.lock()->onTimer();
        }, EventPollerPool::Instance().getFirstPoller());
    }

    // 获取本地sip服务地址配置
    MySipServAddrCfg_dt servAddrCfg;
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServAddrCfg(servAddrCfg)) {
        LOG(ERROR) << "Sip reg app module run failed. get sip serv addr cfg failed.";
        return MyStatus_t::FAILED;
    }
    
    // 获取上级sip服务配置
    MySipRegUpServCfgMap regUpServMap;
    if (MyStatus_t::SUCCESS != MySystemConfig::GetSipUpRegServCfgMap(regUpServMap)) {
        LOG(ERROR) << "Sip reg app module run failed. get sip up reg serv cfg map failed.";
        return MyStatus_t::FAILED;
    }

    // 向上级服务发起注册
    for (const auto& pair : regUpServMap) {
        this->regUpServ(pair.second, servAddrCfg);
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegApp::shutdown()
{
    // 状态检查
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "Sip reg app module shutdown failed. MySipRegApp is not init.";
        return MyStatus_t::SUCCESS;
    }

    // 定时器销毁
    if (nullptr != m_timePtr) {
        m_timePtr.reset();
    }

    std::string appInfo;
    MySipAppHelper::GetSipAppInfo(m_appIdCfg, appInfo);
    LOG(INFO) << "Sip app module shutdown success. " << appInfo << ".";

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegApp::regUpServ(const MySipRegUpServCfg_dt& regUpServCfg, const MySipServAddrCfg_dt& localServCfg)
{
    std::string upRegServInfoStr;
    MyServerHelper::PrintSipUpRegServInfo(regUpServCfg, upRegServInfoStr);

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip reg app register up server failed. invalid endpoint. " << upRegServInfoStr << ".";
        return MyStatus_t::FAILED;
    }

    // sip register消息首部生成
    std::string sURL;
    MySipMsgHelper::GenerateSipMsgURL(regUpServCfg.upSipServRegAddrCfg.id, regUpServCfg.upSipServRegAddrCfg.ipAddr, 
                                      regUpServCfg.upSipServRegAddrCfg.port, regUpServCfg.proto, sURL);

    std::string sFromHdr;
    MySipMsgHelper::GenerateSipMsgFromHeader(localServCfg.id, localServCfg.ipAddr, sFromHdr);

    std::string sToHdr;
    MySipMsgHelper::GenerateSipMsgToHeader(localServCfg.id, localServCfg.ipAddr, sToHdr);

    std::string sContact;
    MySipMsgHelper::GenerateSipMsgContactHeader(localServCfg.id, localServCfg.ipAddr, localServCfg.regPort, sContact);

    pj_str_t sipMsgURL     = pj_str(const_cast<char*>(sURL.c_str()));
    pj_str_t sipMsgFromHdr = pj_str(const_cast<char*>(sFromHdr.c_str()));
    pj_str_t sipMsgToHdr   = pj_str(const_cast<char*>(sToHdr.c_str()));
    pj_str_t sipMsgContact = pj_str(const_cast<char*>(sContact.c_str()));

    // free in func: MySipRegApp::OnRegRespCb()
    MySipRegUpServCfgPtr cpRegUpServCfgPtr = new MySipRegUpServCfg_dt(regUpServCfg);
    
    // sip register上下文创建
    MySipRegcPtr regcPtr = nullptr;
    if(PJ_SUCCESS != pjsip_regc_create(endptPtr, cpRegUpServCfgPtr, &MySipRegApp::OnRegRespCb, &regcPtr)) {
        LOG(ERROR) << "Sip reg app register up server failed. pjsip_regc_create() error. " << upRegServInfoStr << ".";
        return MyStatus_t::FAILED;
    }

    // sip register上下文初始化
    uint32_t expired = 0;
    if (MyStatus_t::SUCCESS != MySystemConfig::GetSipServRegExpiredTimeInterval(expired)) {
        pjsip_regc_destroy(regcPtr);
        LOG(ERROR) << "Sip reg app register up server failed. get sip serv reg expired time interval failed. " << upRegServInfoStr << ".";
        return MyStatus_t::FAILED;
    }

    // 指定传输端口为注册端口
    pjsip_tpselector tpSeclector;
    tpSeclector.type = PJSIP_TPSELECTOR_TRANSPORT;
    tpSeclector.disable_connection_reuse = false;

    MySipTransportPtr transportPtr = nullptr;
    if (MyTpProto_t::UDP == regUpServCfg.proto) {
        if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegUdpTp(&transportPtr)) {
            pjsip_regc_destroy(regcPtr);
            LOG(ERROR) << "Sip reg app register up server failed. get sip serv reg udp tp failed. " << upRegServInfoStr << ".";
            return MyStatus_t::FAILED;
        }
    }
    else {
        const auto& regUpServAddr = regUpServCfg.upSipServRegAddrCfg;
        if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegTcpTp(&transportPtr, regUpServAddr.ipAddr, regUpServAddr.port)) {
            pjsip_regc_destroy(regcPtr);
            LOG(ERROR) << "Sip reg app register up server failed. get sip serv reg tcp tp failed. " << upRegServInfoStr << ".";
            return MyStatus_t::FAILED;
        }
    }

    tpSeclector.u.transport = transportPtr;
    pjsip_regc_set_transport(regcPtr, &tpSeclector);

    if(PJ_SUCCESS != pjsip_regc_init(regcPtr, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, 1, &sipMsgContact, expired)) {
        pjsip_regc_destroy(regcPtr);
        LOG(ERROR) << "Sip reg app register up server failed. pjsip_regc_init() error. " << upRegServInfoStr << ".";
        return MyStatus_t::FAILED;
    }

    // sip register授权
    if (regUpServCfg.upSipServRegAuthCfg.enableAuth) {
        pjsip_cred_info cred;
        pj_bzero(&cred, sizeof(pjsip_cred_info));

        cred.scheme    = pj_str(const_cast<char*>("digest"));
        cred.realm     = pj_str(const_cast<char*>(regUpServCfg.upSipServRegAuthCfg.authRealm.c_str()));
        cred.username  = pj_str(const_cast<char*>(regUpServCfg.upSipServRegAuthCfg.authName.c_str()));
        cred.data      = pj_str(const_cast<char*>(regUpServCfg.upSipServRegAuthCfg.authPwd.c_str()));
        cred.data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;

        if(PJ_SUCCESS != pjsip_regc_set_credentials(regcPtr, 1, &cred)) {
            pjsip_regc_destroy(regcPtr);
            LOG(ERROR) << "Sip reg app register up server failed. pjsip_regc_set_credentials() error. " << upRegServInfoStr << ".";
            return MyStatus_t::FAILED;
        }
    }

    // sip register消息创建
    MySipTxDataPtr txDataPtr = nullptr;
    if(PJ_SUCCESS != pjsip_regc_register(regcPtr, PJ_TRUE, &txDataPtr)) {
        pjsip_regc_destroy(regcPtr);

        LOG(ERROR) << "Sip reg app register up server failed. pjsip_regc_destroy() error. " << upRegServInfoStr << ".";
        return MyStatus_t::FAILED;
    }

    // sip register消息发送
    if(PJ_SUCCESS != pjsip_regc_send(regcPtr, txDataPtr)) {
        pjsip_regc_destroy(regcPtr);

        LOG(ERROR) << "Sip reg app register up server failed. pjsip_regc_send() error. " << upRegServInfoStr << ".";
        return MyStatus_t::FAILED;
    }

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegApp::keepAliveUpServ(const MySipRegUpServCfg_dt& regUpServCfg, const MySipServAddrCfg_dt& localServCfg)
{
    std::string upRegServInfoStr;
    MyServerHelper::PrintSipUpRegServInfo(regUpServCfg, upRegServInfoStr);

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip reg app keepAlive up server failed. get pjsip endpoint failed. " << upRegServInfoStr << ".";
        return MyStatus_t::FAILED;
    }

    // sip keepAlive消息首部生成
    std::string sURL;
    MySipMsgHelper::GenerateSipMsgURL(regUpServCfg.upSipServRegAddrCfg.id, regUpServCfg.upSipServRegAddrCfg.ipAddr, 
                                    regUpServCfg.upSipServRegAddrCfg.port, regUpServCfg.proto, sURL);

    std::string sFromHdr;
    MySipMsgHelper::GenerateSipMsgFromHeader(localServCfg.id, localServCfg.ipAddr, sFromHdr);

    std::string sToHdr;
    MySipMsgHelper::GenerateSipMsgToHeader(localServCfg.id, localServCfg.ipAddr, sToHdr);

    pj_str_t sipMsgURL     = pj_str(const_cast<char*>(sURL.c_str()));
    pj_str_t sipMsgFromHdr = pj_str(const_cast<char*>(sFromHdr.c_str()));
    pj_str_t sipMsgToHdr   = pj_str(const_cast<char*>(sToHdr.c_str()));

    pjsip_method sipMethod = {PJSIP_OTHER_METHOD, {const_cast<char*>("MESSAGE"), (pj_ssize_t)strlen("MESSAGE")}};

    // sip keepAlive消息创建
    MySipTxDataPtr txDataPtr = nullptr;
    if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, nullptr, nullptr, -1, nullptr, &txDataPtr)) {
        LOG(WARNING) << "Sip reg app keepAlive up server failed. create sip message failed. " << upRegServInfoStr << ".";
        return MyStatus_t::FAILED;
    }

    // 指定传输端口为注册端口
    pjsip_tpselector tpSeclector;
    tpSeclector.type = PJSIP_TPSELECTOR_TRANSPORT;
    tpSeclector.disable_connection_reuse = false;

    MySipTransportPtr transportPtr = nullptr;
    if (MyTpProto_t::UDP == regUpServCfg.proto) {
        if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegUdpTp(&transportPtr)) {
            LOG(ERROR) << "Sip reg app register up server failed. get sip serv reg udp tp failed. " << upRegServInfoStr << ".";
            return MyStatus_t::FAILED;
        }
    }
    else {
        const auto& regUpServAddr = regUpServCfg.upSipServRegAddrCfg;
        if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegTcpTp(&transportPtr, regUpServAddr.ipAddr, regUpServAddr.port)) {
            LOG(ERROR) << "Sip reg app register up server failed. get sip serv reg tcp tp failed. " << upRegServInfoStr << ".";
            return MyStatus_t::FAILED;
        }
    }

    tpSeclector.u.transport = transportPtr;
    pjsip_tx_data_set_transport(txDataPtr, &tpSeclector);

    // sip keepAlive消息内容生成
    uint32_t keepAliveIdx = 0;
    if (MyStatus_t::SUCCESS != MySipRegManage::GetSipRegUpServKeepAliveIdx(regUpServCfg.upSipServRegAddrCfg.id, keepAliveIdx)) {
        LOG(WARNING) << "Sip reg app keepAlive up server failed. get keepAlive index failed. " << upRegServInfoStr << ".";
        return MyStatus_t::FAILED;
    }
    
    std::string sipMsgBody;
    MyXmlHelper::GenerateSipKeepAliveMsgBody(std::to_string(keepAliveIdx), localServCfg.id, sipMsgBody);

    // sip keepAlive消息内容创建
    pj_str_t type        = pj_str(const_cast<char*>("Application"));
    pj_str_t subtype     = pj_str(const_cast<char*>("MANSCDP+xml"));
    pj_str_t xmldata     = pj_str(const_cast<char*>(sipMsgBody.c_str()));
    txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

    // delete in function: MySipRegApp::OnKeepAliveRespCb()
    MySipRegUpServCfgPtr cpRegUpServCfgPtr = new MySipRegUpServCfg_dt(regUpServCfg);

    // sip keepAlive消息发送
    if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr ,-1, cpRegUpServCfgPtr, &MySipRegApp::OnKeepAliveRespCb)) {
        LOG(WARNING) << "Sip reg app keepAlive up server failed. send sip message failed. " << upRegServInfoStr << ".";
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

bool MySipRegApp::onTimer()
{
    if (MyStatus_t::SUCCESS != m_status.load()) {
        return false;
    }

    // 获取上级sip服务配置
    MySipServAddrCfg_dt servAddrCfg;
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServAddrCfg(servAddrCfg)) {
        LOG(ERROR) << "Sip reg app module keepAlive up server failed. get sip server addr cfg failed. " << m_servId << ".";
        return false;
    }
    
    MySipRegUpServCfgMap regUpServMap;
    if (MyStatus_t::SUCCESS != MySipRegManage::GetSipRegUpServCfgMap(regUpServMap)) {
        return true;
    }
    
    for (const auto& pair : regUpServMap) {
        std::string upRegServInfoStr;
        MyServerHelper::PrintSipUpRegServInfo(pair.second, upRegServInfoStr);

        int timeDiff = 0;
        unsigned int curRegUpServExpired = 0;
        if (MyStatus_t::SUCCESS != MySipRegManage::GetSipRegUpServExpired(pair.first, curRegUpServExpired)) {
            LOG(ERROR) << "Sip reg app module keepAlive up server failed. get sip reg up serv expired failed. " << upRegServInfoStr << ".";
            continue;
        }
        
        std::string curRegUpServLastRegTime;
        if (MyStatus_t::SUCCESS != MySipRegManage::GetSipRegUpServLastRegTime(pair.first, curRegUpServLastRegTime)) {
            LOG(ERROR) << "Sip reg app module keepAlive up server failed. get sip reg up serv last reg time failed. "<< upRegServInfoStr << ".";
            continue;
        }

        // 移除异常的上级sip服务
        if (MyStatus_t::SUCCESS != MyTimeHelper::CompareWithCurrentTime(curRegUpServLastRegTime, curRegUpServExpired, timeDiff)) {
            LOG(ERROR) << "Sip reg app module erase invalid sip up server. invalid sip up server regist time format. "<< upRegServInfoStr << ".";
                        
            MySipRegManage::DelSipRegUpServInfo(pair.first);
            continue;
        }
        
        // 移除超时的上级sip服务
        if (timeDiff < 0) {
            LOG(ERROR) << "Sip reg app module erase invalid sip up server. sip up server timeout. "<< upRegServInfoStr << ".";
                        
            MySipRegManage::DelSipRegUpServInfo(pair.first);
            continue;
        }

        // 上级sip服务注册即将超时，重新注册(小于5分钟)
        if (timeDiff <= (60 * 5)) {
            this->keepAliveUpServ(pair.second, servAddrCfg);
        }
    }

    // 获取下级sip服务配置
    MySipRegLowServCfgMap regLowServMap;
    if (MyStatus_t::SUCCESS != MySipRegManage::GetSipRegLowServCfgMap(regLowServMap)) {
        return true;
    }
    
    for (const auto& pair : regLowServMap) {
        std::string lowRegServInfoStr;
        MyServerHelper::PrintSipLowRegServInfo(pair.second, lowRegServInfoStr);

        int timeDiff = 0;
        unsigned int curRegLowServExpired = 0;
        if (MyStatus_t::SUCCESS != MySipRegManage::GetSipRegLowServExpired(pair.first, curRegLowServExpired)) {
            LOG(ERROR) << "Sip reg app module erase invalid sip low server. get sip reg low serv expired failed. " << lowRegServInfoStr << ".";
            continue;
        }
        
        std::string curRegLowServLastRegTime;
        if (MyStatus_t::SUCCESS != MySipRegManage::GetSipRegLowServLastRegTime(pair.first, curRegLowServLastRegTime)) {
            LOG(ERROR) << "Sip reg app module erase invalid sip low server. get sip reg low serv last reg time failed. "<< lowRegServInfoStr << ".";
            continue;
        }

        // 移除异常的下级sip服务
        if (MyStatus_t::SUCCESS != MyTimeHelper::CompareWithCurrentTime(curRegLowServLastRegTime, curRegLowServExpired, timeDiff)) {
            LOG(ERROR) << "Sip reg app module erase invalid sip low server. invalid sip low server regist time format. "<< lowRegServInfoStr << ".";
                        
            MySipRegManage::DelSipRegLowServInfo(pair.first);
            continue;
        }

        // 移除超时的下级sip服务
        if (timeDiff < 0) {
            LOG(ERROR) << "Sip reg app module erase invalid sip low server. sip low server timeout."<< lowRegServInfoStr << ".";

            MySipRegManage::DelSipRegLowServInfo(pair.first);
            MySipCatalogManage::DelSipCatalogInfo(pair.first);
            continue;
        }
    }
    return true;
}

MyStatus_t MySipRegApp::onLowSipServRegSuccess(const MySipRegLowServCfg_dt& sipRegLowServCfg)
{
    std::string lowRegServInfoStr;
    MyServerHelper::PrintSipLowRegServInfo(sipRegLowServCfg, lowRegServInfoStr);

    // 下级sip服务注册成功，触发向下级sip服务发送catalog请求
    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServer(sipServWkPtr)) {
        LOG(WARNING) << "Sip reg app trigger req low sip serv catalog failed. can't find sip server. " << lowRegServInfoStr << ".";
        return MyStatus_t::FAILED;
    }

    MySipServer::SmtPtr sipServPtr = sipServWkPtr.lock();
    if (MyStatus_t::SUCCESS != sipServPtr->onReqLowServCatalog(sipRegLowServCfg)) {
        LOG(WARNING) << "Sip reg app trigger req low sip serv catalog failed. MySipServer::onReqLowServCatalog() failed. " << lowRegServInfoStr << ".";
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegApp::onRecvSipRegReqMsg(MySipRxDataPtr rxDataPtr)
{
    // 解析下级sip服务id
    MySipMsgHdrPtr contactHeader = (MySipMsgHdrPtr)pjsip_msg_find_hdr(rxDataPtr->msg_info.msg, PJSIP_H_CONTACT, nullptr);
    if (nullptr == contactHeader) {
        LOG(ERROR) << "Sip reg app receive register request message failed. Can't find contact header. ";
        return MyStatus_t::FAILED;
    }

    char buf[512];
    pjsip_hdr_print_on(contactHeader, buf, sizeof(buf));

    MySipMsgContactHdr_dt sipContactHeader;
    if (MyStatus_t::SUCCESS != MySipMsgHelper::ParseSipMsgContactHdr(buf, sipContactHeader)) {
        LOG(ERROR) << "Sip reg app receive register request message failed. Can't parse contact header. ";
        return MyStatus_t::FAILED;
    }

    std::string contactHeaderStr;
    MySipMsgHelper::PrintSipMsgContactHdr(sipContactHeader, contactHeaderStr);

    // 解析下级sip服务注册有效时间
    MySipMsgHdrPtr expireHeader = (MySipMsgHdrPtr)pjsip_msg_find_hdr(rxDataPtr->msg_info.msg, PJSIP_H_EXPIRES, nullptr);
    if (nullptr == expireHeader) {
        LOG(ERROR) << "Sip reg app receive register request message failed. Can't find expire header. " << contactHeaderStr << ".";
        return MyStatus_t::FAILED;
    }

    memset(buf, 0, sizeof(buf));
    pjsip_hdr_print_on(expireHeader, buf, sizeof(buf));
    
    double expire = 0.0;
    if (MyStatus_t::SUCCESS != MySipMsgHelper::ParseSipMsgExpireHdr(buf, expire)) {
        LOG(ERROR) << "Sip reg app receive register request message failed. Can't parse expire header. " << contactHeaderStr << ".";
        return MyStatus_t::FAILED;
    }

    // 解析下级sip服务鉴权信息
    MySipMsgHdrPtr authHeader = (MySipMsgHdrPtr)pjsip_msg_find_hdr(rxDataPtr->msg_info.msg, PJSIP_H_AUTHORIZATION, nullptr);
    if (nullptr != authHeader) {
        memset(buf, 0, sizeof(buf));
        pjsip_hdr_print_on(authHeader, buf, sizeof(buf));
    }

    // 获取下级sip服务配置
    MySipRegLowServCfgMap regLowServMap;
    if (MyStatus_t::SUCCESS != MySipRegManage::GetSipRegLowServCfgMap(regLowServMap)) {
        // 下级服务首次注册，通过配置查找
        if (MyStatus_t::SUCCESS != MySystemConfig::GetSipLowRegServCfgMap(regLowServMap)) {
            // 下级sip服务配置不存在
            LOG(ERROR) << "Sip reg app receive register request message failed. Can't find low register server config. " << contactHeaderStr << ".";
            return MyStatus_t::FAILED;
        }
    }

    // 查找下级sip服务配置
    auto cfgIter = regLowServMap.find(sipContactHeader.id);
    if (regLowServMap.end() == cfgIter) {
        // 下级服务首次注册，通过配置查找
        if (MyStatus_t::SUCCESS != MySystemConfig::GetSipLowRegServCfgMap(regLowServMap)) {
            LOG(ERROR) << "Sip reg app receive register request message failed. Can't find low register server config. " << contactHeaderStr << ".";
            return MyStatus_t::FAILED;
        }
        
        // 下级sip服务配置不存在
        cfgIter = regLowServMap.find(sipContactHeader.id);
        if (cfgIter == regLowServMap.end()) {
            LOG(ERROR) << "Sip reg app receive register request message failed. Can't find low register server config. " << contactHeaderStr << ".";
            return MyStatus_t::FAILED;
        }
    }

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip reg app receive register request message failed. Can't get pjsip endpoint. " << contactHeaderStr << ".";
        return MyStatus_t::FAILED;
    }

    // 创建sip应答消息
    pjsip_tx_data *txDataPtr = nullptr;

    pjsip_hdr hdrList;
    pj_list_init(&hdrList);

    int statusCode = 200;

    // 向sip应答消息添加Date首部
    std::string curTime;
    MyTimeHelper::GetCurrentSipHeaderTime(curTime);

    pj_str_t valueTime = pj_str(const_cast<char*>(curTime.c_str()));
    pj_str_t keyTime   = pj_str(const_cast<char*>("Date"));

    MySipMsgDateHdrPtr dateHdrPtr = (MySipMsgDateHdrPtr)pjsip_date_hdr_create(rxDataPtr->tp_info.pool, &keyTime, &valueTime);
    pj_list_push_back(&hdrList, dateHdrPtr);

    // 对下级sip服务启用鉴权
    if (cfgIter->second.lowSipServRegAuthCfg.enableAuth) {
        // 下级sip服务注册时未携带鉴权信息
        if (nullptr == authHeader) {
            // 填充鉴权信息
            MySipMsgAuthHdrPtr authHdrPtr = pjsip_www_authenticate_hdr_create(rxDataPtr->tp_info.pool);
            authHdrPtr->scheme = pj_str(const_cast<char*>("digest"));

            std::string nonce;
            MyRandomHelper::Get32BytesLenRandomStr(nonce);
            authHdrPtr->challenge.digest.nonce = pj_str(const_cast<char*>(nonce.c_str()));

            authHdrPtr->challenge.digest.realm = pj_str(const_cast<char*>(cfgIter->second.lowSipServRegAuthCfg.authRealm.c_str()));

            std::string opaque;
            MyRandomHelper::Get32BytesLenRandomStr(opaque);
            authHdrPtr->challenge.digest.opaque = pj_str(const_cast<char*>(opaque.c_str()));

            authHdrPtr->challenge.digest.algorithm = pj_str(const_cast<char*>("MD5"));
            pj_list_push_back(&hdrList, authHdrPtr);

            statusCode = 401;
        }
        else {
            // 下级sip服务注册时携带鉴权信息
            pjsip_auth_srv authSrv;
            pj_str_t realm = pj_str(const_cast<char*>(cfgIter->second.lowSipServRegAuthCfg.authRealm.c_str()));

            // 鉴权信息初始化
            if(PJ_SUCCESS != pjsip_auth_srv_init(rxDataPtr->tp_info.pool, &authSrv, &realm, &MySipRegApp::OnRegFillAuthInfoCb, 0)) {
                LOG(ERROR) << "Sip reg app receive register request message failed. create auth info failed." << contactHeaderStr << ".";
                statusCode = 401;
            }

            // 鉴权处理
            pjsip_auth_srv_verify(&authSrv, rxDataPtr, &statusCode);

            // 鉴权失败
            if (200 != statusCode) {
                LOG(ERROR) << "Sip reg app receive register request message failed. auth failed." << contactHeaderStr << ".";
            }
        }
    }
    
    // 下级sip服务注册信息更新
    const auto& lowSipServAddrCfg = cfgIter->second;
    if (expire > 0) {
        // 下级sip服务注册
        if (MyStatus_t::SUCCESS != MySipRegManage::HasSipRegLowServInfo(sipContactHeader.id)) {
            // 添加下级sip服务配置
            MySipLowRegServInfo_dt lowRegServInfo;
            lowRegServInfo.sipRegLowServCfg            = lowSipServAddrCfg;        
            lowRegServInfo.sipRegLowServExpired        = expire;
            MyTimeHelper::GetCurrentFormatTime(lowRegServInfo.sipRegLowServLastRegTime);

            if (lowSipServAddrCfg.lowSipServRegAddrCfg.ipAddr != sipContactHeader.ipAddr || lowSipServAddrCfg.lowSipServRegAddrCfg.port != sipContactHeader.port) {
                lowRegServInfo.sipRegLowServCfg.lowSipServRegAddrCfg.ipAddr  = sipContactHeader.ipAddr;
                lowRegServInfo.sipRegLowServCfg.lowSipServRegAddrCfg.port    = sipContactHeader.port;
            }
            MySipRegManage::AddSipRegLowServInfo(lowRegServInfo);
        }
        else {
            // 更新下级sip服务配置
            MySipRegManage::UpdateSipRegLowServExpired(cfgIter->second.lowSipServRegAddrCfg.id, expire);

            std::string lastRegTime;
            MyTimeHelper::GetCurrentFormatTime(lastRegTime);
            MySipRegManage::UpdateSipRegLowServLastRegTime(cfgIter->second.lowSipServRegAddrCfg.id, lastRegTime);

            if (lowSipServAddrCfg.lowSipServRegAddrCfg.ipAddr != sipContactHeader.ipAddr || lowSipServAddrCfg.lowSipServRegAddrCfg.port != sipContactHeader.port) {
                auto tmpAddrCfg = lowSipServAddrCfg.lowSipServRegAddrCfg;
                tmpAddrCfg.ipAddr = sipContactHeader.ipAddr;
                tmpAddrCfg.port   = sipContactHeader.port;
                MySipRegManage::UpdateSipRegLowServIpAddr(sipContactHeader.id, tmpAddrCfg);
            }
        }
    }
    else {
        // 下级sip服务注销
        MySipRegManage::DelSipRegLowServInfo(sipContactHeader.id);
    }

    // 发送sip应答消息
    if (PJ_SUCCESS != pjsip_endpt_respond(endptPtr, nullptr, rxDataPtr, statusCode, nullptr, &hdrList, nullptr, nullptr)) {
        LOG(ERROR) << "Sip reg app receive register request message failed. can't send response message. " << contactHeaderStr << ".";
        return MyStatus_t::FAILED;
    }

    if (200 != statusCode && 401 != statusCode) {
        LOG(ERROR) << "Sip reg app receive register request message failed. regist failed. " << " code: " << statusCode 
                   << " " << contactHeaderStr << ".";
        return MyStatus_t::FAILED;
    }

    if (200 == statusCode) {
        this->onLowSipServRegSuccess(lowSipServAddrCfg);
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegApp::onSipRegAppRecvSipKeepAliveReqMsg(MySipRxDataPtr rxDataPtr)
{
    MySipKeepAliveMsgBody_dt keepAliveMsgBody;
    if (MyStatus_t::SUCCESS != MyXmlHelper::ParseSipKeepAliveMsgBody(static_cast<char*>(rxDataPtr->msg_info.msg->body->data), keepAliveMsgBody)) {
        LOG(ERROR) << "Sip reg app module recv keepAlive message. parse keepalive msg failed.";
        return MyStatus_t::FAILED;
    }

    std::string keepAliveMsgBodyStr;
    MySipMsgHelper::PrintSipKeepAliveMsgBody(keepAliveMsgBody, keepAliveMsgBodyStr);

    // 查询下级sip服务注册信息
    int statusCode = 200;
    if (MyStatus_t::SUCCESS != MySipRegManage::HasSipRegLowServInfo(keepAliveMsgBody.deviceId)) {
        // 下级sip服务未注册
        statusCode = 403;
        LOG(ERROR) << "Sip reg app module recv keepAlive message. find lower sip server failed. device id: " << keepAliveMsgBodyStr << ".";
    }
    else {
        // 更新下级sip服务注册信息
        std::string lastRegTime;
        MyTimeHelper::GetCurrentFormatTime(lastRegTime);
        MySipRegManage::UpdateSipRegLowServLastRegTime(keepAliveMsgBody.deviceId, lastRegTime);
    }

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip reg app module recv keepAlive message failed. get pjsip endpoint failed. " << keepAliveMsgBodyStr << ".";
        return MyStatus_t::FAILED;
    }

    // 发送sip应答消息
    if (PJ_SUCCESS != pjsip_endpt_respond(endptPtr, nullptr, rxDataPtr, statusCode, nullptr, nullptr, nullptr, nullptr)) {
        LOG(ERROR) << "Sip reg app receive keepAlive message failed. can't send response message. " << keepAliveMsgBodyStr << ".";
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegApp::getState(MyStatus_t& status) const
{
    status = m_status.load();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegApp::getId(std::string& id) const
{
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(ERROR) << "Sip app module get id failed. MySipCatalogApp is not init.";
        return MyStatus_t::FAILED;
    }

    id = m_appIdCfg.id;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegApp::getSipRegApp(MySipRegApp::SmtWkPtr& wkPtr)
{
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "Sip app module get work pointer failed. MySipRegApp is not init.";
        return MyStatus_t::FAILED;
    }

    wkPtr = this->shared_from_this();
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_APP