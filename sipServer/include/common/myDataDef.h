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
 * @brief sipApp标识
 */
typedef struct MySipAppIdentifyDataType 
{
    //                              app ID
    std::string                     id;

    //                              app名称
    std::string                     name;    

    //                              服务端端口号
    pjsip_module_priority           priority;                    

} MySipAppIdCfg_dt; 

/**
 * @brief sip上级注册服务信息
 */
typedef struct MySipUpperRegisterServerInfoDataType 
{
    //                              app ID
    MySipRegUpServCfg_dt            sipRegUpServCfg;

    //                              app名称
    pjsip_regc*                     sipRegUpServPtr;                  

} MySipUpRegServInfo_dt; 

}; // MY_COMMON