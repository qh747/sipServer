#pragma once
#include <string>
#include "common/myDataDef.h"
#include "common/myConfigDef.h"
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

/**
 * sip服务帮助类
 */
class MySipServerHelper : public MyNonConstructableAndNonCopyable
{
public: 
    /**                         
     * @brief                       获取sip服务信息
     * @return                      获取结果, success-0, fail-非0
     * @param sipServAddrCfg        sip服务配置
     * @param info                  sip服务信息
     */                     
    static MY_COMMON::MyStatus_t    GetSipServInfo(const MY_COMMON::MySipServAddrCfg_dt& sipServAddrCfg, std::string& info);

    /**
     * @brief                       获取sip上级注册服务信息
     * @return                      获取结果, success-0, fail-非0
     * @param sipUpRegServCfg       sip上级注册服务配置
     * @param info                  sip上级注册服务信息
     */
    static MY_COMMON::MyStatus_t    GetSipUpRegServInfo(const MY_COMMON::MySipRegUpServCfg_dt& sipUpRegServCfg, std::string& info);

    /**
     * @brief                       获取sip下级注册服务信息
     * @return                      获取结果, success-0, fail-非0
     * @param sipLowRegServCfg      sip下级注册服务配置
     * @param info                  sip下级注册服务信息
     */
    static MY_COMMON::MyStatus_t    GetSipLowRegServInfo(const MY_COMMON::MySipRegLowServCfg_dt& sipLowRegServCfg, std::string& info);

    /**                         
     * @brief                       获取sip服务线程池名称
     * @return                      获取结果, success-0, fail-非0
     * @param sipServAddrCfg        sip服务配置
     * @param name                  sip服务线程池名称
     */                     
    static MY_COMMON::MyStatus_t    GetSipServThdPoolName(const MY_COMMON::MySipServAddrCfg_dt& sipServAddrCfg, std::string& name);

    /**                         
     * @brief                       获取sip服务事件线程名称
     * @return                      获取结果, success-0, fail-非0
     * @param sipServAddrCfg        sip服务配置
     * @param name                  sip服务事件线程名称
     */                     
    static MY_COMMON::MyStatus_t    GetSipServEvThdName(const MY_COMMON::MySipServAddrCfg_dt& sipServAddrCfg, std::string& info);
};

}; // namespace MY_VIEW