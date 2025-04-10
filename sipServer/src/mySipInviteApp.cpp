#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <pjmedia/sdp_neg.h>
#include "envir/mySystemConfg.h"
#include "envir/mySystemPjsip.h"
#include "utils/mySdpHelper.h"
#include "utils/mySipAppHelper.h"
#include "utils/mySipMsgHelper.h"
#include "manager/myServManage.h"
#include "manager/mySipRegManage.h"
#include "manager/mySipSessionManage.h"
#include "manager/mySipCatalogManage.h"
#include "app/mySipInviteApp.h"
using namespace MY_SDP;
using namespace MY_UTILS;
using namespace MY_COMMON;
using namespace MY_MANAGER;
using MY_ENVIR::MySystemPjsip;
using MY_ENVIR::MySystemConfig;

namespace MY_APP {

void MySipInviteApp::OnInviteStateChanged(MySipInvSessionPtr invSessPtr, MySipEvPtr evPtr)
{

}

void MySipInviteApp::OnNewInviteSession(MySipInvSessionPtr invSessPtr, MySipEvPtr evPtr)
{

}

void MySipInviteApp::OnInviteMediaUpdate(MySipInvSessionPtr invSessPtr, pj_status_t status)
{
    // 解析sip 200 ok消息响应的sdp
    MySipSessionInfo_dt sessionInfo;
    {
        if (nullptr == invSessPtr) {
            LOG(ERROR) << "Sip invite app media update error. invalid invite session.";
            return;
        }

        // 获取sip 200 ok消息响应的sdp
        MySipTxDataPtr errRespDataPtr = nullptr;
        MySipSdpSessionCstPtr remoteSdpSessionPtr = nullptr;
        if (PJ_SUCCESS != pjmedia_sdp_neg_get_active_remote(invSessPtr->neg, &remoteSdpSessionPtr)) {
            pjsip_inv_end_session(invSessPtr, PJSIP_SC_UNSUPPORTED_MEDIA_TYPE, nullptr, &errRespDataPtr);
            pjsip_inv_send_msg(invSessPtr, errRespDataPtr);
    
            LOG(ERROR) << "Sip invite app media update error. get remote sdp error.";
            return;
        }
        
        // 检查对端sdp是否有效
        if (nullptr == remoteSdpSessionPtr || remoteSdpSessionPtr->media_count < 1) {
            pjsip_inv_end_session(invSessPtr, PJSIP_SC_UNSUPPORTED_MEDIA_TYPE, nullptr, &errRespDataPtr);
            pjsip_inv_send_msg(invSessPtr, errRespDataPtr);
    
            LOG(ERROR) << "Sip invite app media update error. invalid remote sdp.";
            return;
        }

        // 对端sdp解析
        char remoteSdpBuf[1024] = {0};
        pjmedia_sdp_print(remoteSdpSessionPtr, remoteSdpBuf, sizeof(remoteSdpBuf));

        MySdpSession::Ptr remoteSdpPtr = std::make_shared<MySdpSession>();
        if (MyStatus_t::SUCCESS != remoteSdpPtr->loadFrom(remoteSdpBuf)) {
            pjsip_inv_end_session(invSessPtr, PJSIP_SC_UNSUPPORTED_MEDIA_TYPE, nullptr, &errRespDataPtr);
            pjsip_inv_send_msg(invSessPtr, errRespDataPtr);

            LOG(ERROR) << "Sip invite app media update error. load remote sdp error.";
            return;
        }

        if (MyStatus_t::SUCCESS != remoteSdpPtr->checkValid()) {
            pjsip_inv_end_session(invSessPtr, PJSIP_SC_UNSUPPORTED_MEDIA_TYPE, nullptr, &errRespDataPtr);
            pjsip_inv_send_msg(invSessPtr, errRespDataPtr);

            LOG(ERROR) << "Sip invite app media update error. invalid remote sdp.";
            return;
        }

        // 本端sdp解析
        MySipSdpSessionCstPtr localSdpSessionPtr = nullptr;
        if (PJ_SUCCESS != pjmedia_sdp_neg_get_active_local(invSessPtr->neg, &localSdpSessionPtr)) {
            pjsip_inv_end_session(invSessPtr, PJSIP_SC_UNSUPPORTED_MEDIA_TYPE, nullptr, &errRespDataPtr);
            pjsip_inv_send_msg(invSessPtr, errRespDataPtr);
    
            LOG(ERROR) << "Sip invite app media update error. get local sdp error.";
            return;
        }

        char localSdpBuf[1024] = {0};
        pjmedia_sdp_print(localSdpSessionPtr, localSdpBuf, sizeof(localSdpBuf));

        MySdpSession::Ptr localSdpPtr = std::make_shared<MySdpSession>();
        if (MyStatus_t::SUCCESS != localSdpPtr->loadFrom(localSdpBuf)) {
            pjsip_inv_end_session(invSessPtr, PJSIP_SC_UNSUPPORTED_MEDIA_TYPE, nullptr, &errRespDataPtr);
            pjsip_inv_send_msg(invSessPtr, errRespDataPtr);

            LOG(ERROR) << "Sip invite app media update error. load local sdp error.";
            return;
        }

        if (MyStatus_t::SUCCESS != remoteSdpPtr->checkValid()) {
            pjsip_inv_end_session(invSessPtr, PJSIP_SC_UNSUPPORTED_MEDIA_TYPE, nullptr, &errRespDataPtr);
            pjsip_inv_send_msg(invSessPtr, errRespDataPtr);

            LOG(ERROR) << "Sip invite app media update error. invalid local sdp.";
            return;
        }

        // 会话信息内容填充
        sessionInfo.deviceId = localSdpPtr->m_origin.m_username;
        {
            // 解析对端服务id
            auto toHeader = static_cast<MySipMsgHdrPtr>(pjsip_msg_find_hdr(invSessPtr->invite_req->msg, 
                PJSIP_H_TO, nullptr));

            if (nullptr == toHeader) {
                pjsip_inv_end_session(invSessPtr, PJSIP_SC_UNSUPPORTED_MEDIA_TYPE, nullptr, &errRespDataPtr);
                pjsip_inv_send_msg(invSessPtr, errRespDataPtr);
    
                LOG(ERROR) << "Sip invite app media update error. to header invalid. device id: " << sessionInfo.deviceId;
                return;
            }

            char toHdrBuf[256] = {0};
            toHeader->vptr->print_on(toHeader, toHdrBuf, sizeof(toHdrBuf));

            std::string remoteServId;
            std::string remoteServIpAddr;
            if (MyStatus_t::SUCCESS != MySipMsgHelper::ParseSipMsgToHdr(toHdrBuf, remoteServId, remoteServIpAddr)) {
                pjsip_inv_end_session(invSessPtr, PJSIP_SC_UNSUPPORTED_MEDIA_TYPE, nullptr, &errRespDataPtr);
                pjsip_inv_send_msg(invSessPtr, errRespDataPtr);
    
                LOG(ERROR) << "Sip invite app media update error. to header parser failed. device id: " << sessionInfo.deviceId;
                return;
            }
            else {
                sessionInfo.remoteServerId = remoteServId;
            }

            // 解析本端服务id
            auto fromHeader = static_cast<MySipMsgHdrPtr>(pjsip_msg_find_hdr(invSessPtr->invite_req->msg, 
                PJSIP_H_FROM, nullptr));

            if (nullptr == fromHeader) {
                pjsip_inv_end_session(invSessPtr, PJSIP_SC_UNSUPPORTED_MEDIA_TYPE, nullptr, &errRespDataPtr);
                pjsip_inv_send_msg(invSessPtr, errRespDataPtr);
    
                LOG(ERROR) << "Sip invite app media update error. from header invalid. device id: " << sessionInfo.deviceId;
                return;
            }

            char fromHdrBuf[256] = {0};
            fromHeader->vptr->print_on(fromHeader, fromHdrBuf, sizeof(fromHdrBuf));

            std::string localServId;
            std::string localServIpAddr;
            if (MyStatus_t::SUCCESS != MySipMsgHelper::ParseSipMsgToHdr(fromHdrBuf, localServId, localServIpAddr)) {
                pjsip_inv_end_session(invSessPtr, PJSIP_SC_UNSUPPORTED_MEDIA_TYPE, nullptr, &errRespDataPtr);
                pjsip_inv_send_msg(invSessPtr, errRespDataPtr);
    
                LOG(ERROR) << "Sip invite app media update error. from header parser failed. device id: " << sessionInfo.deviceId;
                return;
            }
            else {
                sessionInfo.localServerId = localServId;
            }
        }

        {
            if (MyStatus_t::SUCCESS != MySdpHelper::ConvertToSdpPlayWay(localSdpPtr->m_sessionName, sessionInfo.playType)) {
                pjsip_inv_end_session(invSessPtr, PJSIP_SC_UNSUPPORTED_MEDIA_TYPE, nullptr, &errRespDataPtr);
                pjsip_inv_send_msg(invSessPtr, errRespDataPtr);
    
                LOG(ERROR) << "Sip invite app media update error. invalid session name. device id: " << sessionInfo.deviceId;
                return;
            }
    
            MySdpMedia::Ptr validLocalMediaPtr;
            for (const auto& media : localSdpPtr->m_mediaPtrVec) {
                if (MyStatus_t::SUCCESS == media->checkValid()) {
                    validLocalMediaPtr = media;
                    break;
                } 
            }
    
            if (nullptr == validLocalMediaPtr) {
                pjsip_inv_end_session(invSessPtr, PJSIP_SC_UNSUPPORTED_MEDIA_TYPE, nullptr, &errRespDataPtr);
                pjsip_inv_send_msg(invSessPtr, errRespDataPtr);
    
                LOG(ERROR) << "Sip invite app media update error. invalid local media. device id: " << sessionInfo.deviceId;
                return;
            }
    
            if (nullptr == validLocalMediaPtr || MyStatus_t::SUCCESS != MySdpHelper::ConvertToSdpProtoType(validLocalMediaPtr->m_proto,
                sessionInfo.protoType)) {
                pjsip_inv_end_session(invSessPtr, PJSIP_SC_UNSUPPORTED_MEDIA_TYPE, nullptr, &errRespDataPtr);
                pjsip_inv_send_msg(invSessPtr, errRespDataPtr);
    
                LOG(ERROR) << "Sip invite app media update error. invalid proto type. device id: " << sessionInfo.deviceId;
                return;
            }
    
            if (!localSdpPtr->m_connection.empty()) {
                sessionInfo.localServIpAddr = localSdpPtr->m_connection.m_address;
            }
            else {
                sessionInfo.localServIpAddr = validLocalMediaPtr->m_connection.m_address;
            }
    
            sessionInfo.localServPort = validLocalMediaPtr->m_port;
            localSdpPtr->toString(sessionInfo.localSdp);
        }

        {
            MySdpMedia::Ptr validRemoteMediaPtr;
            for (const auto& media : remoteSdpPtr->m_mediaPtrVec) {
                if (MyStatus_t::SUCCESS == media->checkValid()) {
                    validRemoteMediaPtr = media;
                    break;
                } 
            }

            if (nullptr == validRemoteMediaPtr) {
                pjsip_inv_end_session(invSessPtr, PJSIP_SC_UNSUPPORTED_MEDIA_TYPE, nullptr, &errRespDataPtr);
                pjsip_inv_send_msg(invSessPtr, errRespDataPtr);

                LOG(ERROR) << "Sip invite app media update error. invalid remote media. device id: " << sessionInfo.deviceId;
                return;
            }

            if (!remoteSdpPtr->m_connection.empty()) {
                sessionInfo.remoteServIpAddr = remoteSdpPtr->m_connection.m_address;
            }
            else {
                sessionInfo.remoteServIpAddr = validRemoteMediaPtr->m_connection.m_address;
            }

            sessionInfo.remoteServPort = validRemoteMediaPtr->m_port;
            remoteSdpPtr->toString(sessionInfo.remoteSdp);
        }

        LOG(INFO) << "Sip invite app media update success. remote sdp: \n" << remoteSdpBuf;
    }

    // 保存会话信息
    MySipSessionManage::AddSipSessionInfo(sessionInfo.deviceId, sessionInfo);
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
    // 处理对于invite的200 ok响应消息
    m_invCbPtr->on_media_update  = &MySipInviteApp::OnInviteMediaUpdate;

    if (PJ_SUCCESS != pjsip_inv_usage_init(endptPtr, m_invCbPtr.get())) {
        LOG(ERROR) << "Sip app module init failed. pjsip_inv_usage_init() failed.";
        return MyStatus_t::FAILED;
    }

    MySipModPtr modPtr = pjsip_inv_usage_instance();
    if (nullptr == modPtr) {
        LOG(ERROR) << "Sip app module init failed. pjsip invite module is invalid.";
        return MyStatus_t::FAILED;
    }

    // 本端sdp初始化
    MySipSdpCfg_dt sdpCfg;
    if (MyStatus_t::SUCCESS != MySystemConfig::GetSipSdpCfg(sdpCfg)) {
        LOG(ERROR) << "Sip app module init failed. get sdp config error.";
        return MyStatus_t::FAILED;
    }

    m_localSdpPlayPtr = std::make_shared<MySdpSession>();
    if (MyStatus_t::SUCCESS != m_localSdpPlayPtr->loadFromFile(sdpCfg.sipSdpPlayFileNameCfg)) {
        LOG(ERROR) << "Sip app module init failed. load sdp file error. file name: " << sdpCfg.sipSdpPlayFileNameCfg;
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
        return MyStatus_t::FAILED;
    }

    m_invCbPtr.reset();

    m_status.store(MyStatus_t::FAILED);

    std::string appInfo;
    MySipAppHelper::GetSipAppInfo(m_appIdCfg, appInfo);

    LOG(INFO) << "Sip app module shutdown success. " << appInfo << ".";
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipInviteApp::onReqDevicePlayMedia(const std::string& deviceId, const MyHttpReqMediaInfo_dt& reqInfo, std::string& respInfo) const
{
    // pjsip要求自定义线程进行注册才能使用
    MySystemPjsip::RegistSelfThread();

    // 设备类型校验
    if (MyStatus_t::SUCCESS != this->reqDevicePlayMediaCheck(deviceId, reqInfo)) {
        respInfo = "check device type error.";
        LOG(ERROR) << "Sip app module req device play media error. check device type error. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }

    // 查询设备是否已经关联会话
    if (MyStatus_t::SUCCESS == MySipSessionManageView::HasSipSessionInfo(deviceId)) {
        respInfo = "device already associated with a session.";
        LOG(ERROR) << "Sip app module req device play media error. device already associated with a session. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }

    // 设备查询
    MySipCatalogDeviceCfg_dt deviceCfg;
    MySipCatalogPlatCfg_dt deviceOwnerCfg;
    if (MyStatus_t::SUCCESS != this->reqDevicePlayMediaGetInfo(deviceId, deviceCfg, deviceOwnerCfg)) {
        respInfo = "device not exists.";
        LOG(ERROR) << "Sip app module req device play media error. device not exists. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }

    // 获取sip服务内存池
    MySipPoolPtr poolPtr = nullptr;
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServPool(&poolPtr)) {
        respInfo = "sip server pool invalid.";
        LOG(ERROR) << "Sip app module req device play media error. sip server pool invalid. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }

    // 填充invite请求中的sdp
    MySipSdpSessionPtr sdpSessionPtr = nullptr;
    if (MyStatus_t::SUCCESS != this->reqDevicePlayMediaPrepareSdp(deviceId, poolPtr, reqInfo.protoType, &sdpSessionPtr)) {
        respInfo = "sdp prepared error.";
        LOG(ERROR) << "Sip app module req device play media error. prepare sdp invalid. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }

    // 创建会话
    MySipDialogPtr dlgPtr = nullptr;
    if (MyStatus_t::SUCCESS != this->reqDevicePlayMediaConstructDialog(deviceId, deviceCfg, deviceOwnerCfg,
        reqInfo.protoType, &dlgPtr)) {
        respInfo = "construct dialog error.";

        LOG(ERROR) << "Sip app module req device play media error. construct dialog invalid. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }

    MySipInvSessionPtr invSessPtr = nullptr;
    if(PJ_SUCCESS != pjsip_inv_create_uac(dlgPtr, sdpSessionPtr, 0, &invSessPtr)) {
        respInfo = "pjsip create uac error.";

        pjsip_dlg_terminate(dlgPtr);
        LOG(ERROR) << "Sip app module req device play media error. create invite session failed. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }

    // 设置invite请求使用的transport
    if (MyStatus_t::SUCCESS != this->reqDevicePlayMediaSetTransport(deviceId, deviceOwnerCfg, dlgPtr)) {
        respInfo = "set transport error.";

        pjsip_dlg_terminate(dlgPtr);
        LOG(ERROR) << "Sip app module req device play media error. set transport failed. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }

    // 构造invite请求
    MySipTxDataPtr txDataPtr = nullptr;
    if(PJ_SUCCESS != pjsip_inv_invite(invSessPtr, &txDataPtr)) {
        respInfo = "pjsip create invite message error.";

        pjsip_dlg_terminate(dlgPtr);
        LOG(ERROR) << "Sip app module req device play media error. generate invite message error. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }

    pj_str_t subjectName = pj_str(const_cast<char*>("Subject"));
    char subjectBuf[128] = {0};
    sprintf(subjectBuf, "%s:0,%s:0", deviceId.c_str(), m_servId.c_str());

    pj_str_t subjectValue = pj_str(subjectBuf);
    MySipMsgGenericHdrPtr hdr = pjsip_generic_string_hdr_create(poolPtr, &subjectName, &subjectValue);
    pjsip_msg_add_hdr(txDataPtr->msg, reinterpret_cast<MySipMsgHdrPtr>(hdr));

    // 发送invite请求
    if(PJ_SUCCESS != pjsip_inv_send_msg(invSessPtr, txDataPtr)) {
        respInfo = "pjsip send invite message error.";

        pjsip_dlg_terminate(dlgPtr);
        LOG(ERROR) << "Sip app module req device play media error. send invite message error. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }
    else {
        char sdpOffer[1024] = {0};
        pjmedia_sdp_print(sdpSessionPtr, sdpOffer, sizeof(sdpOffer));
        LOG(INFO) << "Sip app module req device play media success. device id: " << deviceId << " sdp offer: " << sdpOffer;
    }

    respInfo = "pjsip send invite message success.";
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipInviteApp::onRecvSipInviteReqMsg(MySipRxDataPtr rxDataPtr) const
{
    MySdpSession remoteSdp;
    MySdpSession localSdp;

    MySipSessionInfo_dt sessionInfo;

    // sdp处理
    int statusCode = pjsip_status_code::PJSIP_SC_OK;
    do {
        // 解析本级服务id
        char toHdrBuf[256] = {0};
        pjsip_uri_print(PJSIP_URI_IN_REQ_URI, rxDataPtr->msg_info.to->uri, toHdrBuf, sizeof(toHdrBuf));

        std::string localServId;
        std::string localServIpAddr;
        if (MyStatus_t::SUCCESS != MySipMsgHelper::ParseSipMsgToHdr(toHdrBuf, localServId, localServIpAddr)) {
            LOG(ERROR) << "Sip invite app receive invite request message error. parse to header failed.";
            statusCode = pjsip_status_code::PJSIP_SC_BAD_REQUEST;
            break;
        }
        else {
            sessionInfo.localServerId = localServId;
        }

        // 解析上级服务id
        char fromHdrBuf[256] = {0};
        pjsip_uri_print(PJSIP_URI_IN_REQ_URI, rxDataPtr->msg_info.from->uri, fromHdrBuf, sizeof(fromHdrBuf));

        std::string remoteServId;
        std::string remoteServIpAddr;
        if (MyStatus_t::SUCCESS != MySipMsgHelper::ParseSipMsgFromHdr(fromHdrBuf, remoteServId, remoteServIpAddr)) {
            LOG(ERROR) << "Sip invite app receive invite request message error. parse from header failed.";
            statusCode = pjsip_status_code::PJSIP_SC_BAD_REQUEST;
            break;
        }
        else {
            sessionInfo.remoteServerId = remoteServId;
        }

        // sdp解析
        if (nullptr == rxDataPtr->msg_info.msg->body) {
            LOG(ERROR) << "Sip invite app receive invite request message error. invalid msg body. ";
            statusCode = pjsip_status_code::PJSIP_SC_BAD_REQUEST;
            break;
        }

        MySipRdataSdpInfoPtr sdpInfoPtr = pjsip_rdata_get_sdp_info(rxDataPtr);
        if (nullptr == sdpInfoPtr) {
            LOG(ERROR) << "Sip invite app receive invite request message error. invalid sdp info. ";
            statusCode = pjsip_status_code::PJSIP_SC_BAD_REQUEST;
            break;
        }

        // sdp校验
        MySipSdpSessionPtr sdpSessionPtr = sdpInfoPtr->sdp;
        if (nullptr == sdpSessionPtr || 0 == sdpSessionPtr->media_count) {
            LOG(ERROR) << "Sip invite app receive invite request message error. invalid sdp session. ";
            statusCode = pjsip_status_code::PJSIP_SC_BAD_REQUEST;
            break;
        }

        // 设备类型校验
        if (DEVICE_TYPE_LENGTH != sdpSessionPtr->origin.user.slen) {
            LOG(ERROR) << "Sip invite app receive invite request message error. invalid field origin length. ";
            statusCode = pjsip_status_code::PJSIP_SC_BAD_REQUEST;
            break;
        }

        sessionInfo.deviceId = std::string(sdpSessionPtr->origin.user.ptr, sdpSessionPtr->origin.user.slen).substr(0, 20);
        std::string deviceType = sessionInfo.deviceId.substr(10, 3);
        if (DEVICE_TYPE_IP_CAMERA != deviceType && DEVICE_TYPE_NETWORK_VIDEO_RECORDER != deviceType) {
            LOG(ERROR) << "Sip invite app receive invite request message error. invalid device type. ";
            statusCode = pjsip_status_code::PJSIP_SC_BAD_REQUEST;
            break;
        }

        // 设备查询
        MySipCatalogDeviceCfg_dt deviceCfg;
        if (MyStatus_t::SUCCESS != MySipCatalogManageView::GetSipCatalogDeviceCfg(m_servId, sessionInfo.deviceId, 
            deviceCfg)) {
            LOG(ERROR) << "Sip invite app receive invite request message error. device not exists. device id: " << sessionInfo.deviceId;
            statusCode = pjsip_status_code::PJSIP_SC_NOT_FOUND;
            break;
        }

        // 设备是否已经关联会话
        if (MyStatus_t::SUCCESS == MySipSessionManageView::HasSipSessionInfo(sessionInfo.deviceId)) {
            LOG(ERROR) << "Sip invite app receive invite request message error. device already associated with a session. device id: " 
                       << sessionInfo.deviceId;
            statusCode = pjsip_status_code::PJSIP_SC_FORBIDDEN;
            break;
        }

        // sdp协商
        char sdpOfferBuf[1024] = {0};
        pjmedia_sdp_print(sdpSessionPtr, sdpOfferBuf, sizeof(sdpOfferBuf));

        if (MyStatus_t::SUCCESS != remoteSdp.loadFrom(sdpOfferBuf)) {
            LOG(ERROR) << "Sip invite app receive invite request message error. load sdp from string error. device id: " 
                       << sessionInfo.deviceId;
            statusCode = pjsip_status_code::PJSIP_SC_FORBIDDEN;
            break;
        }

        if (MyStatus_t::SUCCESS != MySdpHelper::ConvertToSdpPlayWay(remoteSdp.m_sessionName, sessionInfo.playType)) {
            LOG(ERROR) << "Sip invite app receive invite request message error. invalid session name. device id: " << sessionInfo.deviceId;
            statusCode = pjsip_status_code::PJSIP_SC_FORBIDDEN;
            break;
        }

        MySdpMedia::Ptr validRemoteMediaPtr;
        for (const auto& media : remoteSdp.m_mediaPtrVec) {
            if (MyStatus_t::SUCCESS == media->checkValid()) {
                validRemoteMediaPtr = media;
                break;
            } 
        }

        if (nullptr == validRemoteMediaPtr || MyStatus_t::SUCCESS != MySdpHelper::ConvertToSdpProtoType(validRemoteMediaPtr->m_proto,
            sessionInfo.protoType)) {
            LOG(ERROR) << "Sip invite app receive invite request message error. invalid proto type. device id: " << sessionInfo.deviceId;
            statusCode = pjsip_status_code::PJSIP_SC_FORBIDDEN;
            break;
        }

        if (!remoteSdp.m_connection.empty()) {
            sessionInfo.remoteServIpAddr = remoteSdp.m_connection.m_address;
        }
        else {
            sessionInfo.remoteServIpAddr = validRemoteMediaPtr->m_connection.m_address;
        }

        sessionInfo.remoteServPort = validRemoteMediaPtr->m_port;
        remoteSdp.toString(sessionInfo.remoteSdp);

        if (MyStatus_t::SUCCESS != MySdpSession::CreateAnswer(remoteSdp, *m_localSdpPlayPtr, localSdp)) {
            LOG(ERROR) << "Sip invite app receive invite request message error. create answer sdp error. device id: " 
                       << sessionInfo.deviceId;
            statusCode = pjsip_status_code::PJSIP_SC_FORBIDDEN;
            break;
        }

        MySdpMedia::Ptr validLocalMediaPtr;
        for (const auto& media : localSdp.m_mediaPtrVec) {
            if (MyStatus_t::SUCCESS == media->checkValid()) {
                validLocalMediaPtr = media;
                break;
            } 
        }

        if (nullptr == validLocalMediaPtr) {
            LOG(ERROR) << "Sip invite app receive invite request message error. invalid local media. device id: " << sessionInfo.deviceId;
            statusCode = pjsip_status_code::PJSIP_SC_FORBIDDEN;
            break;
        }

        if (!localSdp.m_connection.empty()) {
            sessionInfo.localServIpAddr = localSdp.m_connection.m_address;
        }
        else {
            sessionInfo.localServIpAddr = validLocalMediaPtr->m_connection.m_address;
        }
        
        sessionInfo.localServPort = validLocalMediaPtr->m_port;
        localSdp.toString(sessionInfo.localSdp);

    } while(false);

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip invite app answer invite error. get pjsip endpoint failed. device id: " << sessionInfo.deviceId;
        return MyStatus_t::FAILED;
    }

    // 创建sip应答消息
    pj_str_t type    = {const_cast<char*>("Application"),11};
    pj_str_t sdpType = {const_cast<char*>("SDP"),3};

    MySipTxDataPtr txDataPtr = nullptr;
    pjsip_endpt_create_response(endptPtr, rxDataPtr, statusCode, nullptr, &txDataPtr);

    if (pjsip_status_code::PJSIP_SC_OK != statusCode) {
        txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &sdpType, 
        &(pjsip_rdata_get_sdp_info(rxDataPtr)->body));
    }
    else {
        std::string localSdpStr;
        if (MyStatus_t::SUCCESS != localSdp.toString(localSdpStr)) {
            LOG(ERROR) << "Sip invite app answer invite error. local sdp to string error. device id: " << sessionInfo.deviceId;
            return MyStatus_t::FAILED;
        }

        pj_str_t sdpData = pj_str(const_cast<char*>(localSdpStr.c_str()));
        txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &sdpType, &sdpData);
    }

    // 获取本地sip服务地址配置
    MySipServAddrCfg_dt servAddrCfg;
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServAddrCfg(servAddrCfg)) {
        LOG(ERROR) << "Sip invite app answer invite error. get sip serv addr cfg failed. device id: " << sessionInfo.deviceId;
        return MyStatus_t::FAILED;
    }

    std::string sContact;
    MySipMsgHelper::GenerateSipMsgContactHeader(servAddrCfg.id, servAddrCfg.ipAddr, servAddrCfg.regPort, 
        sContact);

    const pj_str_t contactHeader = pj_str(const_cast<char*>("Contact"));
    const pj_str_t param = pj_str(const_cast<char*>(sContact.c_str()));

    MySipMsgGenericHdrPtr customHeader = pjsip_generic_string_hdr_create(txDataPtr->pool,&contactHeader,&param);
    pjsip_msg_add_hdr(txDataPtr->msg, (MySipMsgHdrPtr)customHeader);

    pjsip_response_addr resAddr;
    if (PJ_SUCCESS != pjsip_get_response_addr(txDataPtr->pool, rxDataPtr, &resAddr)) {
        pjsip_tx_data_dec_ref(txDataPtr);
        return MyStatus_t::FAILED;
    }

    // 发送sip应答消息
    if (PJ_SUCCESS != pjsip_endpt_send_response(endptPtr, &resAddr, txDataPtr, nullptr, nullptr)) {
        pjsip_tx_data_dec_ref(txDataPtr);
        return MyStatus_t::FAILED;
    }

    if (pjsip_status_code::PJSIP_SC_OK != statusCode) {
        return MyStatus_t::FAILED;
    }

    LOG(INFO) << "Sip invite app answer invite success. device id: " << sessionInfo.deviceId << " sdp offer: " << sessionInfo.remoteSdp
              << " sdp answer: " << sessionInfo.localSdp;

    // 保存会话信息
    MySipSessionManage::AddSipSessionInfo(sessionInfo.deviceId, sessionInfo);
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

MyStatus_t MySipInviteApp::reqDevicePlayMediaCheck(const std::string& deviceId, const MyHttpReqMediaInfo_dt& reqInfo) const
{
    // 校验设备类型
    std::string deviceType = deviceId.substr(10, 3);
    if (DEVICE_TYPE_IP_CAMERA != deviceType && DEVICE_TYPE_NETWORK_VIDEO_RECORDER != deviceType) {
        LOG(ERROR) << "Check req device play media failed. device type is invalid. type: " << deviceType;
        return MyStatus_t::FAILED;
    }

    // 校验播放方式
    if (MyMediaPlayWay_t::PLAY != reqInfo.playType) {
        LOG(ERROR) << "Check req device play media failed. play way is invalid. ";
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipInviteApp::reqDevicePlayMediaGetInfo(const std::string& deviceId, MySipCatalogDeviceCfg_dt& deviceCfg,
    MySipCatalogPlatCfg_dt& deviceOwnerCfg) const
{
    // 查询设备信息
    std::string deviceOwnerId;
    if (MyStatus_t::SUCCESS != MySipCatalogManageView::GetSipCatalogDeviceCfg(m_servId, deviceId, deviceCfg)) {
        // 设备不在本级平台下挂载，查询下级服务
        MySipRegServAddrMap lowRegSipServAddrMap;
        if (MyStatus_t::SUCCESS != MySipCatalogManageView::GetSipRespInfoMap(lowRegSipServAddrMap)) {
            LOG(ERROR) << "Get req device info failed. device not belong to sip server. device id: " << deviceId;
            return MyStatus_t::FAILED;
        }

        // 查询设备所属下级平台
        for (const auto& pair : lowRegSipServAddrMap) {
            if (MyStatus_t::SUCCESS == MySipCatalogManageView::GetSipCatalogDeviceCfg(pair.first, deviceId, deviceCfg)) {
                deviceOwnerId = pair.first;
                break;
            }
        }
    }
    else {
        deviceOwnerId = m_servId;
    }

    if (deviceOwnerId.empty()) {
        LOG(ERROR) << "Get req device info failed. device owner id invalid. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }

    // 查询设备所属平台信息
    MySipCatalogPlatCfgMap platCfgMap;
    if (MyStatus_t::SUCCESS != MySipCatalogManageView::GetSipCatalogPlatCfgMap(deviceOwnerId, platCfgMap)) {
        LOG(ERROR) << "Sip app module req device play media failed. find sip catalog plat info failed. device id: "
                   << deviceId;
        return MyStatus_t::FAILED;
    }

    deviceOwnerCfg = platCfgMap.begin()->second;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipInviteApp::reqDevicePlayMediaPrepareSdp(const std::string& deviceId, MySipPoolPtr poolPtr,
    MyTpProto_t reqProtoType, MySipSdpSessionPtrAddr sdpSessionPtrAddr) const
{
    if (nullptr == m_localSdpPlayPtr) {
        LOG(ERROR) << "Prepare sdp failed. local sdp info invalid. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }

    auto sdpSessionPtr = static_cast<MySipSdpSessionPtr>(pj_pool_zalloc(poolPtr, sizeof(pjmedia_sdp_session)));
    sdpSessionPtr->origin.version   = m_localSdpPlayPtr->m_version;
    sdpSessionPtr->origin.user      = pj_str(const_cast<char*>(deviceId.c_str()));
    sdpSessionPtr->origin.id        = std::stoul(m_localSdpPlayPtr->m_origin.m_sessionId);
    sdpSessionPtr->origin.net_type  = pj_str(const_cast<char*>(m_localSdpPlayPtr->m_origin.m_netType.c_str()));
    sdpSessionPtr->origin.addr_type = pj_str(const_cast<char*>(m_localSdpPlayPtr->m_origin.m_addrType.c_str()));
    sdpSessionPtr->origin.addr      = pj_str(const_cast<char*>(m_localSdpPlayPtr->m_origin.m_address.c_str()));
    sdpSessionPtr->name             = pj_str(const_cast<char*>(m_localSdpPlayPtr->m_sessionName.c_str()));
    sdpSessionPtr->time.start       = m_localSdpPlayPtr->m_time.m_start;
    sdpSessionPtr->time.stop        = m_localSdpPlayPtr->m_time.m_stop;

    if (!m_localSdpPlayPtr->m_connection.empty()) {
        sdpSessionPtr->conn = static_cast<MySipSdpConnPtr>(pj_pool_zalloc(poolPtr,sizeof(pjmedia_sdp_conn)));
        sdpSessionPtr->conn->net_type  = pj_str(const_cast<char*>(m_localSdpPlayPtr->m_connection.m_netType.c_str()));
        sdpSessionPtr->conn->addr_type = pj_str(const_cast<char*>(m_localSdpPlayPtr->m_connection.m_addrType.c_str()));
        sdpSessionPtr->conn->addr      = pj_str(const_cast<char*>(m_localSdpPlayPtr->m_connection.m_address.c_str()));
    }

    sdpSessionPtr->media_count = m_localSdpPlayPtr->m_mediaPtrVec.size();
    unsigned int mediaIndex = 0;
    for (const auto& media : m_localSdpPlayPtr->m_mediaPtrVec) {
        if (MySdpTrackType_t::SDP_TRACK_TYPE_AUDIO != media->m_type &&
            MySdpTrackType_t::SDP_TRACK_TYPE_VIDEO != media->m_type) {
            continue;
        }

        if (MyTpProto_t::TCP == reqProtoType) {
            if (MySdpRole_t::SDP_ROLE_INVALID == media->m_setup.m_role) {
                LOG(ERROR) << "Prepare sdp failed. req proto type invalid. device id: " << deviceId;
                return MyStatus_t::FAILED;
            }
        }

        auto mediaPtr = static_cast<MySipSdpMediaPtr>(pj_pool_zalloc(poolPtr, sizeof(pjmedia_sdp_media)));
        if (!media->m_connection.empty()) {
            mediaPtr->conn = static_cast<MySipSdpConnPtr>(pj_pool_zalloc(poolPtr,sizeof(pjmedia_sdp_conn)));
            mediaPtr->conn->net_type  = pj_str(const_cast<char*>(media->m_connection.m_netType.c_str()));
            mediaPtr->conn->addr_type = pj_str(const_cast<char*>(media->m_connection.m_addrType.c_str()));
            mediaPtr->conn->addr      = pj_str(const_cast<char*>(media->m_connection.m_address.c_str()));
        }

        mediaPtr->desc.port       = media->m_port;
        mediaPtr->desc.port_count = 1;
        mediaPtr->desc.fmt_count  = media->m_codecPlanVec.size();

        mediaPtr->desc.transport  = (MyTpProto_t::UDP == reqProtoType ? pj_str(const_cast<char*>("RTP/AVP")) :
            pj_str(const_cast<char*>("TCP/RTP/AVP")));

        mediaPtr->desc.media = (MySdpTrackType_t::SDP_TRACK_TYPE_AUDIO == media->m_type ? pj_str(const_cast<char*>("audio")) :
            pj_str(const_cast<char*>("video")));

        mediaPtr->attr_count = 0;
        for (unsigned int fmtIndex = 0; fmtIndex < media->m_codecPlanVec.size(); ++fmtIndex) {
            const auto& codecPlan = media->m_codecPlanVec[fmtIndex];

            std::string ptStr = std::to_string(codecPlan.m_pt);
            pj_strdup2(poolPtr, &mediaPtr->desc.fmt[fmtIndex], ptStr.c_str());

            auto attrPtr = static_cast<MySipSdpAttrPtr>(pj_pool_zalloc(poolPtr, sizeof(pjmedia_sdp_attr)));
            pj_strdup2(poolPtr, &attrPtr->name, "rtpmap");

            std::string rtpMapStr = ptStr + " " + std::string(codecPlan.m_codec) + "/" + std::to_string(codecPlan.m_sampleRate);
            pj_strdup2(poolPtr, &attrPtr->value, rtpMapStr.c_str());

            mediaPtr->attr[mediaPtr->attr_count++] = attrPtr;
        }

        std::string directionStr;
        if (MyStatus_t::SUCCESS == MySdpHelper::ConvertToSdpDirectionStr(MySdpDirection_t::SDP_DIRECTION_RECVONLY, directionStr)) {
            auto attrPtr = static_cast<MySipSdpAttrPtr>(pj_pool_zalloc(poolPtr, sizeof(pjmedia_sdp_attr)));
            pj_strdup2(poolPtr, &attrPtr->name, directionStr.c_str());
            mediaPtr->attr[mediaPtr->attr_count++] = attrPtr;
        }

        if (MyTpProto_t::TCP == reqProtoType) {
            std::string setupStr;
            MySdpHelper::ConvertToSdpRoleStr(media->m_setup.m_role, directionStr);

            auto attrPtr = static_cast<MySipSdpAttrPtr>(pj_pool_zalloc(poolPtr, sizeof(pjmedia_sdp_attr)));
            attrPtr->name = pj_str(const_cast<char*>("setup"));
            attrPtr->value = pj_str(const_cast<char*>(setupStr.c_str()));
            mediaPtr->attr[mediaPtr->attr_count++] = attrPtr;
        }

        std::string ssrcStr = std::to_string(media->m_ssrcVec[0]);
        pj_strdup2(poolPtr, &sdpSessionPtr->gb28181_extend_y, ssrcStr.c_str());

        sdpSessionPtr->media[mediaIndex++] = mediaPtr;
    }

    if (PJ_SUCCESS != pjmedia_sdp_validate(sdpSessionPtr)) {
        LOG(ERROR) << "Prepare sdp failed. sdp invalid. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }

    *sdpSessionPtrAddr = sdpSessionPtr;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipInviteApp::reqDevicePlayMediaConstructDialog(const std::string& deviceId, const MySipCatalogDeviceCfg_dt& deviceCfg,
    const MySipCatalogPlatCfg_dt& deviceOwnerCfg, MyTpProto_t protoType, MySipDialogPtrAddr dlgPtrAddr) const
{
    // 获取本级平台信息
    MySipServAddrCfg_dt servAddr;
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServAddrCfg(servAddr)) {
        LOG(ERROR) << "Construct sip dialog error. get sip server addr error. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }

    std::string sURL;
    std::string sFromHdr;
    std::string sToHdr;
    std::string sContact;

    if (servAddr.id == deviceOwnerCfg.deviceID) {
        // 本级平台向本级设备请求
        MySipMsgHelper::GenerateSipMsgURL(deviceCfg.deviceID, deviceCfg.deviceIp, std::stoul(deviceCfg.devicePort),
            protoType, sURL);
        MySipMsgHelper::GenerateSipMsgFromHeader(servAddr.id, servAddr.ipAddr, sFromHdr);
        MySipMsgHelper::GenerateSipMsgToHeader(deviceCfg.deviceID, deviceCfg.deviceIp, sToHdr);
        MySipMsgHelper::GenerateSipMsgContactHeader(servAddr.id, servAddr.ipAddr, servAddr.regPort, sContact);
    }
    else {
        // 本级平台向下级平台请求
        MySipMsgHelper::GenerateSipMsgURL(deviceCfg.deviceID, deviceOwnerCfg.deviceIp,
            std::stoul(deviceOwnerCfg.devicePort), protoType, sURL);
        MySipMsgHelper::GenerateSipMsgFromHeader(servAddr.id, servAddr.ipAddr, sFromHdr);
        MySipMsgHelper::GenerateSipMsgToHeader(deviceOwnerCfg.deviceID, deviceOwnerCfg.deviceIp, sToHdr);
        MySipMsgHelper::GenerateSipMsgContactHeader(servAddr.id, servAddr.ipAddr, servAddr.regPort, sContact);
    }

    pj_str_t sipMsgURL     = pj_str(const_cast<char*>(sURL.c_str()));
    pj_str_t sipMsgFromHdr = pj_str(const_cast<char*>(sFromHdr.c_str()));
    pj_str_t sipMsgToHdr   = pj_str(const_cast<char*>(sToHdr.c_str()));
    pj_str_t sipMsgContact = pj_str(const_cast<char*>(sContact.c_str()));

    if (PJ_SUCCESS != pjsip_dlg_create_uac(pjsip_ua_instance(), &sipMsgFromHdr, &sipMsgContact,
        &sipMsgToHdr, &sipMsgURL, dlgPtrAddr)) {
        LOG(ERROR) << "Construct sip dialog error. create invite session failed. device id: " << deviceId;
        pjsip_dlg_terminate(*dlgPtrAddr);
        return MyStatus_t::FAILED;
    }

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipInviteApp::reqDevicePlayMediaSetTransport(const std::string& deviceId, const MY_COMMON::MySipCatalogPlatCfg_dt& deviceOwnerCfg, 
    MySipDialogPtr dlgPtr) const
{
    // 指定发送invite请求的transport
    pjsip_tpselector tpSelector;
    tpSelector.type = PJSIP_TPSELECTOR_TRANSPORT;
    tpSelector.disable_connection_reuse = false;

    if (deviceOwnerCfg.deviceID == m_servId) {
        // 本级服务默认使用udp传输
        if (MyStatus_t::SUCCESS != MyServManage::GetSipServUdpTp(&tpSelector.u.transport)) {
            LOG(ERROR) << "Set sip invite transport error. get sip serv udp tp failed. device id: " << deviceId << ".";
            return MyStatus_t::FAILED;
        }
    }
    else {
        // 下级服务根据注册类型选择传输方式
        MySipRegLowServCfg_dt lowRegServCfg;
        if (MyStatus_t::SUCCESS != MySipRegManage::GetSipRegLowServCfg(deviceOwnerCfg.deviceID, lowRegServCfg)) {
            LOG(ERROR) << "Set sip invite transport error. get sip reg low serv cfg failed. device id: " << deviceId << ".";
            return MyStatus_t::FAILED;
        }

        if (MyTpProto_t::UDP == lowRegServCfg.proto) {
            if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegUdpTp(&tpSelector.u.transport)) {
                LOG(ERROR) << "Set sip invite transport error. get sip reg low serv udp tp failed. device id: " << deviceId << ".";
                return MyStatus_t::FAILED;
            }
        }
        else {
            if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegTcpTp(&tpSelector.u.transport, deviceOwnerCfg.deviceIp,
                std::stoul(deviceOwnerCfg.devicePort))) {
                LOG(ERROR) << "Set sip invite transport error. get sip serv reg tcp tp failed. device id: " << deviceId << ".";
                return MyStatus_t::FAILED;
            }
        }
    }

    if (PJ_SUCCESS != pjsip_dlg_set_transport(dlgPtr, &tpSelector)) {
        LOG(ERROR) << "Set sip invite transport error. set dialog transport failed. device id: " << deviceId;
        return MyStatus_t::FAILED;
    }

    return MyStatus_t::SUCCESS;
}

} // namespace MY_APP