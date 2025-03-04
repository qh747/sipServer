#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySystemPjsip.h"
#include "utils/mySipAppHelper.h"
#include "utils/mySipMsgHelper.h"
#include "manager/myServManage.h"
#include "manager/mySipRegManage.h"
#include "manager/mySipCatalogManage.h"
#include "app/mySipInviteApp.h"
using namespace MY_UTILS;
using namespace MY_COMMON;
using MY_ENVIR::MySystemPjsip;
using MY_MANAGER::MyServManage;
using MY_MANAGER::MySipRegManage;
using MY_MANAGER::MySipCatalogManageView;

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

    m_invCbPtr.reset();

    m_status.store(MyStatus_t::FAILED);

    std::string appInfo;
    MySipAppHelper::GetSipAppInfo(m_appIdCfg, appInfo);

    LOG(INFO) << "Sip app module shutdown success. " << appInfo << ".";
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipInviteApp::onSipInviteAppReqDeviceMedia(const std::string deviceId, std::string& respInfo)
{
    // 设备类型校验
    std::string deviceType = deviceId.substr(10, 3);
    if (DEVICE_TYPE_IP_CAMERA != deviceType || DEVICE_TYPE_NETWORK_VIDEO_RECORDER != deviceType) {
        respInfo = "invalid device type.";
        LOG(ERROR) << "Sip app module onSipInviteAppReqDeviceMedia failed. device type is invalid. type: " << deviceType;
        return MyStatus_t::FAILED;
    }

    // 设备查询
    std::string servId = m_servId;
    MySipCatalogDeviceCfg_dt deviceCfg;
    if (MyStatus_t::SUCCESS != MySipCatalogManageView::GetSipCatalogDeviceCfg(servId, deviceId, deviceCfg)) {
        // 设备不在本级平台下挂载，查询下级服务
        MySipRegServAddrMap lowRegSipServAddrMap;
        if (MyStatus_t::SUCCESS != MySipCatalogManageView::GetSipRespInfoMap(lowRegSipServAddrMap)) {
            respInfo = "device not exists.";
            LOG(ERROR) << "Sip app module onSipInviteAppReqDeviceMedia failed. device not in current platform and low reg sip server invalid. device id: "
                       << deviceId;
            return MyStatus_t::FAILED;
        }

        // 查询设备所属下级平台
        auto iter = lowRegSipServAddrMap.begin();
        while (lowRegSipServAddrMap.end() != iter++) {
            if (MyStatus_t::SUCCESS == MySipCatalogManageView::GetSipCatalogDeviceCfg(iter->first, deviceId, deviceCfg)) {
                break;
            }
        }

        if (lowRegSipServAddrMap.end() == iter) {
            respInfo = "device not exists.";
            LOG(ERROR) << "Sip app module onSipInviteAppReqDeviceMedia failed. device not exists. device id: " << deviceId;
            return MyStatus_t::FAILED;
        }

        servId = iter->first;
    }

    // 获取下级平台信息
    MySipCatalogPlatCfgMap platCfgMap;
    if (MyStatus_t::SUCCESS != MySipCatalogManageView::GetSipCatalogPlatCfgMap(servId, platCfgMap)) {
        respInfo = "device not exists.";
        LOG(ERROR) << "Sip app module onSipInviteAppReqDeviceMedia failed. find sip catalog plat info failed. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }

    if(platCfgMap.size() > 1) {
        respInfo = "invalid low sip catalog platform info.";
        LOG(ERROR) << "Sip app module onSipInviteAppReqDeviceMedia failed. sip catalog plat info invalid. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }

    const auto& platCfg = platCfgMap.begin()->second;

    MyTpProto_t proto = MyTpProto_t::UDP;
    if (MyStatus_t::SUCCESS != MySipRegManage::GetSipRegLowServProto(platCfg.deviceID, proto)) {
        respInfo = "invalid low sip server proto info.";
        LOG(ERROR) << "Sip app module onSipInviteAppReqDeviceMedia failed. get low sip reg server proto failed. device id: " << deviceId;
    }

    // 获取本级平台信息
    MySipServAddrCfg_dt servAddr;
    MyStatus_t state = MyStatus_t::FAILED;
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServAddrCfg(servAddr)) {
        respInfo = "sip server addr invalid.";
        LOG(ERROR) << "Sip app module onSipInviteAppReqDeviceMedia failed. get current sip server addr failed. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }

    // 构造invite请求
    std::string sURL;
    std::string sFromHdr;
    std::string sToHdr;
    std::string sContact;

    if (servAddr.id == platCfg.deviceID) {
        MySipMsgHelper::GenerateSipMsgURL(deviceCfg.deviceID, deviceCfg.deviceIp, std::stoul(deviceCfg.devicePort), proto, sURL);
        MySipMsgHelper::GenerateSipMsgFromHeader(servAddr.id, servAddr.ipAddr, sFromHdr);
        MySipMsgHelper::GenerateSipMsgToHeader(deviceCfg.deviceID, deviceCfg.deviceIp, sToHdr);
        MySipMsgHelper::GenerateSipMsgContactHeader(servAddr.id, servAddr.ipAddr, servAddr.regPort, sContact);
    }
    else {
        MySipMsgHelper::GenerateSipMsgURL(deviceCfg.deviceID, platCfg.deviceIp, std::stoul(platCfg.devicePort), proto, sURL);
        MySipMsgHelper::GenerateSipMsgFromHeader(servAddr.id, servAddr.ipAddr, sFromHdr);
        MySipMsgHelper::GenerateSipMsgToHeader(deviceCfg.deviceID, platCfg.deviceIp, sToHdr);
        MySipMsgHelper::GenerateSipMsgContactHeader(servAddr.id, servAddr.ipAddr, servAddr.regPort, sContact);
    }

    pj_str_t sipMsgURL     = pj_str(const_cast<char*>(sURL.c_str()));
    pj_str_t sipMsgFromHdr = pj_str(const_cast<char*>(sFromHdr.c_str()));
    pj_str_t sipMsgToHdr   = pj_str(const_cast<char*>(sToHdr.c_str()));
    pj_str_t sipMsgContact = pj_str(const_cast<char*>(sContact.c_str()));

    MySipDialogPtr dlgPtr = nullptr;
    if (PJ_SUCCESS != pjsip_dlg_create_uac(pjsip_ua_instance(), &sipMsgFromHdr, &sipMsgContact, 
                                   &sipMsgToHdr, &sipMsgURL, &dlgPtr)) {
        respInfo = "sip server create invite failed.";
        LOG(ERROR) << "Sip app module onSipInviteAppReqDeviceMedia failed. sip server create invite failed. device id: " << deviceId;
        return MyStatus_t::FAILED; 
    }

    // 填充invite请求中的sdp
    
    
    // 发送invite请求
    
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