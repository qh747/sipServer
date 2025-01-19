#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "utils/mySipAppHelper.h"
#include "server/mySipServer.h"
#include "app/mySipMsgProcApp.h"
using MY_COMMON::MyStatus_t;
using MY_COMMON::MySipAppIdCfg_dt;
using MY_UTILS::MySipAppHelper;
using MY_SERVER::MySipServer;

namespace MY_APP {

MySipMsgProcApp::MySipMsgProcApp() : m_appIdPtr(nullptr), m_status(MyStatus_t::FAILED), 
                                     m_appModPtr(nullptr), m_servSmtWkPtr()
{

}

MySipMsgProcApp::~MySipMsgProcApp()
{
    if (MyStatus_t::SUCCESS == m_status.load()) {
        this->shutdown();
    }
}

MyStatus_t MySipMsgProcApp::init(SipServSmtWkPtr servPtr, const std::string& name, pjsip_module_priority priority)
{
    // 状态检查
    if (MyStatus_t::SUCCESS == m_status.load()) {
        LOG(WARNING) << "Sip app module init failed. MySipMsgProcApp is init already.";
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
    m_appIdPtr->name     = name;
    m_appIdPtr->priority = priority;

    // app模块初始化
    m_appModPtr                     = std::make_shared<pjsip_module>();
    m_appModPtr->name.ptr           = const_cast<char*>(m_appIdPtr->name.c_str());
    m_appModPtr->name.slen          = m_appIdPtr->name.length();
    m_appModPtr->id                 = -1;
    m_appModPtr->priority           = m_appIdPtr->priority;
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
    m_appIdPtr->id = std::to_string(m_appModPtr->id);

    LOG(INFO) << "Sip app module init success. " << MySipAppHelper::GetSipAppInfo(*m_appIdPtr) << ".";

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
        LOG(WARNING) << "Sip app module shutdown failed. app is not init.";
        return MyStatus_t::SUCCESS;
    }

    m_status.store(MyStatus_t::FAILED);
    LOG(INFO) << "Sip app module shutdown success. " << MySipAppHelper::GetSipAppInfo(*m_appIdPtr) << ".";

    SipServSmtPtr strongServPtr = m_servSmtWkPtr.lock();
    if (nullptr == strongServPtr) {
        SipAppEndptPtr endptPtr = strongServPtr->getSipServEndptPtr();
        if (nullptr != endptPtr) {
            pjsip_endpt_unregister_module(endptPtr, m_appModPtr.get());
        }
    }
    
    m_appIdPtr.reset();
    m_appModPtr.reset();
    return MyStatus_t::SUCCESS;
}

pj_status_t MySipMsgProcApp::OnAppModuleLoadCb(SipAppEndptPtr endpt)
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

pj_bool_t MySipMsgProcApp::OnAppModuleRecvReqCb(SipAppRxDataPtr rdata)
{
    return PJ_SUCCESS;
}

pj_bool_t MySipMsgProcApp::OnAppModuleRecvRespCb(SipAppRxDataPtr rdata)
{
    return PJ_SUCCESS;
}

pj_status_t MySipMsgProcApp::OnAppModuleSendReqCb(SipAppTxDataPtr tdata)
{
    return PJ_SUCCESS;
}

pj_status_t MySipMsgProcApp::OnAppModuleSendRespCb(SipAppTxDataPtr tdata)
{
    return PJ_SUCCESS;
}

void MySipMsgProcApp::OnAppModuleTsxStateChangeCb(SipAppTsxPtr tsx, SipAppEvPtr event)
{

}

}; // namespace MY_APP