#include <sstream>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include "app/mySipApp.h"

namespace MY_APP {

/**
 * sip应用析构类
 */
class MySipAppRels
{
public:
    MySipAppRels(MySipAppWrapper::SipAppEndptPtr endptPtr) : m_endpointPtr(endptPtr) {}
    ~MySipAppRels() {
        if (nullptr != MySipAppWrapper::AppModule()) {
            MySipAppWrapper::Destory(m_endpointPtr);
        }
    }

private:
    MySipAppWrapper::SipAppEndptPtr m_endpointPtr;
};

MySipAppWrapper::SipAppModPtr MySipAppWrapper::AppModulePtr = nullptr;

MySipAppWrapper::SipAppModPtr MySipAppWrapper::AppModule()
{
    return AppModulePtr;
}

MyStatus_t MySipAppWrapper::Init(SipAppEndptPtr endpt, const std::string& name, pjsip_module_priority priority)
{
    if (nullptr != AppModulePtr) {
        LOG(WARNING) << "Sip app module has been init: " << MySipAppWrapper::GetAppModuleInfo() << ".";
        return MyStatus_t::SUCCESS;
    }

    if (nullptr == endpt) {
        LOG(ERROR) << "Sip app module init failed: endpt is null.";
        return MyStatus_t::FAILED;
    }

    AppModulePtr                                = new pjsip_module();
    static std::string MY_SIP_APP_WRAPPER_NAME  = (name.empty() ? "MySipApp" : name);
    AppModulePtr->name.ptr                      = new char[MY_SIP_APP_WRAPPER_NAME.length() + 1];
    strncpy(AppModulePtr->name.ptr, MY_SIP_APP_WRAPPER_NAME.c_str(), MY_SIP_APP_WRAPPER_NAME.length());
    AppModulePtr->name.slen                     = MY_SIP_APP_WRAPPER_NAME.length();
    AppModulePtr->id                            = -1;
    AppModulePtr->priority                      = priority;
    AppModulePtr->prev                          = nullptr;
    AppModulePtr->next                          = nullptr;
    AppModulePtr->load                          = MySipAppWrapper::OnAppModuleLoadCb;
    AppModulePtr->unload                        = MySipAppWrapper::OnAppModuleUnLoadCb;
    AppModulePtr->start                         = MySipAppWrapper::OnAppModuleStartCb;
    AppModulePtr->stop                          = MySipAppWrapper::OnAppModuleStopCb;
    AppModulePtr->on_rx_request                 = MySipAppWrapper::OnAppModuleRecvReqCb;
    AppModulePtr->on_rx_response                = MySipAppWrapper::OnAppModuleRecvRespCb;
    AppModulePtr->on_tx_request                 = MySipAppWrapper::OnAppModuleSendReqCb;
    AppModulePtr->on_tx_response                = MySipAppWrapper::OnAppModuleSendRespCb;
    AppModulePtr->on_tsx_state                  = MySipAppWrapper::OnAppModuleTsxStateChangeCb;

    pj_status_t result = pjsip_endpt_register_module(endpt, AppModulePtr);
    if(PJ_SUCCESS != result) {
        delete AppModulePtr;
        AppModulePtr = nullptr;

        LOG(ERROR) << "Sip app module init failed: register endpoint faild. code: " << result << ".";
        return MyStatus_t::FAILED;
    }

    static MySipAppRels rels(endpt);

    LOG(INFO) << "Sip app module init success: " << MySipAppWrapper::GetAppModuleInfo() << ".";
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipAppWrapper::Destory(SipAppEndptPtr endpt)
{
    if (nullptr == AppModulePtr) {
        LOG(WARNING) << "Sip app module ia not init.";
        return MyStatus_t::SUCCESS;
    }

    LOG(INFO) << "Sip app module destory success: " << MySipAppWrapper::GetAppModuleInfo() << ".";

    if (nullptr != AppModulePtr->name.ptr) {
        delete[] AppModulePtr->name.ptr;
    }

    if (nullptr != endpt) {
        pjsip_endpt_unregister_module(endpt, AppModulePtr);
    }
    else {
        delete AppModulePtr;
    }

    AppModulePtr = nullptr;
    return MyStatus_t::SUCCESS;
}

std::string MySipAppWrapper::GetAppModuleInfo()
{
    if (nullptr != AppModulePtr) {
        std::stringstream ss;
        ss << "name: " << AppModulePtr->name.ptr << " id: " << AppModulePtr->id << " priority: " << AppModulePtr->priority;
        return ss.str();
    }
    return "";
}

pj_status_t MySipAppWrapper::OnAppModuleLoadCb(SipAppEndptPtr endpt)
{
    return PJ_SUCCESS;
}

pj_status_t MySipAppWrapper::OnAppModuleUnLoadCb(void)
{
    return PJ_SUCCESS;
}

pj_status_t MySipAppWrapper::OnAppModuleStartCb(void)
{
    return PJ_SUCCESS;
}

pj_status_t MySipAppWrapper::OnAppModuleStopCb(void)
{
    return PJ_SUCCESS;
}

pj_bool_t MySipAppWrapper::OnAppModuleRecvReqCb(SipAppRxDataPtr rdata)
{
    return PJ_SUCCESS;
}

pj_bool_t MySipAppWrapper::OnAppModuleRecvRespCb(SipAppRxDataPtr rdata)
{
    return PJ_SUCCESS;
}

pj_status_t MySipAppWrapper::OnAppModuleSendReqCb(SipAppTxDataPtr tdata)
{
    return PJ_SUCCESS;
}

pj_status_t MySipAppWrapper::OnAppModuleSendRespCb(SipAppTxDataPtr tdata)
{
    return PJ_SUCCESS;
}

void MySipAppWrapper::OnAppModuleTsxStateChangeCb(SipAppTsxPtr tsx, SipAppEvPtr event)
{

}

}; // namespace MY_APP