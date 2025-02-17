#include <cstring>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySystemConfg.h"
#include "envir/mySystemPjsip.h"
#include "utils/myXmlHelper.h"
#include "utils/mySipAppHelper.h"
#include "utils/mySipMsgHelper.h"
#include "utils/mySipServerHelper.h"
#include "manager/myServManage.h"
#include "manager/mySipServRegManage.h"
#include "manager/mySipServCatalogManager.h"
#include "app/mySipCatalogApp.h"
using namespace toolkit;
using namespace MY_UTILS;
using namespace MY_COMMON;
using MY_ENVIR::MySystemPjsip;
using MY_ENVIR::MySystemConfig;
using MY_MANAGER::MyServManage;
using MY_MANAGER::MySipServRegManage;
using MY_MANAGER::MySipServCatalogManage;

namespace MY_APP {

/**
 * @brief SIP目录设备线程参数
 */
typedef struct {
    //                              消息sn号
    std::string                     sn;

    //                              上级sip服务注册配置
    MySipRegUpServCfg_dt            regUpServCfg;

    //                              本级服务地址
    MySipServAddrCfg_dt             servAddrCfg;
    
} MySipCatalogThdParam_dt;

typedef MySipCatalogThdParam_dt*    MySipCatalogThdParamPtr;

int MySipCatalogApp::SipCatalogAppThdFunc(MY_COMMON::MySipEvThdCbParamPtr arg)
{
    MySipCatalogThdParamPtr     thdParamPtr  = (MySipCatalogThdParamPtr)arg;
    const std::string&          sn           = thdParamPtr->sn;
    const MySipRegUpServCfg_dt& regUpServCfg = thdParamPtr->regUpServCfg;
    const MySipServAddrCfg_dt&  servAddrCfg  = thdParamPtr->servAddrCfg;

    // 获取endpoint
    MySipEndptPtr endptPtr = MySystemPjsip::GetPjsipEndptPtr();
    if (nullptr == endptPtr) {
        LOG(ERROR) << "Sip catalog app push local server catalog failed. get pjsip endpoint failed. " 
                   << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
        return -1;
    }

    // 获取设备目录配置
    MySipCatalogPlatCfgMap    platCfgMap    = MySipServCatalogManage::GetSipServCatalogPlatCfgMap(servAddrCfg.id);
    MySipCatalogSubPlatCfgMap subPlatCfgMap = MySipServCatalogManage::GetSipServCatalogSubPlatCfgMap(servAddrCfg.id);
    auto subPlatCfgMapIter = subPlatCfgMap.begin();
    while (subPlatCfgMapIter != subPlatCfgMap.end()) {
        // 删除无效子平台
        if (platCfgMap.end() == platCfgMap.find(subPlatCfgMapIter->second.parentID)) {
            subPlatCfgMapIter = subPlatCfgMap.erase(subPlatCfgMapIter);
        }
        else {
            ++subPlatCfgMapIter;
        }
    }

    MySipCatalogSubVirtualPlatCfgMap subVirtualCfgMap = MySipServCatalogManage::GetSipServCatalogSubVirtualPlatCfgMap(servAddrCfg.id);
    auto subVirtualCfgMapIter = subVirtualCfgMap.begin();
    while (subVirtualCfgMapIter != subVirtualCfgMap.end()) {
        // 删除无效虚拟子平台
        if (platCfgMap.end() == platCfgMap.find(subVirtualCfgMapIter->second.parentID) &&
            subPlatCfgMap.end() == subPlatCfgMap.find(subVirtualCfgMapIter->second.parentID)) {
            subVirtualCfgMapIter = subVirtualCfgMap.erase(subVirtualCfgMapIter);
        }
        else {
            ++subVirtualCfgMapIter;
        }
    }

    MySipCatalogDeviceCfgMap deviceCfgMap = MySipServCatalogManage::GetSipServCatalogDeviceCfgMap(servAddrCfg.id);
    auto deviceCfgMapIter = deviceCfgMap.begin();
    while (deviceCfgMapIter != deviceCfgMap.end()) {
        // 删除无效设备
        if (platCfgMap.end() == platCfgMap.find(deviceCfgMapIter->second.parentID) &&
            subPlatCfgMap.end() == subPlatCfgMap.find(deviceCfgMapIter->second.parentID) &&
            subVirtualCfgMap.end() == subVirtualCfgMap.find(deviceCfgMapIter->second.parentID)) {
                deviceCfgMapIter = deviceCfgMap.erase(deviceCfgMapIter);
        }
        else {
            ++deviceCfgMapIter;
        }
    }

    // 获取设备目录总数
    std::size_t sumNum     = platCfgMap.size() + subPlatCfgMap.size() + subVirtualCfgMap.size() + deviceCfgMap.size();

    // sip catalog消息首部生成
    std::string sURL       = MySipMsgHelper::GenerateSipMsgURL(regUpServCfg.upSipServAddrCfg.id, regUpServCfg.upSipServAddrCfg.ipAddr, 
                                                               regUpServCfg.upSipServAddrCfg.port, regUpServCfg.proto);
    std::string sFromHdr   = MySipMsgHelper::GenerateSipMsgFromHeader(servAddrCfg.id, servAddrCfg.ipAddr);
    std::string sToHdr     = MySipMsgHelper::GenerateSipMsgToHeader(servAddrCfg.id, servAddrCfg.ipAddr);

    pj_str_t sipMsgURL     = pj_str(const_cast<char*>(sURL.c_str()));
    pj_str_t sipMsgFromHdr = pj_str(const_cast<char*>(sFromHdr.c_str()));
    pj_str_t sipMsgToHdr   = pj_str(const_cast<char*>(sToHdr.c_str()));

    pjsip_method sipMethod = {PJSIP_OTHER_METHOD, {const_cast<char*>("MESSAGE"), (pj_ssize_t)strlen("MESSAGE")}};

    // 推送本级平台
    for (const auto& platCfgPair : platCfgMap) {
        // sip catalog消息创建
        MySipTxDataPtr txDataPtr = nullptr;
        if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, nullptr, nullptr, -1, nullptr, &txDataPtr)) {
            LOG(ERROR) << "Sip catalog app push local server catalog platform config failed. create pjsip request failed. "
                       << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
            continue;
        }

