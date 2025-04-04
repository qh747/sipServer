#pragma once
#include <map>
#include <string>
#include <pjsip.h>
#include <pjsip_ua.h>
#include "common/myConfigDef.h"

namespace MY_COMMON {

// 平台类型
const std::string DEVICE_TYPE_PLATFORM                      = "200";
// 子平台类型
const std::string DEVICE_TYPE_SUB_PLATFORM                  = "215";
// 虚拟子平台类型
const std::string DEVICE_TYPE_VIRTUAL_SUB_PLATFORM          = "216";
// 网络摄像机类型
const std::string DEVICE_TYPE_IP_CAMERA                     = "131";
// 网络硬盘录像机
const std::string DEVICE_TYPE_NETWORK_VIDEO_RECORDER        = "132";

// 设备类型长度
constexpr uint32_t DEVICE_TYPE_LENGTH                       = 20;

/**
 * @brief 自定义返回值枚举类型
 */
typedef enum class MyStatusType : int
{
    //                                                      失败
    FAILED                                                  = -1,

    //                                                      成功
    SUCCESS                                                 = 0,

} MyStatus_t;

/**
 * @brief 媒体播放方式枚举类型
 */
typedef enum class MyMediaPlayWayType : int
{
    //                                                      播放媒体
    PLAY                                                    = 0,

    //                                                      回放媒体
    PLAYBACK                                                = 1,

} MyMedaPlayWay_t;

/**
 * @brief sip消息uri
 */
typedef struct MySipMessageUriDataType 
{
    //                                                      sip服务ID
    std::string                                             id;

    //                                                      sip服务ip地址
    std::string                                             ipAddr; 

    //                                                      sip服务端口
    uint16_t                                                port;
    
    //                                                      sip服务传输协议类型
    MyTpProto_t                                             proto;                    

} MySipMsgUri_dt; 

/**
 * @brief sip消息contact首部
 */
typedef struct MySipMessageContactHeaderDataType 
{
    //                                                      sip服务ID
    std::string                                             id;

    //                                                      sip服务ip地址
    std::string                                             ipAddr; 

    //                                                      sip服务端口
    uint16_t                                                port;                  

} MySipMsgContactHdr_dt; 

/**
 * @brief sipApp标识
 */
typedef struct MySipAppIdentifyDataType 
{
    //                                                      app ID
    std::string                                             id;

    //                                                      app名称
    std::string                                             name; 
       
    //                                                      app优先级
    pjsip_module_priority                                   priority;                    

} MySipAppIdCfg_dt; 

/**
 * @brief sip上级注册服务信息
 */
typedef struct MySipUpperRegisterServerInfoDataType 
{
    //                                                      sip上级注册服务配置
    MySipRegUpServCfg_dt                                    sipRegUpServCfg;

    //                                                      sip上级注册有效时长
    uint32_t                                                sipRegUpServExpired;

    //                                                      sip上级注册服务最近一次注册时间(格式：yyyy-mm-dd hh:mm:ss)
    std::string                                             sipRegUpServLastRegTime;   

    //                                                      sip上级注册服务保活定时器索引
    uint32_t                                                sipRegUpServKeepAliveIdx;

} MySipUpRegServInfo_dt; 

/**
 * @brief sip下级注册服务信息
 */
typedef struct MySipLowerRegisterServerInfoDataType 
{
    //                                                      sip下级注册服务配置
    MySipRegLowServCfg_dt                                   sipRegLowServCfg;

    //                                                      sip下级注册有效时长
    uint32_t                                                sipRegLowServExpired;

    //                                                      sip下级注册服务最近一次注册时间(格式：yyyy-mm-dd hh:mm:ss)
    std::string                                             sipRegLowServLastRegTime;

} MySipLowRegServInfo_dt; 

/**
 * @brief sip设备目录信息
 */
typedef struct MySipCatalogInfoDataType 
{
    //                                                      上级平台发送请求携带的sn号
    std::string                                             sn;

    //                                                      本级平台设备目录信息，key = device id, value = platform info
    std::map<std::string, MySipCatalogPlatCfg_dt>           sipPlatMap;

    //                                                      本级子平台设备目录信息，key = device id, value = sub platform info
    std::map<std::string, MySipCatalogSubPlatCfg_dt>        sipSubPlatMap;

    //                                                      本级虚拟子平台设备目录信息，key = device id, value = virtual sub platform info
    std::map<std::string, MySipCatalogVirtualSubPlatCfg_dt> sipSubVirtualPlatMap;

    //                                                      本级设备设备目录信息，key = device id, value = device info
    std::map<std::string, MySipCatalogDeviceCfg_dt>         sipDeviceMap;

} MySipCatalogInfo_dt;

/**
 * @brief http请求媒体信息
 */
typedef struct MyHttpReqMediaInfoDataType
{
    //                                                      设备id
    std::string                                             deviceId;

    //                                                      播放方式
    MyMedaPlayWay_t                                         playType;

    //                                                      协议类型
    MyTpProto_t                                             protoType;

} MyHttpReqMediaInfo_dt;

/**
 * @brief sip会话信息
 */
typedef struct MySipSessionInfoDataType
{
    //                                                      设备id
    std::string                                             deviceId;

    //                                                      播放方式
    MyMedaPlayWay_t                                         playType;

    //                                                      协议类型
    MyTpProto_t                                             protoType;

    //                                                      本端服务id
    std::string                                             localServerId;

    //                                                      本端服务ip地址
    std::string                                             localServIpAddr;

    //                                                      本端服务端口
    uint16_t                                                localServPort;

    //                                                      本端sdp
    std::string                                             localSdp;

    //                                                      对端服务id
    std::string                                             remoteServerId;

    //                                                      对端服务ip地址
    std::string                                             remoteServIpAddr;
    
    //                                                      对端服务端口
    uint16_t                                                remoteServPort;

    //                                                      对端sdp
    std::string                                             remoteSdp;

} MySipSessionInfo_dt;


}; // MY_COMMON