#include <thread>
#include <chrono>
#include <cstdlib>
#include <cstring>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySystemConfg.h"
#include "envir/mySystemPjsip.h"
#include "manager/myServManage.h"
#include "manager/mySipServRegManage.h"
#include "utils/myXmlHelper.h"
#include "utils/myTimeHelper.h"
#include "utils/mySipAppHelper.h"
#include "utils/mySipMsgHelper.h"
#include "utils/myRandomHelper.h"
#include "utils/mySipServerHelper.h"
#include "app/mySipRegApp.h"
using namespace MY_UTILS;
using namespace MY_COMMON;
using toolkit::Timer;
using toolkit::EventPollerPool;
using MY_ENVIR::MySystemPjsip;
using MY_ENVIR::MySystemConfig;
using MY_MANAGER::MyServManage;
using MY_MANAGER::MySipServRegManage;

namespace MY_APP {

void MySipRegApp::OnRegRespCb(MySipRegCbParamPtr regParamPtr)
{
    if (nullptr == regParamPtr) {
        LOG(ERROR) << "Sip app reg module register response callback failed. invalid param.";
        return;
    }
}

pj_status_t MySipRegApp::OnRegFillAuthInfoCb(MySipPoolPtr poolPtr, MySipStrCstPtr realmPtr, 
                                             MySipStrCstPtr namePtr, MySipCredInfoPtr credInfoPtr)
{
    std::string nameStr  = std::string(namePtr->ptr).substr(0, namePtr->slen);
    std::string realmStr = std::string(realmPtr->ptr).substr(0, realmPtr->slen);

    MySipServAuthCfg_dt authCfg = MySipServRegManage::GetSipRegLowAuthCfg(nameStr, realmStr);
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

void MySipRegApp::OnKeepAliveRespCb(MySipEvThdCbParamPtr evParamPtr, MySipEvPtr evPtr)
{
    MySipRegUpServCfgPtr sipRegUpServCfgPtr = static_cast<MySipRegUpServCfgPtr>(evParamPtr); 

    // 查找本级sip服务id
    std::string servId = MySipServRegManage::GetSipLocalServId(sipRegUpServCfgPtr->upSipServAddrCfg.id, true);
    if (servId.empty()) {
        LOG(ERROR) << "Sip app reg module keep alive response callback failed. invalid up reg sip server. id: " << sipRegUpServCfgPtr->upSipServAddrCfg.id;
        return;
    }

    if(200 != evPtr->body.tsx_state.tsx->status_code) {
        // 移除无效上级服务
        MySipServRegManage::DelSipRegUpServInfo(servId, sipRegUpServCfgPtr->upSipServAddrCfg.id);
    }
    else {
        // 更新有效上级服务
        uint32_t keepAliveIdx = MySipServRegManage::GetSipRegUpServKeepAliveIdx(servId, sipRegUpServCfgPtr->upSipServAddrCfg.id);
        MySipServRegManage::UpdateSipRegUpServKeepAliveIdx(servId, sipRegUpServCfgPtr->upSipServAddrCfg.id, ++keepAliveIdx);
        MySipServRegManage::UpdateSipRegUpServLastRegTime(servId, sipRegUpServCfgPtr->upSipServAddrCfg.id, MyTimeHelper::GetCurrentFormatTime());
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
        LOG(WARNING) << "Sip app module init failed. MySipRegApp is init already.";
        return MyStatus_t::SUCCESS;
    }

    // 获取endpoint
    MySipEndptPtr endptPtr = MySystemPjsip::GetPjsipEndptPtr();
    if (nullptr == endptPtr) {
        LOG(ERROR) << "Sip app module init failed. invalid endpoint.";
        return MyStatus_t::FAILED;
    }

    // 服务id赋值
    m_servId = servId;

    // app标识初始化 
    m_appIdCfg.id       = id;
    m_appIdCfg.name     = name;   
    m_appIdCfg.priority = priority;

    LOG(INFO) << "Sip app module init success. " << MySipAppHelper::GetSipAppInfo(m_appIdCfg) << ".";

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
        MySipRegApp::SmtWkPtr weakSelf = this->getSipRegApp();
        m_timePtr = std::make_shared<Timer>(3.0f, [weakSelf]() -> bool {
            if (weakSelf.expired()) {
                return false;
            }
            return weakSelf.lock()->onTimer();
        }, EventPollerPool::Instance().getFirstPoller());
    }

    // 获取本地sip服务地址配置
    MySipServAddrCfg_dt servAddrCfg = MyServManage::GetSipServAddrCfg(m_servId);
    
    // 获取上级sip服务配置
    MySipRegUpServCfgMap regUpServMap = MySipServRegManage::GetSipRegUpServCfgMap(m_servId);

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
        LOG(WARNING) << "Sip app module shutdown failed. MySipRegApp is not init.";
        return MyStatus_t::SUCCESS;
    }

