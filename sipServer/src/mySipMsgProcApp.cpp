#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "common/mySipMsgDef.h"
#include "envir/mySystemPjsip.h"
#include "manager/myAppManage.h"
#include "manager/myServManage.h"
#include "manager/mySipRegManage.h"
#include "utils/myXmlHelper.h"
#include "utils/myStrHelper.h"
#include "utils/mySipAppHelper.h"
#include "utils/mySipMsgHelper.h"
#include "app/mySipMsgProcApp.h"
using namespace MY_UTILS;
using namespace MY_COMMON;
using namespace MY_MANAGER;
using MY_ENVIR::MySystemPjsip;

namespace MY_APP {

MySipMsgProcApp::MySipMsgProcApp() : m_status(MyStatus_t::FAILED), m_appModPtr(nullptr)
{

}

MySipMsgProcApp::~MySipMsgProcApp()
{
    if (MyStatus_t::SUCCESS == m_status.load()) {
        this->shutdown();
    }
}

MyStatus_t MySipMsgProcApp::init(const std::string& servId, const std::string& name, pjsip_module_priority priority)
{
    // 状态检查
    if (MyStatus_t::SUCCESS == m_status.load()) {
        LOG(WARNING) << "Sip app module init failed. MySipMsgProcApp is init already.";
        return MyStatus_t::SUCCESS;
    }

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip app module init failed. endpoint is invalid.";
        return MyStatus_t::FAILED;
    }

    // app模块初始化
    m_appModPtr                     = std::make_shared<pjsip_module>();
    m_appModPtr->name.ptr           = const_cast<char*>(name.c_str());
    m_appModPtr->name.slen          = name.length();
    m_appModPtr->id                 = -1;
    m_appModPtr->priority           = priority;
    m_appModPtr->prev               = nullptr;
    m_appModPtr->next               = nullptr;
    m_appModPtr->load               = MySipMsgProcApp::OnAppModuleLoadCb;
    m_appModPtr->unload             = MySipMsgProcApp::OnAppModuleUnLoadCb;
    m_appModPtr->start              = MySipMsgProcApp::OnAppModuleStartCb;
    m_appModPtr->stop               = MySipMsgProcApp::OnAppModuleStopCb;
    m_appModPtr->on_rx_request      = MySipMsgProcApp::OnAppModuleRecvReqCb;
    m_appModPtr->on_rx_response     = MySipMsgProcApp::OnAppModuleRecvRespCb;
    m_appModPtr->on_tx_request      = MySipMsgProcApp::OnAppModuleSendReqCb;
    m_appModPtr->on_tx_response     = MySipMsgProcApp::OnAppModuleSendRespCb;
    m_appModPtr->on_tsx_state       = MySipMsgProcApp::OnAppModuleTsxStateChangeCb;

    // app模块注册
    if(PJ_SUCCESS != pjsip_endpt_register_module(endptPtr, m_appModPtr.get())) {
        m_appModPtr.reset();

        LOG(ERROR) << "Sip app module init failed. register endpoint failed.";
        return MyStatus_t::FAILED;
    }

    // 服务id赋值
    m_servId = servId;

    // app标识初始化 
    m_appIdCfg.name      = name;
    m_appIdCfg.priority  = priority;
    m_appIdCfg.id        = std::to_string(m_appModPtr->id);

    std::string appInfo;
    MySipAppHelper::GetSipAppInfo(m_appIdCfg, appInfo);
    LOG(INFO) << "Sip app module init success. " << appInfo << ".";

    m_status.store(MyStatus_t::SUCCESS);
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipMsgProcApp::run()
{
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipMsgProcApp::shutdown()
{
    // 状态检查
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "Sip app module shutdown failed. MySipMsgProcApp is not init.";
        return MyStatus_t::SUCCESS;
    }

    m_status.store(MyStatus_t::FAILED);

    std::string appInfo;
    MySipAppHelper::GetSipAppInfo(m_appIdCfg, appInfo);
    LOG(INFO) << "Sip app module shutdown success. " << appInfo << ".";

    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS == MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        pjsip_endpt_unregister_module(endptPtr, m_appModPtr.get());
    }
    
