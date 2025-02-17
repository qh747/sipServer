#pragma once
#include <string>
#include <cstdbool>
#include "common/myTypeDef.h"

namespace MY_COMMON {

/**
 * @brief sip消息枚举类型
 */
typedef enum class MySipMessageBodyType : int
{
    //                                              未定义
    SIP_MSG_UNKNOWN                                 = -1,

    //                                              keepAlive消息
    SIP_MSG_KEEPALIVE                               = 0,  

    //                                              catalog消息
    SIP_MSG_CATALOG                                 = 1,           

} MySipMsgBody_t;

/**
 * @brief sip keepalive消息内容
 */
typedef struct MySipKeepAliveMsgBodyDataType 
{
    //                                              消息类型cmdType
    std::string                                     cmdType;

    //                                              下级服务id DeviceID
    std::string                                     deviceId;

    //                                              消息索引SN
    std::string                                     sn;

    //                                              保活状态Status
    bool                                            status = false;                 

} MySipKeepAliveMsgBody_dt; 

/**
 * @brief sip catalog请求消息内容
 */
typedef struct MySipCatalogReqMsgBodyDataType 
{
    //                                              消息类型cmdType
    std::string                                     cmdType;

    //                                              下级服务id DeviceID
    std::string                                     deviceId;

    //                                              消息索引SN
    std::string                                     sn;         

} MySipCatalogReqMsgBody_dt; 

/**
 * @brief sip catalog平台配置消息内容
 */
typedef struct MySipCatalogPlatformConfigMsgBodyDataType 
{
    //                                              消息类型cmdType
    std::string                                     cmdType;

    //                                              下级服务id DeviceID
    std::string                                     deviceId;

    //                                              消息索引SN
    std::string                                     sn;

    //                                              设备总数
    std::size_t                                     sumNum;

    //                                              设备列表
    std::size_t                                     deviceList;

    //                                              平台配置信息
    MySipCatalogPlatCfgMap                          platCfgMap;

} MySipCatalogPlatCfgMsgBody_dt;

/**
 * @brief sip catalog子平台配置消息内容
 */
typedef struct MySipCatalogSubPlatformConfigMsgBodyDataType 
{
    //                                              消息类型cmdType
    std::string                                     cmdType;

    //                                              下级服务id DeviceID
    std::string                                     deviceId;

    //                                              消息索引SN
    std::string                                     sn;

    //                                              设备总数
    std::size_t                                     sumNum;

    //                                              设备列表
    std::size_t                                     deviceList;

    //                                              子平台配置信息
    MySipCatalogSubPlatCfgMap                       subPlatCfgMap;

} MySipCatalogSubPlatCfgMsgBody_dt; 

/**
 * @brief sip catalog虚拟子平台配置消息内容
 */
typedef struct MySipCatalogSubVirtualPlatformConfigMsgBodyDataType 
{
    //                                              消息类型cmdType
    std::string                                     cmdType;

    //                                              下级服务id DeviceID
    std::string                                     deviceId;

    //                                              消息索引SN
    std::string                                     sn;

    //                                              设备总数
    std::size_t                                     sumNum;

    //                                              设备列表
    std::size_t                                     deviceList;

    //                                              虚拟子平台配置信息
    MySipCatalogSubVirtualPlatCfgMap                subVirtualPlatCfgMap;

} MySipCatalogSubVirtualPlatCfgMsgBody_dt; 

/**
 * @brief sip catalog设备配置消息内容
 */
typedef struct MySipCatalogDeviceConfigMsgBodyDataType 
{
    //                                              消息类型cmdType
    std::string                                     cmdType;

    //                                              下级服务id DeviceID
    std::string                                     deviceId;

    //                                              消息索引SN
    std::string                                     sn;

    //                                              设备总数
    std::size_t                                     sumNum;

    //                                              设备列表
    std::size_t                                     deviceList;

    //                                              设备配置信息
    MySipCatalogDeviceCfgMap                        deviceCfgMap;

} MySipCatalogDeviceCfgMsgBody_dt; 

}; // namespace MY_COMMON