        // sip catalog消息内容填充
        std::string msgBody = MyXmlHelper::GenerateSipCatalogPlatCfgMsgBody(platCfgPair.second, sn, sumNum);

        pj_str_t type    = pj_str(const_cast<char*>("Application"));
        pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
        pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

        txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

        // sip catalog消息发送
        if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, nullptr, nullptr)) {
            LOG(ERROR) << "Sip catalog app push local server catalog platform config failed. send pjsip request failed. "
                       << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
            continue;
        }
    }

    // 推送本级子平台
    for (const auto& subPlatCfgPair : subPlatCfgMap) {
        // sip catalog消息创建
        MySipTxDataPtr txDataPtr = nullptr;
        if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, nullptr, nullptr, -1, nullptr, &txDataPtr)) {
            LOG(ERROR) << "Sip catalog app push local server catalog subPlatform config failed. create pjsip request failed. "
                       << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
            continue;
        }

        // sip catalog消息内容填充
        std::string msgBody = MyXmlHelper::GenerateSipCatalogSubPlatCfgMsgBody(subPlatCfgPair.second, sn, sumNum);

        pj_str_t type    = pj_str(const_cast<char*>("Application"));
        pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
        pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

        txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

        // sip catalog消息发送
        if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, nullptr, nullptr)) {
            LOG(ERROR) << "Sip catalog app push local server catalog subPlatform config failed. send pjsip request failed. "
                       << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
            continue;
        }
    }

    // 推送本级虚拟子平台
    for (const auto& subVirtualPlatCfgPair : subVirtualCfgMap) {
        // sip catalog消息创建
        MySipTxDataPtr txDataPtr = nullptr;
        if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, nullptr, nullptr, -1, nullptr, &txDataPtr)) {
            LOG(ERROR) << "Sip catalog app push local server catalog subVirtualPlatform config failed. create pjsip request failed. "
                       << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
            continue;
        }

        // sip catalog消息内容填充
        std::string msgBody = MyXmlHelper::GenerateSipCatalogSubVirtualPlatCfgMsgBody(subVirtualPlatCfgPair.second, sn, sumNum);

        pj_str_t type    = pj_str(const_cast<char*>("Application"));
        pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
        pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

        txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

        // sip catalog消息发送
        if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, nullptr, nullptr)) {
            LOG(ERROR) << "Sip catalog app push local server catalog subVirtualPlatform config failed. send pjsip request failed. "
                       << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
            continue;
        }
    }

    // 推送本级设备
    for (const auto& deviceCfgPair : deviceCfgMap) {
        // sip catalog消息创建
        MySipTxDataPtr txDataPtr = nullptr;
        if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, nullptr, nullptr, -1, nullptr, &txDataPtr)) {
            LOG(ERROR) << "Sip catalog app push local server catalog device config failed. create pjsip request failed. "
                       << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
            continue;
        }

        // sip catalog消息内容填充
        std::string msgBody = MyXmlHelper::GenerateSipCatalogDeviceCfgMsgBody(deviceCfgPair.second, sn, sumNum);

        pj_str_t type    = pj_str(const_cast<char*>("Application"));
        pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
        pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

        txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

        // sip catalog消息发送
        if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, nullptr, nullptr)) {
            LOG(ERROR) << "Sip catalog app push local server catalog device config failed. send pjsip request failed. "
                       << MySipServerHelper::GetSipUpRegServInfo(regUpServCfg) << ".";
            continue;
        }
    }

    // new in function: MySipCatalogApp::onRecvSipCatalogReqMsg()
    delete thdParamPtr;
    return 0;
}