    m_appModPtr.reset();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipMsgProcApp::getState(MyStatus_t& status) const
{
    status = m_status.load();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipMsgProcApp::getId(std::string& id) const
{
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "Sip app module get id failed. MySipMsgProcApp is not init.";
        return MyStatus_t::FAILED;
    }

    id = m_appIdCfg.id;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipMsgProcApp::getSipMsgProcApp(MySipMsgProcApp::SmtWkPtr& wkPtr)
{
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "Sip app module get work pointer failed. MySipMsgProcApp is not init.";
        return MyStatus_t::FAILED;
    }

    wkPtr = this->shared_from_this();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipMsgProcApp::onProcSipRegisterReqMsg(MySipRxDataPtr rdataPtr)
{
    // sip regist uri解析
    char buf[256];
    pjsip_uri_print(PJSIP_URI_IN_REQ_URI, rdataPtr->msg_info.msg->line.req.uri, buf, sizeof(buf));
    LOG(INFO) << "Sip app module recv sip register message. uri: " << buf << " message: " << pjsip_rx_data_get_info(rdataPtr);

    MySipMsgUri_dt sipUri;
    if (MyStatus_t::SUCCESS != MySipMsgHelper::ParseSipMsgURL(buf, sipUri)) {
        LOG(ERROR) << "Sip app module recv sip register message failed. parse uri failed. uri: " << buf << ".";
        return MyStatus_t::FAILED;
    }

    if (sipUri.id != m_servId) {
        LOG(ERROR) << "Sip app module recv sip register message failed. invalid service id. uri: " << buf << ".";
        return MyStatus_t::FAILED;
    }

    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServer(sipServWkPtr)) {
        LOG(ERROR) << "Sip app module recv sip register message failed. find sip server failed. uri: " << buf << ".";
        return MyStatus_t::FAILED;
    }

    // 处理sip注册请求
    return sipServWkPtr.lock()->onServRecvSipRegReqMsg(rdataPtr);
}

MyStatus_t MySipMsgProcApp::onProcSipInviteReqMsg(MySipRxDataPtr rdataPtr)
{
    // sip regist uri解析
    char buf[256];
    pjsip_uri_print(PJSIP_URI_IN_REQ_URI, rdataPtr->msg_info.msg->line.req.uri, buf, sizeof(buf));
    LOG(INFO) << "Sip app module recv sip register message. uri: " << buf << " message: " << pjsip_rx_data_get_info(rdataPtr);

    // 解析本级服务id
    char toHdrBuf[256] = {0};
    pjsip_uri_print(PJSIP_URI_IN_REQ_URI, rdataPtr->msg_info.to->uri, toHdrBuf, sizeof(toHdrBuf));

    std::string servId;
    std::string servIpAddr;
    if (MyStatus_t::SUCCESS != MySipMsgHelper::ParseSipMsgToHdr(toHdrBuf, servId, servIpAddr)) {
        LOG(ERROR) << "Sip app module recv sip invite message error. parse to header failed.";
        return MyStatus_t::FAILED;
    }
    else if (m_servId != servId) {
        LOG(ERROR) << "Sip app module recv sip invite message error. invalid service id. uri: " << buf << ".";
        return MyStatus_t::FAILED;
    }

    // 解析上级服务id
    char fromHdrBuf[256] = {0};
    pjsip_uri_print(PJSIP_URI_IN_REQ_URI, rdataPtr->msg_info.from->uri, fromHdrBuf, sizeof(fromHdrBuf));

    std::string remoteServId;
    std::string remoteServIpAddr;
    if (MyStatus_t::SUCCESS != MySipMsgHelper::ParseSipMsgFromHdr(fromHdrBuf, remoteServId, remoteServIpAddr)) {
        LOG(ERROR) << "Sip app module recv sip invite message error. parse from header failed.";
        return MyStatus_t::FAILED;
    }

    if (MyStatus_t::SUCCESS != MySipRegManage::HasSipRegUpServInfo(remoteServId)) {
        LOG(ERROR) << "Sip app module recv sip invite message error. find up service failed. uri: " << buf << ".";
        return MyStatus_t::FAILED;
    }

    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServer(sipServWkPtr)) {
        LOG(ERROR) << "Sip app module recv sip invite message error. find sip server failed. uri: " << buf << ".";
        return MyStatus_t::FAILED;
    }

