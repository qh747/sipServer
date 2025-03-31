#pragma once
#include <string>
#include "common/myDataDef.h"
#include "common/myConfigDef.h"
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

/**
 * 服务帮助类
 */
class MyServerHelper : public MyNonConstructableAndNonCopyable
{
public: 
    /**                         
     * @brief                       获取sip服务信息
     * @return                      获取结果, success-0, fail-非0
     * @param sipServAddrCfg        sip服务配置
     * @param info                  sip服务信息
     */                     
    static MY_COMMON::MyStatus_t    PrintSipServInfo(const MY_COMMON::MySipServAddrCfg_dt& sipServAddrCfg, std::string& info);

    /**                         
     * @brief                       获取sip注册服务信息
     * @return                      获取结果, success-0, fail-非0
     * @param sipRegServAddrCfg     sip注册服务配置
     * @param info                  sip注册服务信息
     */                     
    static MY_COMMON::MyStatus_t    PrintSipRegServInfo(const MY_COMMON::MySipServRegAddrCfg_dt& sipRegServAddrCfg, std::string& info);

    /**
     * @brief                       获取sip上级注册服务信息
     * @return                      获取结果, success-0, fail-非0
     * @param sipUpRegServCfg       sip上级注册服务配置
     * @param info                  sip上级注册服务信息
     */
    static MY_COMMON::MyStatus_t    PrintSipUpRegServInfo(const MY_COMMON::MySipRegUpServCfg_dt& sipUpRegServCfg, std::string& info);

    /**
     * @brief                       获取sip下级注册服务信息
     * @return                      获取结果, success-0, fail-非0
     * @param sipLowRegServCfg      sip下级注册服务配置
     * @param info                  sip下级注册服务信息
     */
    static MY_COMMON::MyStatus_t    PrintSipLowRegServInfo(const MY_COMMON::MySipRegLowServCfg_dt& sipLowRegServCfg, std::string& info);

    /**                         
     * @brief                       获取sip服务线程池名称
     * @return                      获取结果, success-0, fail-非0
     * @param sipServAddrCfg        sip服务配置
     * @param name                  sip服务线程池名称
     */                     
    static MY_COMMON::MyStatus_t    PrintSipServThdPoolName(const MY_COMMON::MySipServAddrCfg_dt& sipServAddrCfg, std::string& name);

public:
    /**                         
     * @brief                       获取http服务信息
     * @return                      获取结果, success-0, fail-非0
     * @param servAddrCfg           http服务配置
     * @param info                  http服务信息
     */           
    static MY_COMMON::MyStatus_t    PrintHttpServInfo(const MY_COMMON::MyHttpServAddrCfg_dt& servAddrCfg, std::string& info);
};

}; // namespace MY_VIEW