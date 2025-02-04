#pragma once
#include <string>
#include <cstdint>
#include <cstdbool>
#include <pjsip.h>
#include <pjsip_ua.h>
#include "common/myConfigDef.h"

namespace MY_COMMON {

/**
 * @brief 自定义返回值枚举类型
 */
typedef enum class MyStatusType : int
{
    //                              成功
    SUCCESS                         = 0,  

    //                              失败
    FAILED                          = -1,           

} MyStatus_t;

/**
 * @brief sip消息uri
 */
typedef struct MySipMessageUriDataType 
{
    //                              sip服务ID
    std::string                     id;

    //                              sip服务ip地址
    std::string                     ipAddr; 

    //                              sip服务端口
    uint16_t                        port;

    //                              sip服务传输协议类型
    MyTpProto_t                     proto;                    

} MySipMsgUri_dt; 

/**
 * @brief sip消息contact首部
 */
typedef struct MySipMessageContactHeaderDataType 
{
    //                              sip服务ID
    std::string                     id;

    //                              sip服务ip地址
    std::string                     ipAddr; 

    //                              sip服务端口
    uint16_t                        port;                  

} MySipMsgContactHdr_dt; 

/**
 * @brief sipApp标识
 */
typedef struct MySipAppIdentifyDataType 
{
    //                              app ID
    std::string                     id;

    //                              app名称
    std::string                     name;    

    //                              app优先级
    pjsip_module_priority           priority;                    

} MySipAppIdCfg_dt; 

/**
 * @brief sip上级注册服务信息
 */
typedef struct MySipUpperRegisterServerInfoDataType 
{
    //                              sip上级注册服务配置
    MySipRegUpServCfg_dt            sipRegUpServCfg;

    //                              sip上级注册服务句柄
    pjsip_regc*                     sipRegUpServPtr;                  

} MySipUpRegServInfo_dt; 

/**
 * @brief sip下级注册服务信息
 */
typedef struct MySipLowerRegisterServerInfoDataType 
{
    //                              sip下级注册服务配置
    MySipRegLowServCfg_dt           sipRegLowServCfg;

    //                              sip下级注册服务最近一次注册时间(格式：yyyy-mm-dd hh:mm:ss)
    std::string                     sipRegLowServLastRegTime;

    //                              sip下级注册服务注册状态
    MyStatus_t                      sipRegLowServRegStatus;

} MySipLowRegServInfo_dt; 

}; // MY_COMMON