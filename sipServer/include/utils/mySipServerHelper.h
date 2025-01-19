#pragma once
#include <map>
#include <string>
#include <memory>
#include <atomic>
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
     * @brief                                           获取sip服务信息
     * @return                                          sip服务信息
     */                     
    static std::string                                  GetSipServInfo(const MY_COMMON::MySipServAddrCfg_dt& sipServAddrCfg);

    /**
     * @brief                                           获取sip上级注册服务信息
     * @return                                          sip上级注册服务信息
     * @param sipUpRegServCfg                           sip上级注册服务配置
     */
    static std::string                                  GetSipUpRegServInfo(const MY_COMMON::MySipRegUpServCfg_dt& sipUpRegServCfg);

    /**
     * @brief                                           获取sip下级注册服务信息
     * @return                                          sip下级注册服务信息
     * @param sipLowRegServCfg                          sip下级注册服务配置
     */
    static std::string                                  GetSipLowRegServInfo(const MY_COMMON::MySipRegLowServCfg_dt& sipLowRegServCfg);

    /**                         
     * @brief                                           获取sip服务线程池名称
     * @return                                          sip服务线程池名称
     */                     
    static std::string                                  GetSipServThdPoolName(const MY_COMMON::MySipServAddrCfg_dt& sipServAddrCfg);

    /**                         
     * @brief                                           获取sip服务事件线程名称
     * @return                                          sip服务事件线程名称
     */                     
    static std::string                                  GetSipServEvThdName(const MY_COMMON::MySipServAddrCfg_dt& sipServAddrCfg);
};

}; // namespace MY_VIEW