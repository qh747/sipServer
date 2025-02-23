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
    //                                              catalog请求消息
    SIP_MSG_CATALOG_QUERY                           = 1, 
    //                                              catalog响应消息
    SIP_MSG_CATALOG_RESPONSE                        = 2,

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
typedef struct MySipCatalogRequestMessageBodyDataType 
{
    //                                              消息类型cmdType
    std::string                                     cmdType;
    //                                              下级服务id DeviceID
    std::string                                     deviceId;
    //                                              消息索引SN
    std::string                                     sn;         

} MySipCatalogReqMsgBody_dt; 

/**
 * @brief sip catalog响应消息内容
 */
typedef struct MySipCatalogResponseMessageBodyDataType 
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
    //                                              子平台配置信息
    MySipCatalogSubPlatCfgMap                       subPlatCfgMap;
    //                                              虚拟子平台配置信息
    MySipCatalogSubVirtualPlatCfgMap                subVirtualPlatCfgMap;
    //                                              设备配置信息
    MySipCatalogDeviceCfgMap                        deviceCfgMap;

} MySipCatalogRespMsgBody_dt;

}; // namespace MY_COMMON