    // 处理sip invite请求
    return sipServWkPtr.lock()->onServRecvSipInviteReqMsg(rdataPtr);
}

MyStatus_t MySipMsgProcApp::onProcSipKeepAliveReqMsg(MySipRxDataPtr rdataPtr)
{
    MySipKeepAliveMsgBody_dt keepAliveMsgBody;
    if (MyStatus_t::SUCCESS != MyXmlHelper::ParseSipKeepAliveMsgBody(static_cast<char*>(rdataPtr->msg_info.msg->body->data), 
        keepAliveMsgBody)) {
        LOG(ERROR) << "Sip app module recv keepalive message failed. parse message failed.";
        return MyStatus_t::FAILED;
    }

    std::string msgInfo;
    MySipMsgHelper::PrintSipKeepAliveMsgBody(keepAliveMsgBody, msgInfo);
    LOG(INFO) << "Sip app module recv keepalive message: " << pjsip_rx_data_get_info(rdataPtr);;

    std::string cmdTypeStr;
    MyStrHelper::ConvertToLowStr(keepAliveMsgBody.cmdType, cmdTypeStr);
    if ("keepalive" != cmdTypeStr) {
        LOG(ERROR) << "Sip app module recv keepalive message failed. invalid cmd type. " << msgInfo << ".";
        return MyStatus_t::FAILED;
    }

    if (!keepAliveMsgBody.status) {
        LOG(ERROR) << "Sip app module recv keepalive message failed. status is error. " << msgInfo << ".";
        return MyStatus_t::FAILED;
    }

    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServer(sipServWkPtr)) {
        LOG(ERROR) << "Sip app module recv keepalive message failed. find sip server failed. " << msgInfo << ".";
        return MyStatus_t::FAILED;
    }

    // 处理sip保活请求
    return sipServWkPtr.lock()->onServRecvSipKeepAliveReqMsg(rdataPtr);
}

MyStatus_t MySipMsgProcApp::onProcSipCatalogQueryReqMsg(MySipRxDataPtr rdataPtr)
{
    MySipCatalogReqMsgBody_dt catalogMsgBody;
    std::string msgBodyStr = static_cast<const char*>(rdataPtr->msg_info.msg->body->data);
    if (MyStatus_t::SUCCESS != MyXmlHelper::ParseSipCatalogQueryReqMsgBody(msgBodyStr, catalogMsgBody)) {
        LOG(ERROR) << "Sip app module recv catalog query message failed. parse message failed.";
        return MyStatus_t::FAILED;
    }
    else {
        LOG(INFO) << "Sip app module recv catalog query message: " << pjsip_rx_data_get_info(rdataPtr);
    }

    std::string msgInfo;
    MySipMsgHelper::PrintSipCatalogMsgBody(catalogMsgBody, msgInfo);

    std::string cmdTypeStr;
    MyStrHelper::ConvertToLowStr(catalogMsgBody.cmdType, cmdTypeStr);

    if ("catalog" != cmdTypeStr) {
        LOG(ERROR) << "Sip app module recv catalog query message failed. invalid cmd type. " << msgInfo << ".";
        return MyStatus_t::FAILED;
    }

    if (catalogMsgBody.deviceId != m_servId) {
        LOG(ERROR) << "Sip app module recv catalog query message failed. get local serv id failed. " << msgInfo << ".";
        return MyStatus_t::FAILED;
    }

    MySipServer::SmtWkPtr sipServWkPtr; 
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServer(sipServWkPtr)) {
        LOG(ERROR) << "Sip app module recv catalog query message failed. find sip server failed. message serv id: " 
                   << catalogMsgBody.deviceId << ".";

        return MyStatus_t::FAILED;
    }

    // 处理sip设备目录查询请求
    return sipServWkPtr.lock()->onServRecvSipCatalogQueryReqMsg(rdataPtr);
}

