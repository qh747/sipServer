#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySystemPjsip.h"
#include "utils/mySipAppHelper.h"
#include "app/mySipInviteApp.h"
using namespace MY_UTILS;
using namespace MY_COMMON;
using MY_ENVIR::MySystemPjsip;

namespace MY_APP {

void MySipInviteApp::OnInviteStateChanged(MY_COMMON::MySipInvSessionPtr invSessPtr, MY_COMMON::MySipEvPtr evPtr)
{

}

void MySipInviteApp::OnNewInviteSession(MY_COMMON::MySipInvSessionPtr invSessPtr, MY_COMMON::MySipEvPtr evPtr)
{

}

void MySipInviteApp::OnInviteMediaUpdate(MY_COMMON::MySipInvSessionPtr invSessPtr, pj_status_t status)
{
    
}

void MySipInviteApp::OnInviteSendAck(MY_COMMON::MySipInvSessionPtr invSessPtr, MY_COMMON::MySipRxDataPtr rxDataPtr)
{

}

MySipInviteApp::MySipInviteApp() : m_status(MyStatus_t::FAILED), m_invCbPtr(nullptr)
{

}

MySipInviteApp::~MySipInviteApp()
{
    if (MyStatus_t::SUCCESS == m_status.load()) {
        this->shutdown();
    }
}
    
MyStatus_t MySipInviteApp::init(const std::string& servId, const std::string& name, pjsip_module_priority priority)
{
    // 状态检查
    if (MyStatus_t::SUCCESS == m_status.load()) {
        LOG(WARNING) << "Sip app module init failed. MySipInviteApp is init already.";
        return MyStatus_t::SUCCESS;
    }

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip app module init failed. endpoint is invalid.";
        return MyStatus_t::FAILED;
    }

    // invite回调函数地址初始化
    m_invCbPtr = std::make_shared<pjsip_inv_callback>();
    m_invCbPtr->on_state_changed = &MySipInviteApp::OnInviteStateChanged;
    m_invCbPtr->on_new_session   = &MySipInviteApp::OnNewInviteSession;
    m_invCbPtr->on_media_update  = &MySipInviteApp::OnInviteMediaUpdate;
    m_invCbPtr->on_send_ack      = &MySipInviteApp::OnInviteSendAck;

    if (PJ_SUCCESS != pjsip_inv_usage_init(endptPtr, m_invCbPtr.get())) {
        LOG(ERROR) << "Sip app module init failed. pjsip_inv_usage_init() failed.";
        return MyStatus_t::FAILED;
    }

    MySipModPtr modPtr = pjsip_inv_usage_instance();
    if (nullptr == modPtr) {
        LOG(ERROR) << "Sip app module init failed. pjsip invite module is invalid.";
        return MyStatus_t::FAILED;
    }

    // 服务id赋值
    m_servId = servId;

    // app标识初始化 
    m_appIdCfg.name     = name;
    m_appIdCfg.priority = priority;
    m_appIdCfg.id       = std::to_string(modPtr->id);

    std::string appInfo;
    MySipAppHelper::GetSipAppInfo(m_appIdCfg, appInfo);
    LOG(INFO) << "Sip app module init success. " << appInfo << ".";

    m_status.store(MyStatus_t::SUCCESS);
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipInviteApp::run()
{
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipInviteApp::shutdown()
{
    // 状态检查
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "Sip app module shutdown failed. MySipInviteApp is not init.";
        return MyStatus_t::SUCCESS;
    }

    m_status.store(MyStatus_t::FAILED);

    std::string appInfo;
    MySipAppHelper::GetSipAppInfo(m_appIdCfg, appInfo);

    LOG(INFO) << "Sip app module shutdown success. " << appInfo << ".";
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipInviteApp::getState(MyStatus_t& status) const
{
    status = m_status.load();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipInviteApp::getId(std::string& id) const
{
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "Sip app module get id failed. MySipInviteApp is not init.";
        return MyStatus_t::FAILED;
    }

    id = m_appIdCfg.id;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipInviteApp::getSipInviteApp(MySipInviteApp::SmtWkPtr& wkPtr)
{
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "Sip app module get work pointer failed. MySipInviteApp is not init.";
        return MyStatus_t::FAILED;
    }

    wkPtr = this->shared_from_this();
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_APP