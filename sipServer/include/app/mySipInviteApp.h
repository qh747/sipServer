#pragma once
#include <string>
#include <memory>
#include "common/myTypeDef.h"
#include "sdp/mySdpSession.h"

namespace MY_APP {

/**
 * sip应用会话邀请处理类
 */
class MySipInviteApp : public std::enable_shared_from_this<MySipInviteApp>
{
public:
    typedef std::shared_ptr<MySipInviteApp>     SmtPtr;
    typedef std::weak_ptr<MySipInviteApp>       SmtWkPtr;

public:
    MySipInviteApp() : m_status(MY_COMMON::MyStatus_t::FAILED), m_invCbPtr(nullptr) {}
    ~MySipInviteApp();

public:
    /**
     * @brief                                   邀请会话状态改变回调
     * @param invSessPtr                        邀请会话对象
     * @param evPtr                             事件对象
     */
    static void                                 OnInviteStateChanged(MY_COMMON::MySipInvSessionPtr invSessPtr,
                                                    MY_COMMON::MySipEvPtr evPtr);

    /**
     * @brief                                   邀请会话创建回调
     * @param invSessPtr                        邀请会话对象
     * @param evPtr                             事件对象
     */
    static void                                 OnNewInviteSession(MY_COMMON::MySipInvSessionPtr invSessPtr,
                                                    MY_COMMON::MySipEvPtr evPtr);

    /**
     * @brief                                   邀请会话媒体更新回调
     * @param invSessPtr                        邀请会话对象
     * @param status                            状态
     */
    static void                                 OnInviteMediaUpdate(MY_COMMON::MySipInvSessionPtr invSessPtr,
                                                    pj_status_t status);

    /**
     * @brief                                   邀请会话响应Ack回调
     * @param invSessPtr                        邀请会话对象
     * @param rxDataPtr                         接收数据
     */
    static void                                 OnInviteSendAck(MY_COMMON::MySipInvSessionPtr invSessPtr,
                                                    MY_COMMON::MySipRxDataPtr rxDataPtr);

public:
    /**
     * @brief                                   应用初始化
     * @return                                  初始化结果，0-success，-1-failed
     * @param servId                            sip服务ID
     * @param name                              应用名称
     * @param priority                          应用优先级
     */ 
    MY_COMMON::MyStatus_t                       init(const std::string& servId, const std::string& name,
                                                    pjsip_module_priority priority);

    /** 
     * @brief                                   应用启动
     * @return                                  启动结果，0-success，-1-failed
     */ 
    MY_COMMON::MyStatus_t                       run();

    /** 
     * @brief                                   应用停止
     * @return                                  停止结果，0-success，-1-failed
     */ 
    MY_COMMON::MyStatus_t                       shutdown();

public:
    /** 
     * @brief                                   请求设备播放媒体流
     * @return                                  请求结果，0-success，-1-failed
     * @param deviceId                          设备ID
     * @param reqInfo                           请求信息
     * @param respInfo                          响应信息
     */ 
    MY_COMMON::MyStatus_t                       onReqDevicePlayMedia(const std::string& deviceId, const MY_COMMON::MyHttpReqMediaInfo_dt& reqInfo,
                                                    std::string& respInfo) const;

    /**
     * @brief                                   处理上级sip服务的sip invite请求消息
     * @return                                  处理结果，0-success，-1-failed
     * @param rxDataPtr                         sip invite请求消息
     */
    MY_COMMON::MyStatus_t                       onRecvSipInviteReqMsg(MY_COMMON::MySipRxDataPtr rxDataPtr) const;

public:     
    /**     
     * @brief                                   应用是否启动
     * @return                                  获取结果，0-success，-1-failed
     * @param status                            启动结果，0-success，-1-failed
     */             
    MY_COMMON::MyStatus_t                       getState(MY_COMMON::MyStatus_t& status) const;

    /** 
     * @brief                                   获取应用id
     * @return                                  获取结果，0-success，-1-failed
     * @param id                                应用id
     */ 
    MY_COMMON::MyStatus_t                       getId(std::string& id) const;