MyStatus_t MySipMsgProcApp::onProcSipCatalogResponseReqMsg(MySipRxDataPtr rdataPtr)
{
    // 获取消息内容
    std::string msgBodyStr = static_cast<const char*>(rdataPtr->msg_info.msg->body->data);

    // 获取消息内容中的设备id
    std::string sipRegLowServId;
    MyXmlHelper::GetSipCatalogRespMsgDeviceId(msgBodyStr, sipRegLowServId);
    
    if (sipRegLowServId.empty()) {
        LOG(ERROR) << "Sip app module recv catalog response message failed. invalid lower regist server id.";
        return MyStatus_t::FAILED;
    }

    LOG(INFO) << "Sip app module recv catalog response message. device id: " << sipRegLowServId << " message: " 
              << pjsip_rx_data_get_info(rdataPtr);

    // 获取本级服务id
    if (MyStatus_t::SUCCESS != MySipRegManage::HasSipRegLowServInfo(sipRegLowServId)) {
        LOG(ERROR) << "Sip app module recv catalog response message failed. invalid low reg serv. message serv id: " << sipRegLowServId << ".";
        return MyStatus_t::FAILED;
    }

    MySipServer::SmtWkPtr sipServWkPtr; 
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServer(sipServWkPtr)) {
        LOG(ERROR) << "Sip app module recv catalog response message failed.  sip server invalid.";
        return MyStatus_t::FAILED;
    }

    // 处理sip设备目录响应请求
    return sipServWkPtr.lock()->onServRecvSipCatalogRespMsg(rdataPtr);
}

pj_status_t MySipMsgProcApp::OnAppModuleLoadCb(MySipEndptPtr endpt)
{
    return PJ_SUCCESS;
}

pj_status_t MySipMsgProcApp::OnAppModuleUnLoadCb(void)
{
    return PJ_SUCCESS;
}

pj_status_t MySipMsgProcApp::OnAppModuleStartCb(void)
{
    return PJ_SUCCESS;
}

pj_status_t MySipMsgProcApp::OnAppModuleStopCb(void)
{
    return PJ_SUCCESS;
}