    // 定时器销毁
    if (nullptr != m_timePtr) {
        m_timePtr.reset();
    }

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegApp::regUpServ(const MySipRegUpServCfg_dt& regUpServCfg, const MySipServAddrCfg_dt& localServCfg)
{
    // 获取endpoint
    MySipEndptPtr endptPtr = MySystemPjsip::GetPjsipEndptPtr();
    if (nullptr == endptPtr) {
        LOG(ERROR) << "Sip app register up server failed. invalid endpoint. " 
                   << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
        return MyStatus_t::FAILED;
    }

    // sip register消息首部生成
    std::string sURL     = MySipMsgHelper::GenerateSipMsgURL(regUpServCfg.upSipServAddrCfg.id, regUpServCfg.upSipServAddrCfg.ipAddr, 
                                                             regUpServCfg.upSipServAddrCfg.port, regUpServCfg.proto);
    std::string sFromHdr = MySipMsgHelper::GenerateSipMsgFromHeader(localServCfg.id, localServCfg.ipAddr);
    std::string sToHdr   = MySipMsgHelper::GenerateSipMsgToHeader(localServCfg.id, localServCfg.ipAddr);
    std::string sContact = MySipMsgHelper::GenerateSipMsgContactHeader(localServCfg.id, localServCfg.ipAddr, localServCfg.port);

    pj_str_t sipMsgURL     = pj_str(const_cast<char*>(sURL.c_str()));
    pj_str_t sipMsgFromHdr = pj_str(const_cast<char*>(sFromHdr.c_str()));
    pj_str_t sipMsgToHdr   = pj_str(const_cast<char*>(sToHdr.c_str()));
    pj_str_t sipMsgContact = pj_str(const_cast<char*>(sContact.c_str()));

    MySipRegcPtr         regcPtr        = nullptr;
    MySipRegUpServCfg_dt cpRegUpServCfg = regUpServCfg;
    
    // sip register上下文创建
    if(PJ_SUCCESS != pjsip_regc_create(endptPtr, &cpRegUpServCfg, &MySipRegApp::OnRegRespCb, &regcPtr)) {
        LOG(ERROR) << "Sip app register up server failed. pjsip_regc_create() error. " 
                   << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
        return MyStatus_t::FAILED;
    }

    // sip register上下文初始化
    uint32_t expired = 3600;
    if(PJ_SUCCESS != pjsip_regc_init(regcPtr, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, 1, &sipMsgContact, expired)) {
        pjsip_regc_destroy(regcPtr);
        LOG(ERROR) << "Sip app register up server failed. pjsip_regc_init() error. " 
                   << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
        return MyStatus_t::FAILED;
    }

    // sip register授权
    if (cpRegUpServCfg.upSipServAuthCfg.enableAuth) {
        pjsip_cred_info cred;
        pj_bzero(&cred, sizeof(pjsip_cred_info));

        cred.scheme    = pj_str(const_cast<char*>("digest"));
        cred.realm     = pj_str(const_cast<char*>(cpRegUpServCfg.upSipServAuthCfg.authRealm.c_str()));
        cred.username  = pj_str(const_cast<char*>(cpRegUpServCfg.upSipServAuthCfg.authName.c_str()));
        cred.data      = pj_str(const_cast<char*>(cpRegUpServCfg.upSipServAuthCfg.authPwd.c_str()));
        cred.data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;

        if(PJ_SUCCESS != pjsip_regc_set_credentials(regcPtr, 1, &cred)) {
            pjsip_regc_destroy(regcPtr);
            LOG(ERROR) << "Sip app register up server failed. pjsip_regc_set_credentials() error. " 
                       << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
            return MyStatus_t::FAILED;
        }
    }

    // sip register消息创建
    MySipTxDataPtr txDataPtr = nullptr;
    if(PJ_SUCCESS != pjsip_regc_register(regcPtr, PJ_TRUE, &txDataPtr)) {
        pjsip_regc_destroy(regcPtr);

        LOG(ERROR) << "Sip app register up server failed. pjsip_regc_destroy() error. " 
                   << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
        return MyStatus_t::FAILED;
    }

    // sip register消息发送
    if(PJ_SUCCESS != pjsip_regc_send(regcPtr, txDataPtr)) {
        pjsip_regc_destroy(regcPtr);

        LOG(ERROR) << "Sip app register up server failed. pjsip_regc_send() error. " 
                   << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
        return MyStatus_t::FAILED;
    }

    // 保存sip reg上下文
    MySipUpRegServInfo_dt upRegServInfo;
    upRegServInfo.sipRegUpServCfg           = cpRegUpServCfg;
    upRegServInfo.sipRegUpServLastRegTime   = MyTimeHelper::GetCurrentFormatTime();
    upRegServInfo.sipRegUpServKeepAliveIdx  = 0;
    upRegServInfo.sipRegUpServExpired       = expired;
    return MySipServRegManage::AddSipRegUpServInfo(localServCfg.id, upRegServInfo);
}

MyStatus_t MySipRegApp::keepAliveUpServ(const MySipRegUpServCfg_dt& regUpServCfg, const MySipServAddrCfg_dt& localServCfg)
{
    // 获取endpoint
    MySipEndptPtr endptPtr = MySystemPjsip::GetPjsipEndptPtr();
    if (nullptr == endptPtr) {
        LOG(ERROR) << "Sip app keepAlive up server failed. get pjsip endpoint failed. " << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
        return MyStatus_t::FAILED;
    }

    // sip keepAlive消息首部生成
    std::string sURL     = MySipMsgHelper::GenerateSipMsgURL(regUpServCfg.upSipServAddrCfg.id, regUpServCfg.upSipServAddrCfg.ipAddr, 
                                                             regUpServCfg.upSipServAddrCfg.port, regUpServCfg.proto);
    std::string sFromHdr = MySipMsgHelper::GenerateSipMsgFromHeader(localServCfg.id, localServCfg.ipAddr);
    std::string sToHdr   = MySipMsgHelper::GenerateSipMsgToHeader(localServCfg.id, localServCfg.ipAddr);

    pj_str_t sipMsgURL     = pj_str(const_cast<char*>(sURL.c_str()));
    pj_str_t sipMsgFromHdr = pj_str(const_cast<char*>(sFromHdr.c_str()));
    pj_str_t sipMsgToHdr   = pj_str(const_cast<char*>(sToHdr.c_str()));

    pjsip_method sipMethod = {PJSIP_OTHER_METHOD, {const_cast<char*>("MESSAGE"), strlen("MESSAGE")}};

    // sip keepAlive消息创建
    MySipTxDataPtr txDataPtr = nullptr;
    if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, nullptr, nullptr, -1, nullptr, &txDataPtr)) {
        LOG(WARNING) << "Sip app keepAlive up server failed. create sip message failed. " 
                     << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
        return MyStatus_t::FAILED;
    }

    // sip keepAlive消息内容生成
    uint32_t    keepAliveIdx = MySipServRegManage::GetSipRegUpServKeepAliveIdx(localServCfg.id, regUpServCfg.upSipServAddrCfg.id);
    std::string sipMsgBody   = MyXmlHelper::GenerateSipKeepAliveBody(std::to_string(keepAliveIdx), localServCfg.id);

    // sip keepAlive消息内容创建
    pj_str_t type        = pj_str("Application");
    pj_str_t subtype     = pj_str("MANSCDP+xml");
    pj_str_t xmldata     = pj_str(const_cast<char*>(sipMsgBody.c_str()));
    txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

    // sip keepAlive消息发送
    MySipRegUpServCfg_dt cpRegUpServCfg = regUpServCfg;
    if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr ,-1, &cpRegUpServCfg, &MySipRegApp::OnKeepAliveRespCb)) {
        LOG(WARNING) << "Sip app keepAlive up server failed. send sip message failed. " 
                     << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

bool MySipRegApp::onTimer()
{
    if (MyStatus_t::SUCCESS != m_status.load()) {
        return false;
    }

    MySipServAddrCfg_dt servAddrCfg = MyServManage::GetSipServAddrCfg(m_servId);
    MySipEndptPtr       endptPtr    = MySystemPjsip::GetPjsipEndptPtr();

    // 获取上级sip服务配置
    MySipRegUpServCfgMap regUpServMap = MySipServRegManage::GetSipRegUpServCfgMap(m_servId);
    for (const auto& pair : regUpServMap) {
        int          timeDiff                = 0;
        unsigned int curRegUpServExpired     = MySipServRegManage::GetSipRegUpServExpired(m_servId, pair.first);
        std::string  curRegUpServLastRegTime = MySipServRegManage::GetSipRegUpServLastRegTime(m_servId, pair.first);

        // 移除异常的上级sip服务
        if (MyStatus_t::SUCCESS != MyTimeHelper::CompareWithCurrentTime(curRegUpServLastRegTime, curRegUpServExpired, timeDiff)) {
            LOG(ERROR) << "Sip app module erase invalid sip up server. invalid sip up server regist time format. "
                       <<  MySipServerHelper::GetSipUpRegServInfo(pair.second) << ".";
                        
            MySipServRegManage::DelSipRegUpServInfo(m_servId, pair.first);
            continue;
        }
        
        // 移除超时的上级sip服务
        if (timeDiff <= 0) {
            LOG(ERROR) << "Sip app module erase invalid sip up server. sip up server timeout. "
                       <<  MySipServerHelper::GetSipUpRegServInfo(pair.second) << ".";
                        
            MySipServRegManage::DelSipRegUpServInfo(m_servId, pair.first);
            continue;
        }

        // 上级sip服务注册即将超时，重新注册(小于5分钟)
        if (timeDiff < (60 * 5)) {
            this->keepAliveUpServ(pair.second, servAddrCfg);
        }
    }

    // 获取下级sip服务配置
    MySipRegLowServCfgMap regLowServMap = MySipServRegManage::GetSipRegLowServCfgMap(m_servId);
    for (const auto& pair : regLowServMap) {
        int          timeDiff                 = 0;
        unsigned int curRegLowServExpired     = MySipServRegManage::GetSipRegLowServExpired(m_servId, pair.first);
        std::string  curRegLowServLastRegTime = MySipServRegManage::GetSipRegLowServLastRegTime(m_servId, pair.first);

        // 移除异常的下级sip服务
        if (MyStatus_t::SUCCESS != MyTimeHelper::CompareWithCurrentTime(curRegLowServLastRegTime, curRegLowServExpired, timeDiff)) {
            LOG(ERROR) << "Sip app module erase invalid sip low server. invalid sip low server regist time format. "
                       <<  MySipServerHelper::GetSipLowRegServInfo(pair.second) << ".";
                        
            MySipServRegManage::DelSipRegLowServInfo(m_servId, pair.first);
            continue;
        }

        // 移除超时的下级sip服务
        if (timeDiff <= 0) {
            LOG(ERROR) << "Sip app module erase invalid sip low server. sip low server timeout."
                       <<  MySipServerHelper::GetSipLowRegServInfo(pair.second) << ".";

            MySipServRegManage::DelSipRegLowServInfo(m_servId, pair.first);
            continue;
        }
    }
    return true;
}

MyStatus_t MySipRegApp::onRecvSipRegReqMsg(MySipRxDataPtr rxDataPtr)
{
    // 解析下级sip服务id
    MySipMsgHdrPtr contactHeader = (MySipMsgHdrPtr)pjsip_msg_find_hdr(rxDataPtr->msg_info.msg, PJSIP_H_CONTACT, nullptr);
    if (nullptr == contactHeader) {
        LOG(ERROR) << "Sip app receive register request message failed. Can't find contact header. ";
        return MyStatus_t::FAILED;
    }

    char buf[512];
    pjsip_hdr_print_on(contactHeader, buf, sizeof(buf));

    MySipMsgContactHdr_dt sipContactHeader;
    if (MyStatus_t::SUCCESS != MySipMsgHelper::ParseSipMsgContactHdr(buf, sipContactHeader)) {
        LOG(ERROR) << "Sip app receive register request message failed. Can't parse contact header. ";
        return MyStatus_t::FAILED;
    }

    // 解析下级sip服务注册有效时间
    MySipMsgHdrPtr expireHeader = (MySipMsgHdrPtr)pjsip_msg_find_hdr(rxDataPtr->msg_info.msg, PJSIP_H_EXPIRES, nullptr);
    if (nullptr == expireHeader) {
        LOG(ERROR) << "Sip app receive register request message failed. Can't find expire header. " 
                   << MySipMsgHelper::PrintSipMsgContactHdr(sipContactHeader) << ".";
        return MyStatus_t::FAILED;
    }

    memset(buf, 0, sizeof(buf));
    pjsip_hdr_print_on(expireHeader, buf, sizeof(buf));
    
    double expire = 0.0;
    if (MyStatus_t::SUCCESS != MySipMsgHelper::ParseSipMsgExpireHdr(buf, expire)) {
        LOG(ERROR) << "Sip app receive register request message failed. Can't parse expire header. " 
                   << MySipMsgHelper::PrintSipMsgContactHdr(sipContactHeader) << ".";
        return MyStatus_t::FAILED;
    }

    // 解析下级sip服务鉴权信息
    MySipMsgHdrPtr authHeader = (MySipMsgHdrPtr)pjsip_msg_find_hdr(rxDataPtr->msg_info.msg, PJSIP_H_AUTHORIZATION, nullptr);
    if (nullptr != authHeader) {
        memset(buf, 0, sizeof(buf));
        pjsip_hdr_print_on(authHeader, buf, sizeof(buf));
    }

    // 获取下级sip服务配置
    MySipRegLowServCfgMap regLowServMap = MySipServRegManage::GetSipRegLowServCfgMap(m_servId);
    auto cfgIter = regLowServMap.find(sipContactHeader.id);
    if (regLowServMap.end() == cfgIter) {
        // 下级sip服务配置不存在
        LOG(ERROR) << "Sip app receive register request message failed. Can't find low register server config. " 
                   << MySipMsgHelper::PrintSipMsgContactHdr(sipContactHeader) << ".";
        return MyStatus_t::FAILED;
    }

    // 获取endpoint
    MySipEndptPtr endptPtr = MySystemPjsip::GetPjsipEndptPtr();

    pjsip_hdr hdrList;
    pj_list_init(&hdrList);

    int statusCode = 200;

    // 向sip应答消息添加Date首部
    std::string curTime    = MyTimeHelper::GetCurrentSipHeaderTime();
    pj_str_t    valueTime  = pj_str(const_cast<char*>(curTime.c_str()));
    pj_str_t    keyTime    = pj_str("Date");

    MySipMsgDateHdrPtr dateHdrPtr = (MySipMsgDateHdrPtr)pjsip_date_hdr_create(rxDataPtr->tp_info.pool, &keyTime, &valueTime);
    pj_list_push_back(&hdrList, dateHdrPtr);

    // 对下级sip服务启用鉴权
    if (cfgIter->second.lowSipServAuthCfg.enableAuth) {
        // 下级sip服务注册时未携带鉴权信息
        if (nullptr == authHeader) {
            // 填充鉴权信息
            MySipMsgAuthHdrPtr authHdrPtr = pjsip_www_authenticate_hdr_create(rxDataPtr->tp_info.pool);
            authHdrPtr->scheme = pj_str("digest");

            std::string nonce = MyRandomHelper::Get32BitsLenRandomStr();
            authHdrPtr->challenge.digest.nonce = pj_str(const_cast<char*>(nonce.c_str()));

            authHdrPtr->challenge.digest.realm = pj_str(const_cast<char*>(cfgIter->second.lowSipServAuthCfg.authRealm.c_str()));

            std::string opaque = MyRandomHelper::Get32BitsLenRandomStr();
            authHdrPtr->challenge.digest.opaque = pj_str(const_cast<char*>(opaque.c_str()));

            authHdrPtr->challenge.digest.algorithm = pj_str("MD5");
            pj_list_push_back(&hdrList, authHdrPtr);

            statusCode = 401;
        }
        else {
            // 下级sip服务注册时携带鉴权信息
            pjsip_auth_srv authSrv;
            pj_str_t realm = pj_str(const_cast<char*>(cfgIter->second.lowSipServAuthCfg.authRealm.c_str()));

            // 鉴权信息初始化
            if(PJ_SUCCESS != pjsip_auth_srv_init(rxDataPtr->tp_info.pool, &authSrv, &realm, &MySipRegApp::OnRegFillAuthInfoCb, 0)) {
                LOG(ERROR) << "Sip app receive register request message failed. create auth info failed."
                           << MySipMsgHelper::PrintSipMsgContactHdr(sipContactHeader) << ".";
                statusCode = 401;
            }

            // 鉴权处理
            pjsip_auth_srv_verify(&authSrv, rxDataPtr, &statusCode);

            // 鉴权失败
            if (200 != statusCode) {
                LOG(ERROR) << "Sip app receive register request message failed. auth failed."
                           << MySipMsgHelper::PrintSipMsgContactHdr(sipContactHeader) << ".";
            }
        }
    }
    
    // 下级sip服务注册信息更新
    const auto& lowSipServAddrCfg = cfgIter->second;
    if (expire > 0) {
        // 下级sip服务注册
        if (MyStatus_t::SUCCESS != MySipServRegManage::HasSipRegLowServInfo(m_servId, sipContactHeader.id)) {
            // 添加下级sip服务配置
            MySipLowRegServInfo_dt lowRegServInfo;
            lowRegServInfo.sipRegLowServCfg            = lowSipServAddrCfg;        
            lowRegServInfo.sipRegLowServExpired        = expire;
            lowRegServInfo.sipRegLowServLastRegTime    = MyTimeHelper::GetCurrentFormatTime();

            if (lowSipServAddrCfg.lowSipServAddrCfg.ipAddr != sipContactHeader.ipAddr || lowSipServAddrCfg.lowSipServAddrCfg.port != sipContactHeader.port) {
                lowRegServInfo.sipRegLowServCfg.lowSipServAddrCfg.ipAddr = sipContactHeader.ipAddr;
                lowRegServInfo.sipRegLowServCfg.lowSipServAddrCfg.port   = sipContactHeader.port;
            }
            MySipServRegManage::AddSipRegLowServInfo(m_servId, lowRegServInfo);
        }
        else {
            // 更新下级sip服务配置
            MySipServRegManage::UpdateSipRegLowServExpired(m_servId, cfgIter->second.lowSipServAddrCfg.id, expire);
            MySipServRegManage::UpdateSipRegLowServLastRegTime(m_servId, cfgIter->second.lowSipServAddrCfg.id, MyTimeHelper::GetCurrentFormatTime());

            if (lowSipServAddrCfg.lowSipServAddrCfg.ipAddr != sipContactHeader.ipAddr || lowSipServAddrCfg.lowSipServAddrCfg.port != sipContactHeader.port) {
                auto tmpAddrCfg = lowSipServAddrCfg.lowSipServAddrCfg;
                tmpAddrCfg.ipAddr = sipContactHeader.ipAddr;
                tmpAddrCfg.port   = sipContactHeader.port;
                MySipServRegManage::UpdateSipRegLowServIpAddr(m_servId, sipContactHeader.id, tmpAddrCfg);
            }
        }
    }
    else {
        // 下级sip服务注销
        MySipServRegManage::DelSipRegLowServInfo(m_servId, sipContactHeader.id);
    }

    // 发送sip应答消息
    if (PJ_SUCCESS != pjsip_endpt_respond(endptPtr, nullptr, rxDataPtr, statusCode, nullptr, &hdrList, nullptr, nullptr)) {
        LOG(ERROR) << "Sip app receive register request message failed. can't send response message. "
                   << MySipMsgHelper::PrintSipMsgContactHdr(sipContactHeader) << ".";
        return MyStatus_t::FAILED;
    }

    if (200 != statusCode) {
        LOG(ERROR) << "Sip app receive register request message failed. regist failed. " << " code: " << statusCode
                   << MySipMsgHelper::PrintSipMsgContactHdr(sipContactHeader) << ".";
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_APP