    /** 
     * @brief                                   获取应用
     * @return                                  获取结果，0-success，-1-failed
     * @param wkPtr                             应用实例
     */
     MY_COMMON::MyStatus_t                      getSipInviteApp(SmtWkPtr& wkPtr);

private:
    /**
     * @brief                                   检查请求设备播放媒体流是否有效
     * @return                                  检查结果，0-success，-1-failed
     * @param deviceId                          设备ID
     * @param reqInfo                           请求信息
     */
    MY_COMMON::MyStatus_t                       reqDevicePlayMediaCheck(const std::string& deviceId,
                                                    const MY_COMMON::MyHttpReqMediaInfo_dt& reqInfo) const;

    /**
     * @brief                                   获取请求设备播放媒体流的相关参数
     * @return                                  检查结果，0-success，-1-failed
     * @param deviceId                          设备ID
     * @param deviceCfg                         设备信息
     * @param deviceOwnerCfg                    设备所属平台信息
     */
    MY_COMMON::MyStatus_t                       reqDevicePlayMediaGetInfo(const std::string& deviceId,
                                                    MY_COMMON::MySipCatalogDeviceCfg_dt& deviceCfg,
                                                    MY_COMMON::MySipCatalogPlatCfg_dt& deviceOwnerCfg) const;

    /**
     * @brief                                   获取请求设备播放媒体流的相关参数
     * @return                                  检查结果，0-success，-1-failed
     * @param deviceId                          设备ID
     * @param poolPtr                           内存池地址
     * @param reqProtoType                      请求协议类型
     * @param sdpSessionPtrAddr                 sdp会话地址
     */
    MY_COMMON::MyStatus_t                       reqDevicePlayMediaPrepareSdp(const std::string& deviceId,
                                                    MY_COMMON::MySipPoolPtr poolPtr,
                                                    MY_COMMON::MyTpProto_t reqProtoType,
                                                    MY_COMMON::MySipSdpSessionPtrAddr sdpSessionPtrAddr) const;

    /**
     * @brief                                   获取请求设备播放媒体流的相关参数
     * @return                                  检查结果，0-success，-1-failed
     * @param deviceId                          设备ID
     * @param deviceCfg                         设备信息
     * @param deviceOwnerCfg                    设备所属平台信息
     * @param protoType                         协议类型
     * @param dlgPtrAddr                        会话地址
     */
    MY_COMMON::MyStatus_t                       reqDevicePlayMediaConstructDialog(const std::string& deviceId,
                                                    const MY_COMMON::MySipCatalogDeviceCfg_dt& deviceCfg,
                                                    const MY_COMMON::MySipCatalogPlatCfg_dt& deviceOwnerCfg,
                                                    MY_COMMON::MyTpProto_t protoType,
                                                    MY_COMMON::MySipDialogPtrAddr dlgPtrAddr) const;

    /**
     * @brief                                   设置请求设备播放媒体流的传输协议
     * @return                                  设置结果，0-success，-1-failed
     * @param deviceId                          设备ID
     * @param deviceOwnerCfg                    设备所属平台配置
     * @param dlgPtr                            会话
     */
    MY_COMMON::MyStatus_t                       reqDevicePlayMediaSetTransport(const std::string& deviceId,
                                                    const MY_COMMON::MySipCatalogPlatCfg_dt& deviceOwnerCfg, 
                                                    MY_COMMON::MySipDialogPtr dlgPtr) const;

private:    
    //                                          sip服务ID
    std::string                                 m_servId;

    //                                          应用模块ID
    MY_COMMON::MySipAppIdCfg_dt                 m_appIdCfg;

    //                                          启动状态 
    MY_COMMON::MyAtomicStatus                   m_status; 

    //                                          invite回调函数地址
    MY_COMMON::MySipInvCbSmtPtr                 m_invCbPtr;

    //                                          本端sdp - 用于请求媒体流播放
    MY_SDP::MySdpSession::Ptr                   m_localSdpPlayPtr;
};

}; // namespace MY_APP