pj_bool_t MySipMsgProcApp::OnAppModuleRecvReqCb(MySipRxDataPtr rdataPtr)
{
    char msgBuf[1024] = {0};
    pjsip_msg_print(rdataPtr->msg_info.msg, msgBuf, sizeof(msgBuf));
    LOG(INFO) << "Recv request message: " << "\n-------------------------------\n" << msgBuf << "\n-------------------------------";

    // sip 消息uri解析
    char buf[256];
    pjsip_uri_print(PJSIP_URI_IN_REQ_URI, rdataPtr->msg_info.msg->line.req.uri, buf, sizeof(buf));

    // 消息处理app获取
    MySipMsgProcApp::SmtWkPtr sipMsgProcAppWkPtr;
    if (MyStatus_t::SUCCESS != MyAppManage::GetSipMsgProcApp(sipMsgProcAppWkPtr)) {
        LOG(ERROR) << "Sip msg proc app module recv sip request message failed. find msg proc app failed. uri: " << buf << ".";
        return PJ_FALSE;
    }
    
    auto sipMsgProcAppPtr = sipMsgProcAppWkPtr.lock();
    if (nullptr == sipMsgProcAppPtr) {
        LOG(ERROR) << "Sip msg proc app module recv sip request message failed. invalid msg proc app. uri: " << buf << ".";
        return PJ_FALSE;
    }

    // 消息处理
    if (PJSIP_REGISTER_METHOD == rdataPtr->msg_info.msg->line.req.method.id) {
        // 收到sip regist请求消息
        if (MyStatus_t::SUCCESS != sipMsgProcAppPtr->onProcSipRegisterReqMsg(rdataPtr)) {
            LOG(ERROR) << "Sip msg proc app module recv sip register message failed. proc msg failed. uri: " << buf << ".";
            return PJ_FALSE;
        }
    }
    else if (PJSIP_INVITE_METHOD == rdataPtr->msg_info.msg->line.req.method.id) {
        // 收到sip invite请求消息
        if (MyStatus_t::SUCCESS != sipMsgProcAppPtr->onProcSipInviteReqMsg(rdataPtr)) {
            LOG(ERROR) << "Sip msg proc app module recv sip invite message failed. proc msg failed. uri: " << buf << ".";
            return PJ_FALSE;
        }
    }
    else if (PJSIP_OTHER_METHOD == rdataPtr->msg_info.msg->line.req.method.id) {
        // 收到其他请求消息
        MySipMsgBody_t type = MySipMsgBody_t::SIP_MSG_UNKNOWN;
        MyXmlHelper::GetSipMsgBodyType(static_cast<const char*>(rdataPtr->msg_info.msg->body->data), type);

        if (MySipMsgBody_t::SIP_MSG_KEEPALIVE == type) {
            // 收到sip keepalive请求消息
            if (MyStatus_t::SUCCESS != sipMsgProcAppPtr->onProcSipKeepAliveReqMsg(rdataPtr)) {
                LOG(ERROR) << "Sip msg proc app module recv sip keepalive message failed. proc msg failed. uri: " << buf << ".";
                return PJ_FALSE;
            }
        }
        else if (MySipMsgBody_t::SIP_MSG_CATALOG_QUERY == type) {
            // 收到sip catalog查询请求消息
            if (MyStatus_t::SUCCESS != sipMsgProcAppPtr->onProcSipCatalogQueryReqMsg(rdataPtr)) {
                LOG(ERROR) << "Sip msg proc app module recv sip catalog query message failed. proc msg failed. uri: " << buf << ".";
                return PJ_FALSE;
            }
        }
        else if (MySipMsgBody_t::SIP_MSG_CATALOG_RESPONSE == type) {
            // 收到sip catalog响应请求消息
            if (MyStatus_t::SUCCESS != sipMsgProcAppPtr->onProcSipCatalogResponseReqMsg(rdataPtr)) {
                LOG(ERROR) << "Sip msg proc app module recv sip catalog response message failed. proc msg failed. uri: " << buf << ".";
                return PJ_FALSE;
            }
        }
    }
    else {
        LOG(ERROR) << "Sip msg proc app module recv sip unknown message failed. proc msg failed. uri: " << buf << ".";
        return PJ_FALSE;
    }
    return PJ_SUCCESS;
}

pj_bool_t MySipMsgProcApp::OnAppModuleRecvRespCb(MySipRxDataPtr rdataPtr)
{
    return PJ_SUCCESS;
}

pj_status_t MySipMsgProcApp::OnAppModuleSendReqCb(MySipTxDataPtr tdataPtr)
{
    return PJ_SUCCESS;
}

pj_status_t MySipMsgProcApp::OnAppModuleSendRespCb(MySipTxDataPtr tdataPtr)
{
    return PJ_SUCCESS;
}

void MySipMsgProcApp::OnAppModuleTsxStateChangeCb(MySipTsxPtr tsxPtr, MySipEvPtr evPtr)
{

}

}; // namespace MY_APP