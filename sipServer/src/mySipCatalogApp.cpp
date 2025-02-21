#include <cstring>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySystemConfg.h"
#include "envir/mySystemPjsip.h"
#include "envir/mySystemThreadPool.h"
#include "utils/myXmlHelper.h"
#include "utils/mySipAppHelper.h"
#include "utils/mySipMsgHelper.h"
#include "utils/mySipServerHelper.h"
#include "manager/myServManage.h"
#include "manager/mySipServRegManage.h"
#include "manager/mySipServCatalogManager.h"
#include "app/mySipCatalogApp.h"
using namespace MY_UTILS;
using namespace MY_COMMON;
using MY_ENVIR::MySystemPjsip;
using MY_ENVIR::MySystemConfig;
using MY_ENVIR::MySystemThdPool;
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
    // pjsip要求自定义线程进行注册才能使用
    pj_thread_desc desc;
    if(!pj_thread_is_registered()) {
        MySipThdPtr thdPtr = nullptr;
        pj_thread_register(nullptr, desc, &thdPtr);
    }

    MySipCatalogThdParamPtr thdParamPtr = (MySipCatalogThdParamPtr)arg;
    if (nullptr == thdParamPtr) {
        LOG(ERROR) << "Sip catalog app push local server catalog failed. thd param is null.";
        return -1;
    }

    // 内存管理, new in function: MySipCatalogApp::onSipCatalogAppRecvSipCatalogQueryReqMsg()
    std::unique_ptr<MySipCatalogThdParam_dt> memManagePtr(thdParamPtr);

    const std::string&          sn           = thdParamPtr->sn;
    const MySipRegUpServCfg_dt& regUpServCfg = thdParamPtr->regUpServCfg;
    const MySipServAddrCfg_dt&  servAddrCfg  = thdParamPtr->servAddrCfg;

    std::string upRegServInfo;
    MySipServerHelper::GetSipUpRegServInfo(regUpServCfg, upRegServInfo);

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip catalog app push local server catalog failed. get pjsip endpoint failed. " << upRegServInfo << ".";
        return -1;
    }

    // 获取设备目录配置
    MySipCatalogPlatCfgMap platCfgMap;
    if (MyStatus_t::SUCCESS != MySipServCatalogManage::GetSipServCatalogPlatCfgMap(servAddrCfg.id, platCfgMap)) {
        LOG(ERROR) << "Sip catalog app push local server catalog failed. get catalog plat cfg failed. "  << upRegServInfo << ".";
        return -1;
    }

    MySipCatalogSubPlatCfgMap subPlatCfgMap;
    if (MyStatus_t::SUCCESS != MySipServCatalogManage::GetSipServCatalogSubPlatCfgMap(servAddrCfg.id, subPlatCfgMap)) {
        LOG(ERROR) << "Sip catalog app push local server catalog failed. get catalog sub plat cfg failed. " << upRegServInfo << ".";
        return -1;
    }

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

    MySipCatalogSubVirtualPlatCfgMap subVirtualCfgMap;
    if (MyStatus_t::SUCCESS != MySipServCatalogManage::GetSipServCatalogSubVirtualPlatCfgMap(servAddrCfg.id, subVirtualCfgMap)) {
        LOG(ERROR) << "Sip catalog app push local server catalog failed. get catalog sub virtual plat cfg failed. " << upRegServInfo << ".";
        return -1;
    }

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

    MySipCatalogDeviceCfgMap deviceCfgMap;
    if (MyStatus_t::SUCCESS != MySipServCatalogManage::GetSipServCatalogDeviceCfgMap(servAddrCfg.id, deviceCfgMap)) {
        LOG(ERROR) << "Sip catalog app push local server catalog failed. get catalog device cfg failed. " << upRegServInfo << ".";
        return -1;
    }

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
    std::size_t sumNum = platCfgMap.size() + subPlatCfgMap.size() + subVirtualCfgMap.size() + deviceCfgMap.size();

    // sip catalog消息首部生成
    std::string sURL;
    MySipMsgHelper::GenerateSipMsgURL(regUpServCfg.upSipServAddrCfg.id, regUpServCfg.upSipServAddrCfg.ipAddr, 
                                      regUpServCfg.upSipServAddrCfg.port, regUpServCfg.proto, sURL);

    std::string sFromHdr;
    MySipMsgHelper::GenerateSipMsgFromHeader(servAddrCfg.id, servAddrCfg.ipAddr, sFromHdr);

    std::string sToHdr;
    MySipMsgHelper::GenerateSipMsgToHeader(servAddrCfg.id, servAddrCfg.ipAddr, sToHdr);

    pj_str_t sipMsgURL     = pj_str(const_cast<char*>(sURL.c_str()));
    pj_str_t sipMsgFromHdr = pj_str(const_cast<char*>(sFromHdr.c_str()));
    pj_str_t sipMsgToHdr   = pj_str(const_cast<char*>(sToHdr.c_str()));

    pjsip_method sipMethod = {PJSIP_OTHER_METHOD, {const_cast<char*>("MESSAGE"), (pj_ssize_t)strlen("MESSAGE")}};

    // 推送本级平台
    for (const auto& platCfgPair : platCfgMap) {
        // sip catalog消息创建
        MySipTxDataPtr txDataPtr = nullptr;
        if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, nullptr, nullptr, -1, nullptr, &txDataPtr)) {
            LOG(ERROR) << "Sip catalog app push local server catalog platform config failed. create pjsip request failed. " << upRegServInfo << ".";
            continue;
        }

        // sip catalog消息内容填充
        std::string msgBody;
        MyXmlHelper::GenerateSipCatalogPlatCfgMsgBody(platCfgPair.second, sn, sumNum, msgBody);

        pj_str_t type    = pj_str(const_cast<char*>("Application"));
        pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
        pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

        txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

        // sip catalog消息发送
        if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, nullptr, nullptr)) {
            LOG(ERROR) << "Sip catalog app push local server catalog platform config failed. send pjsip request failed. " << upRegServInfo << ".";
            continue;
        }
    }

    // 推送本级子平台
    for (const auto& subPlatCfgPair : subPlatCfgMap) {
        // sip catalog消息创建
        MySipTxDataPtr txDataPtr = nullptr;
        if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, nullptr, nullptr, -1, nullptr, &txDataPtr)) {
            LOG(ERROR) << "Sip catalog app push local server catalog subPlatform config failed. create pjsip request failed. " << upRegServInfo << ".";
            continue;
        }

        // sip catalog消息内容填充
        std::string msgBody;
        MyXmlHelper::GenerateSipCatalogSubPlatCfgMsgBody(subPlatCfgPair.second, sn, sumNum, msgBody);

        pj_str_t type    = pj_str(const_cast<char*>("Application"));
        pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
        pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

        txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

        // sip catalog消息发送
        if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, nullptr, nullptr)) {
            LOG(ERROR) << "Sip catalog app push local server catalog subPlatform config failed. send pjsip request failed. " << upRegServInfo << ".";
            continue;
        }
    }

    // 推送本级虚拟子平台
    for (const auto& subVirtualPlatCfgPair : subVirtualCfgMap) {
        // sip catalog消息创建
        MySipTxDataPtr txDataPtr = nullptr;
        if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, nullptr, nullptr, -1, nullptr, &txDataPtr)) {
            LOG(ERROR) << "Sip catalog app push local server catalog subVirtualPlatform config failed. create pjsip request failed. " << upRegServInfo << ".";
            continue;
        }

        // sip catalog消息内容填充
        std::string msgBody;
        MyXmlHelper::GenerateSipCatalogSubVirtualPlatCfgMsgBody(subVirtualPlatCfgPair.second, sn, sumNum, msgBody);

        pj_str_t type    = pj_str(const_cast<char*>("Application"));
        pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
        pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

        txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

        // sip catalog消息发送
        if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, nullptr, nullptr)) {
            LOG(ERROR) << "Sip catalog app push local server catalog subVirtualPlatform config failed. send pjsip request failed. " << upRegServInfo << ".";
            continue;
        }
    }

    // 推送本级设备
    for (const auto& deviceCfgPair : deviceCfgMap) {
        // sip catalog消息创建
        MySipTxDataPtr txDataPtr = nullptr;
        if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, nullptr, nullptr, -1, nullptr, &txDataPtr)) {
            LOG(ERROR) << "Sip catalog app push local server catalog device config failed. create pjsip request failed. " << upRegServInfo << ".";
            continue;
        }

        // sip catalog消息内容填充
        std::string msgBody;
        MyXmlHelper::GenerateSipCatalogDeviceCfgMsgBody(deviceCfgPair.second, sn, sumNum, msgBody);

        pj_str_t type    = pj_str(const_cast<char*>("Application"));
        pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
        pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

        txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

        // sip catalog消息发送
        if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, nullptr, nullptr)) {
            LOG(ERROR) << "Sip catalog app push local server catalog device config failed. send pjsip request failed. " << upRegServInfo << ".";
            continue;
        }
    }
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
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip catalog app module init failed. invalid endpoint.";
        return MyStatus_t::FAILED;
    }

    // 服务id赋值
    m_servId = servId;

    // app标识初始化 
    m_appIdCfg.id       = id;
    m_appIdCfg.name     = name;   
    m_appIdCfg.priority = priority;

    // 创建设备目录信息
    MySipCatalogInfo_dt catalogInfo;

    // 添加设备目录平台信息
    MySipCatalogPlatCfgMap platCfgMap;
    MySystemConfig::GetSipCatalogPlatCfgMap(m_servId, platCfgMap);

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

    const auto& sipPlatMap = catalogInfo.sipPlatMap;

    // 添加设备目录子平台信息
    MySipCatalogSubPlatCfgMap subPlatCfgMap;
    MySystemConfig::GetSipCatalogSubPlatCfgMap(m_servId, subPlatCfgMap);
    
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

    const auto& sipSubPlatMap = catalogInfo.sipSubPlatMap;

    // 添加设备目录虚拟子平台信息
    MySipCatalogSubVirtualPlatCfgMap subVirtualCfgMap;
    MySystemConfig::GetSipCatalogSubVirtualPlatCfgMap(m_servId, subVirtualCfgMap);

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

    const auto& sipSubVirtualPlatMap = catalogInfo.sipSubVirtualPlatMap;

    // 添加设备目录设备信息
    MySipCatalogDeviceCfgMap deviceCfgMap;
    MySystemConfig::GetSipCatalogDeviceCfgMap(m_servId, deviceCfgMap);

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

    // 添加设备目录信息
    MySipServCatalogManage::AddSipServCatalogInfo(m_servId, catalogInfo);

    std::string sipAppInfo;
    MySipAppHelper::GetSipAppInfo(m_appIdCfg, sipAppInfo);
    LOG(INFO) << "Sip catalog app module init success. " << sipAppInfo << ".";

    m_status.store(MyStatus_t::SUCCESS);
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipCatalogApp::run()
{
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

MyStatus_t MySipCatalogApp::onSipCatalogAppRecvSipCatalogQueryReqMsg(MySipRxDataPtr rxDataPtr)
{
    MySipCatalogReqMsgBody_dt catalogMsgBody;
    if (MyStatus_t::SUCCESS != MyXmlHelper::ParseSipCatalogQueryReqMsgBody(static_cast<char*>(rxDataPtr->msg_info.msg->body->data), catalogMsgBody)) {
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

    MySipRegUpServCfg_dt upServRegCfg;
    if (MyStatus_t::SUCCESS != MySipServRegManage::GetSipRegUpServCfg(m_servId, upRegServId, upServRegCfg)) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. get up reg serv cfg failed. " << upRegServId << ".";
        return MyStatus_t::FAILED;
    }

    if (upServRegCfg.upSipServAddrCfg.id.empty()) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. invalid up reg serv id. " << upRegServId << ".";
        return MyStatus_t::FAILED;
    }

    // 获取本地服务配置
    MySipServAddrCfg_dt localServCfg;
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServAddrCfg(m_servId, localServCfg)) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. get local serv cfg failed. " << m_servId << ".";
        return MyStatus_t::FAILED;
    }
    
    if (localServCfg.id.empty()) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. invalid local serv id. " << m_servId << ".";
        return MyStatus_t::FAILED;
    }

    // 设备目录信息不存在
    if (MyStatus_t::SUCCESS != MySipServCatalogManage::HasSipServCatalogInfo(m_servId)) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. invalid catalog info. local serv id. " << m_servId << ".";
        return MyStatus_t::FAILED;
    }

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. get pjsip endpt ptr failed. ";
        return MyStatus_t::FAILED;
    }

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

    // 更新SN号
    MySipServCatalogManage::UpdateSipServCatalogSN(m_servId, catalogMsgBody.sn);

    // 添加上级服务信息
    MySipServCatalogManage::AddSipUpQueryServInfo(m_servId, upServRegCfg.upSipServAddrCfg);

    // 发送sip设备目录消息
    MySystemThdPool::ThreadPoolTaskFunc thdPoolFunc = [thdParamPtr]() {
        MySipCatalogApp::SipCatalogAppThdFunc(thdParamPtr);
    };
    
    MySystemThdPool::ExecuteTask(thdPoolFunc);

    // 消息处理成功
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipCatalogApp::onSipCatalogAppRecvSipCatalogResponseReqMsg(MySipRxDataPtr rxDataPtr)
{
    // 获取消息内容
    std::string msgBodyStr = static_cast<const char*>(rxDataPtr->msg_info.msg->body->data);


    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipCatalogApp::getState(MyStatus_t& status) const
{
    status = m_status.load();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipCatalogApp::getId(std::string& id) const
{
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(ERROR) << "Sip app module get id failed. MySipCatalogApp is not init.";
        return MyStatus_t::FAILED;
    }

    id = m_appIdCfg.id;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipCatalogApp::getSipCatalogApp(MySipCatalogApp::SmtWkPtr& wkPtr)
{
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "Sip app module get work pointer failed. MySipCatalogApp is not init.";
        return MyStatus_t::FAILED;
    }

    wkPtr = this->shared_from_this();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipCatalogApp::onSipCatalogAppReqLowServCatalog(const MySipRegLowServCfg_dt& regLowServCfg, const MySipServAddrCfg_dt& localServCfg)
{
    std::string lowRegServInfo;
    MySipServerHelper::GetSipLowRegServInfo(regLowServCfg, lowRegServInfo);

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip catalog app request low server catalog failed. get pjsip endpoint failed. " << lowRegServInfo << ".";
        return MyStatus_t::FAILED;
    }

    // sip catalog消息首部生成
    std::string sURL;
    MySipMsgHelper::GenerateSipMsgURL(regLowServCfg.lowSipServAddrCfg.id, regLowServCfg.lowSipServAddrCfg.ipAddr, 
                                      regLowServCfg.lowSipServAddrCfg.port, regLowServCfg.proto, sURL);

    std::string sFromHdr;
    MySipMsgHelper::GenerateSipMsgFromHeader(localServCfg.id, localServCfg.ipAddr, sFromHdr);

    std::string sToHdr;
    MySipMsgHelper::GenerateSipMsgToHeader(localServCfg.id, localServCfg.ipAddr, sToHdr);

    pj_str_t sipMsgURL     = pj_str(const_cast<char*>(sURL.c_str()));
    pj_str_t sipMsgFromHdr = pj_str(const_cast<char*>(sFromHdr.c_str()));
    pj_str_t sipMsgToHdr   = pj_str(const_cast<char*>(sToHdr.c_str()));

    pjsip_method sipMethod = {PJSIP_OTHER_METHOD, {const_cast<char*>("MESSAGE"), (pj_ssize_t)strlen("MESSAGE")}};

    // sip catalog消息创建
    MySipTxDataPtr txDataPtr = nullptr;
    if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, nullptr, nullptr, -1, nullptr, &txDataPtr)) {
        LOG(ERROR) << "Sip catalog app request low server catalog failed. create pjsip request failed. " << lowRegServInfo << ".";
        return MyStatus_t::FAILED;
    }

    // sip catalog消息内容填充
    std::string msgBody;
    MyXmlHelper::GenerateSipCatalogQueryReqMsgBody(regLowServCfg.lowSipServAddrCfg.id, msgBody);

    pj_str_t type    = pj_str(const_cast<char*>("Application"));
    pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
    pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

    txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

    // sip catalog消息发送
    if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, nullptr, nullptr)) {
        LOG(ERROR) << "Sip catalog app request low server catalog failed. send pjsip request failed. " << lowRegServInfo << ".";
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_APP