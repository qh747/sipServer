#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "common/mySipMsgDef.h"
#include "envir/mySystemPjsip.h"
#include "manager/myAppManage.h"
#include "manager/myServManage.h"
#include "manager/mySipServRegManage.h"
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
    MySipEndptPtr endptPtr = MySystemPjsip::GetPjsipEndptPtr();
    if (nullptr == endptPtr) {
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

        LOG(ERROR) << "Sip app module init failed. register endpoint faild.";
        return MyStatus_t::FAILED;
    }

    // 服务id赋值
    m_servId = servId;

    // app标识初始化 
    m_appIdCfg.name      = name;
    m_appIdCfg.priority  = priority;
    m_appIdCfg.id        = std::to_string(m_appModPtr->id);

    LOG(INFO) << "Sip app module init success. " << MySipAppHelper::GetSipAppInfo(m_appIdCfg) << ".";

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
    LOG(INFO) << "Sip app module shutdown success. " << MySipAppHelper::GetSipAppInfo(m_appIdCfg) << ".";

    MySipEndptPtr endptPtr = MySystemPjsip::GetPjsipEndptPtr();
    if (nullptr != endptPtr) {
        pjsip_endpt_unregister_module(endptPtr, m_appModPtr.get());
    }
    
    m_appModPtr.reset();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipMsgProcApp::onProcSipReqMsg(MY_COMMON::MySipRxDataPtr rdataPtr)
{
    // 收到sip regist请求消息
    if (PJSIP_REGISTER_METHOD == rdataPtr->msg_info.msg->line.req.method.id) {
        // sip regist uri解析
        char buf[256];
        pjsip_uri_print(PJSIP_URI_IN_REQ_URI, rdataPtr->msg_info.msg->line.req.uri, buf, sizeof(buf));
        LOG(INFO) << "Sip app module recv sip regist request. uri: " << buf;

        MySipMsgUri_dt sipUri;
        if (MyStatus_t::SUCCESS != MySipMsgHelper::ParseSipMsgURL(buf, sipUri)) {
            LOG(ERROR) << "Sip app module parse sip regist request uri failed. uri: " << buf << ".";
            return MyStatus_t::FAILED;
        }

        auto sipServWkPtr = MyServManage::GetSipServer(sipUri.id);
        if (sipServWkPtr.expired()) {
            LOG(ERROR) << "Sip app module find sip server by sip regist request failed. uri: " << buf << ".";
            return MyStatus_t::FAILED;
        }
        
        // 处理sip注册请求成功
        auto sipServPtr = sipServWkPtr.lock();
        if (MyStatus_t::SUCCESS == sipServPtr->onRecvSipRegMsg(rdataPtr)) {
            return MyStatus_t::SUCCESS;
        }
        // 处理sip注册请求失败
        return MyStatus_t::FAILED;
    }
    // 收到其他请求消息
    else if (PJSIP_OTHER_METHOD == rdataPtr->msg_info.msg->line.req.method.id) {
        MySipMsgBody_t type = MyXmlHelper::GetSipMsgBodyType(static_cast<const char*>(rdataPtr->msg_info.msg->body->data));
        if (MySipMsgBody_t::SIP_MSG_KEEPALIVE == type) {
            MySipKeepAliveMsgBody_dt keepAliveMsgBody;
            if (MyStatus_t::SUCCESS != MyXmlHelper::ParseSipKeepAliveMsgBody(static_cast<char*>(rdataPtr->msg_info.msg->body->data), keepAliveMsgBody)) {
                LOG(ERROR) << "Sip app module recv unknown message. parse keepalive msg failed.";
                return MyStatus_t::FAILED;
            }

            LOG(INFO) << "Sip app module recv keepalive message. " << MySipMsgHelper::PrintSipKeepAliveMsgBody(keepAliveMsgBody) << ".";

            if ("keepalive" != MyStrHelper::ConvertToLowStr(keepAliveMsgBody.cmdType)) {
                LOG(ERROR) << "Sip app module recv keepalive message. invalid cmd type. " << MySipMsgHelper::PrintSipKeepAliveMsgBody(keepAliveMsgBody) << ".";
                return MyStatus_t::FAILED;
            }

            if (!keepAliveMsgBody.status) {
                LOG(ERROR) << "Sip app module recv keepalive message. status is error. " << MySipMsgHelper::PrintSipKeepAliveMsgBody(keepAliveMsgBody) << ".";
                return MyStatus_t::FAILED;
            }

            std::string localServId = MySipServRegManage::GetSipLocalServId(keepAliveMsgBody.deviceId, false);
            if (localServId.empty()) {
                LOG(ERROR) << "Sip app module recv keepalive message. invalid local serv id. " << MySipMsgHelper::PrintSipKeepAliveMsgBody(keepAliveMsgBody) << ".";
                return MyStatus_t::FAILED;
            }

            auto sipServWkPtr = MyServManage::GetSipServer(localServId);
            if (sipServWkPtr.expired()) {
                LOG(ERROR) << "Sip app module find sip server by keepalive message failed. local serv id: " << localServId << ".";
                return MyStatus_t::FAILED;
            }

            // 处理sip注册请求成功
            auto sipServPtr = sipServWkPtr.lock();
            if (MyStatus_t::SUCCESS == sipServPtr->onRecvSipKeepAliveMsg(rdataPtr)) {
                return MyStatus_t::SUCCESS;
            }
        }
        else if (MySipMsgBody_t::SIP_MSG_CATALOG == type) {
            MySipCatalogReqMsgBody_dt catalogMsgBody;
            if (MyStatus_t::SUCCESS != MyXmlHelper::ParseSipCatalogReqMsgBody(static_cast<char*>(rdataPtr->msg_info.msg->body->data), catalogMsgBody)) {
                LOG(ERROR) << "Sip app module recv unknown message. parse catalog msg failed.";
                return MyStatus_t::FAILED;
            }

            LOG(INFO) << "Sip app module recv catalog message. " << MySipMsgHelper::PrintSipCatalogMsgBody(catalogMsgBody) << ".";

            if ("catalog" != MyStrHelper::ConvertToLowStr(catalogMsgBody.cmdType)) {
                LOG(ERROR) << "Sip app module recv catalog message. invalid cmd type. " << MySipMsgHelper::PrintSipCatalogMsgBody(catalogMsgBody) << ".";
                return MyStatus_t::FAILED;
            }

            std::string localServId = MySipServRegManage::GetSipLocalServId(catalogMsgBody.deviceId, false);
            if (localServId.empty()) {
                LOG(ERROR) << "Sip app module recv catalog message. invalid local serv id. " << MySipMsgHelper::PrintSipCatalogMsgBody(catalogMsgBody) << ".";
                return MyStatus_t::FAILED;
            }

            auto sipServWkPtr = MyServManage::GetSipServer(localServId);
            if (sipServWkPtr.expired()) {
                LOG(ERROR) << "Sip app module find sip server by catalog message failed. local serv id: " << localServId << ".";
                return MyStatus_t::FAILED;
            }

            // 处理sip注册请求成功
            auto sipServPtr = sipServWkPtr.lock();
            if (MyStatus_t::SUCCESS == sipServPtr->onRecvSipCatalogMsg(rdataPtr)) {
                return MyStatus_t::SUCCESS;
            }
        }
        // 处理sip注册请求失败
        return MyStatus_t::FAILED;
    }
    // 收到未知请求消息
    else {
        LOG(ERROR) << "Sip app module recv unknown message.";
        return MyStatus_t::FAILED;
    }    
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
    if (PJSIP_REQUEST_MSG == rdataPtr->msg_info.msg->type) {
        // sip 消息uri解析
        char buf[256];
        pjsip_uri_print(PJSIP_URI_IN_REQ_URI, rdataPtr->msg_info.msg->line.req.uri, buf, sizeof(buf));

        MySipMsgUri_dt sipUri;
        if (MyStatus_t::SUCCESS != MySipMsgHelper::ParseSipMsgURL(buf, sipUri)) {
            LOG(ERROR) << "Sip msg proc app module sip request message failed. parse uri failed. uri: " << buf << ".";
            return PJ_FALSE;
        }

        // 消息处理app获取
        auto sipMsgProcAppWkPtr = MyAppManage::GetSipMsgProcApp(sipUri.id);
        auto sipMsgProcAppPtr = sipMsgProcAppWkPtr.lock();
        if (nullptr == sipMsgProcAppPtr) {
            LOG(ERROR) << "Sip msg proc app module sip request message failed. invalid msg proc app. uri: " << buf << ".";
            return PJ_FALSE;
        }

        // 消息处理
        if (MyStatus_t::SUCCESS != sipMsgProcAppPtr->onProcSipReqMsg(rdataPtr)) {
            LOG(ERROR) << "Sip msg proc app module sip request message failed. proc msg failed. uri: " << buf << ".";
            return PJ_FALSE;
        }
    }
    else {
        // todo: sip response message process
        LOG(ERROR) << "Sip msg proc app module recv sip response message failed. current not support recv resp msg.";
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