MySipCatalogApp::MySipCatalogApp() : m_status(MyStatus_t::FAILED)
{
}

MySipCatalogApp::~MySipCatalogApp()
{
    if (MyStatus_t::SUCCESS == m_status.load()) {
        this->shutdown();
    }
}

MyStatus_t MySipCatalogApp::init(const std::string& servId, const std::string& id, const std::string& name, pjsip_module_priority priority)
{
    // 状态检查
    if (MyStatus_t::SUCCESS == m_status.load()) {
        LOG(WARNING) << "Sip catalog app module init failed. MySipCatalogApp is init already.";
        return MyStatus_t::SUCCESS;
    }

    // 获取endpoint
    MySipEndptPtr endptPtr = MySystemPjsip::GetPjsipEndptPtr();
    if (nullptr == endptPtr) {
        LOG(ERROR) << "Sip catalog app module init failed. invalid endpoint.";
        return MyStatus_t::FAILED;
    }

    // 服务id赋值
    m_servId = servId;

    // app标识初始化 
    m_appIdCfg.id       = id;
    m_appIdCfg.name     = name;   
    m_appIdCfg.priority = priority;

    LOG(INFO) << "Sip catalog app module init success. " << MySipAppHelper::GetSipAppInfo(m_appIdCfg) << ".";

    m_status.store(MyStatus_t::SUCCESS);
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipCatalogApp::run()
{
    // 定时器初启动
    if (nullptr == m_timePtr) {
        MySipCatalogApp::SmtWkPtr weakSelf = this->getSipCatalogApp();
        m_timePtr = std::make_shared<Timer>(60 * 5, [weakSelf]() -> bool {
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
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipCatalogApp::shutdown()
{
    // 状态检查
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "Sip catalog app module shutdown failed. MySipCatalogApp is not init.";
        return MyStatus_t::SUCCESS;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipCatalogApp::onRecvSipCatalogReqMsg(MySipRxDataPtr rxDataPtr)
{
    MySipCatalogReqMsgBody_dt catalogMsgBody;
    if (MyStatus_t::SUCCESS != MyXmlHelper::ParseSipCatalogReqMsgBody(static_cast<char*>(rxDataPtr->msg_info.msg->body->data), catalogMsgBody)) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. parse catalog msg failed.";
        return MyStatus_t::FAILED;
    }

    // 服务id校验
    if (catalogMsgBody.deviceId != m_servId) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. invalid device id. " << catalogMsgBody.deviceId << ".";
        return MyStatus_t::FAILED;
    }

    // 解析上级服务id
    char fromHdrBuf[256] = {0};
    pjsip_uri_print(PJSIP_URI_IN_REQ_URI, rxDataPtr->msg_info.from->uri, fromHdrBuf, sizeof(fromHdrBuf));

    std::string upRegServId;
    std::string upRegServIpAddr;
    if (MyStatus_t::SUCCESS != MySipMsgHelper::ParseSipMsgFromHdr(fromHdrBuf, upRegServId, upRegServIpAddr)) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. parse from header failed.";
        return MyStatus_t::FAILED;
    }

    auto upServRegCfg = MySipServRegManage::GetSipRegUpServCfg(m_servId, upRegServId);
    if (upServRegCfg.upSipServAddrCfg.id.empty()) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. invalid up reg serv id. " << upRegServId << ".";
        return MyStatus_t::FAILED;
    }

    // 获取本地服务配置
    auto localServCfg = MyServManage::GetSipServAddrCfg(m_servId);
    if (localServCfg.id.empty()) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. invalid local serv id. " << m_servId << ".";
        return MyStatus_t::FAILED;
    }

    // 更新设备目录信息
    if (MyStatus_t::SUCCESS == MySipServCatalogManage::HasSipServCatalogInfo(m_servId)) {
        // 更新SN号
        MySipServCatalogManage::UpdateSipServCatalogSN(m_servId, catalogMsgBody.sn);
    }
    else {
        // 获取设备目录配置
        MySipCatalogPlatCfgMap           platCfgMap       = MySystemConfig::GetSipCatalogPlatCfgMap(m_servId);
        MySipCatalogSubPlatCfgMap        subPlatCfgMap    = MySystemConfig::GetSipCatalogSubPlatCfgMap(m_servId);
        MySipCatalogSubVirtualPlatCfgMap subVirtualCfgMap = MySystemConfig::GetSipCatalogSubVirtualPlatCfgMap(m_servId);
        MySipCatalogDeviceCfgMap         deviceCfgMap     = MySystemConfig::GetSipCatalogDeviceCfgMap(m_servId);

        MySipCatalogInfo_dt catalogInfo;

        // 添加设备目录sn号
        catalogInfo.sn = catalogMsgBody.sn;

        // 添加设备目录平台信息
        for (const auto& platCfgPair : platCfgMap) {
            // 确保平台id与服务id一致
            if (platCfgPair.first != m_servId) {
                continue;
            }

            // 确保平台id未重复
            if (catalogInfo.sipPlatMap.end() != catalogInfo.sipPlatMap.find(platCfgPair.first)) {
                continue;
            }

            // 插入设备目录平台配置
            catalogInfo.sipPlatMap.insert(std::make_pair(platCfgPair.first, platCfgPair.second));
        }

        // 添加设备目录子平台信息
        const auto& sipPlatMap = catalogInfo.sipPlatMap;
        for (const auto& subPlatCfgPair : subPlatCfgMap) {
            // 确保子平台id关联的平台id存在
            if (sipPlatMap.end() == sipPlatMap.find(subPlatCfgPair.second.parentID)) {
                continue;
            }

            // 确保子平台id未重复
            if (catalogInfo.sipSubPlatMap.end() != catalogInfo.sipSubPlatMap.find(subPlatCfgPair.first)) {
                continue;
            }

            // 插入设备目录子平台配置
            catalogInfo.sipSubPlatMap.insert(std::make_pair(subPlatCfgPair.first, subPlatCfgPair.second));
        }

        // 添加设备目录虚拟子平台信息
        const auto& sipSubPlatMap = catalogInfo.sipSubPlatMap;
        for (const auto& subVirtualCfgPair : subVirtualCfgMap) {
            // 确保虚拟子平台id关联的平台/子平台id存在
            if (sipSubPlatMap.end() == sipSubPlatMap.find(subVirtualCfgPair.second.parentID) &&
                sipPlatMap.end() == sipPlatMap.find(subVirtualCfgPair.second.parentID)) {
                continue;
            }

            // 确保虚拟子平台id未重复
            if (catalogInfo.sipSubVirtualPlatMap.end() != catalogInfo.sipSubVirtualPlatMap.find(subVirtualCfgPair.first)) {
                continue;
            }

            // 插入设备目录虚拟子平台配置
            catalogInfo.sipSubVirtualPlatMap.insert(std::make_pair(subVirtualCfgPair.first, subVirtualCfgPair.second));
        }

        // 添加设备目录设备信息
        const auto& sipSubVirtualPlatMap = catalogInfo.sipSubVirtualPlatMap;
        for (const auto& deviceCfgPair : deviceCfgMap) {
            // 确保设备id关联的虚拟子平台/子平台/平台id存在
            if (sipSubVirtualPlatMap.end() == sipSubVirtualPlatMap.find(deviceCfgPair.second.parentID) &&
                sipSubPlatMap.end() == sipSubPlatMap.find(deviceCfgPair.second.parentID) &&
                sipPlatMap.end() == sipPlatMap.find(deviceCfgPair.second.parentID)) {
                continue;
            }

            // 确保设备id未重复
            if (catalogInfo.sipDeviceMap.end() != catalogInfo.sipDeviceMap.find(deviceCfgPair.first)) {
                continue;
            }

            // 插入设备目录设备配置
            catalogInfo.sipDeviceMap.insert(std::make_pair(deviceCfgPair.first, deviceCfgPair.second));
        }

        // 添加设备目录服务信息
        MySipServCatalogManage::AddSipServCatalogInfo(m_servId, catalogInfo);
    }

    // 获取endpoint
    MySipEndptPtr endptPtr = MySystemPjsip::GetPjsipEndptPtr();

    // 发送sip应答消息
    if (PJ_SUCCESS != pjsip_endpt_respond(endptPtr, nullptr, rxDataPtr, 200, nullptr, nullptr, nullptr, nullptr)) {
        LOG(ERROR) << "Sip catalog app receive catalog message failed. can't send response message. ";
        return MyStatus_t::FAILED;
    }

    // 创建发送sip设备目录消息线程参数，delete in function: MySipCatalogApp::SipCatalogAppThdFunc()
    MySipCatalogThdParamPtr thdParamPtr = new MySipCatalogThdParam_dt();
    thdParamPtr->sn                     = catalogMsgBody.sn;
    thdParamPtr->regUpServCfg           = upServRegCfg;
    thdParamPtr->servAddrCfg            = localServCfg;

    // 创建发送sip设备目录消息线程
    auto servThdPoolPtr = MyServManage::GetSipServThdPoolPtr(m_servId);
    if (PJ_SUCCESS != pj_thread_create(servThdPoolPtr, nullptr, &MySipCatalogApp::SipCatalogAppThdFunc, thdParamPtr, 0, 0, nullptr)) {
        LOG(ERROR) << "Sip catalog app receive catalog message failed. can't create send catalog message thread. ";
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

bool MySipCatalogApp::onTimer()
{
    if (MyStatus_t::SUCCESS != m_status.load()) {
        return false;
    }

    // 获取本地服务配置
    auto localServCfg = MyServManage::GetSipServAddrCfg(m_servId);
    if (localServCfg.id.empty()) {
        return false;
    }

    // 获取下级sip服务配置
    MySipRegLowServCfgMap regLowServMap = MySipServRegManage::GetSipRegLowServCfgMap(m_servId);
    for (const auto& pair : regLowServMap) {
        this->reqLowServCatalog(pair.second, localServCfg);
    }
    return true;
}

MyStatus_t MySipCatalogApp::reqLowServCatalog(const MySipRegLowServCfg_dt& regLowServCfg, const MySipServAddrCfg_dt& localServCfg)
{
    // 获取endpoint
    MySipEndptPtr endptPtr = MySystemPjsip::GetPjsipEndptPtr();
    if (nullptr == endptPtr) {
        LOG(ERROR) << "Sip catalog app request low server catalog failed. get pjsip endpoint failed. " 
                   << MySipServerHelper::GetSipLowRegServInfo(regLowServCfg) << ".";
        return MyStatus_t::FAILED;
    }

    // sip catalog消息首部生成
    std::string sURL     = MySipMsgHelper::GenerateSipMsgURL(regLowServCfg.lowSipServAddrCfg.id, regLowServCfg.lowSipServAddrCfg.ipAddr, 
                                                             regLowServCfg.lowSipServAddrCfg.port, regLowServCfg.proto);
    std::string sFromHdr = MySipMsgHelper::GenerateSipMsgFromHeader(localServCfg.id, localServCfg.ipAddr);
    std::string sToHdr   = MySipMsgHelper::GenerateSipMsgToHeader(localServCfg.id, localServCfg.ipAddr);

    pj_str_t sipMsgURL     = pj_str(const_cast<char*>(sURL.c_str()));
    pj_str_t sipMsgFromHdr = pj_str(const_cast<char*>(sFromHdr.c_str()));
    pj_str_t sipMsgToHdr   = pj_str(const_cast<char*>(sToHdr.c_str()));

    pjsip_method sipMethod = {PJSIP_OTHER_METHOD, {const_cast<char*>("MESSAGE"), (pj_ssize_t)strlen("MESSAGE")}};

    // sip catalog消息创建
    MySipTxDataPtr txDataPtr = nullptr;
    if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, nullptr, nullptr, -1, nullptr, &txDataPtr)) {
        LOG(ERROR) << "Sip catalog app request low server catalog failed. create pjsip request failed. "
                   << MySipServerHelper::GetSipLowRegServInfo(regLowServCfg) << ".";
        return MyStatus_t::FAILED;
    }

    // sip catalog消息内容填充
    std::string msgBody = MyXmlHelper::GenerateSipCatalogReqMsgBody(regLowServCfg.lowSipServAddrCfg.id);

    pj_str_t type    = pj_str(const_cast<char*>("Application"));
    pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
    pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

    txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

    // sip catalog消息发送
    if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, nullptr, nullptr)) {
        LOG(ERROR) << "Sip catalog app request low server catalog failed. send pjsip request failed. "
                   << MySipServerHelper::GetSipLowRegServInfo(regLowServCfg) << ".";
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_APP