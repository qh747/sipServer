#pragma once
#include <string>
#include "common/myDataDef.h"
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

/**
 * sipApp帮助类
 */
class MySipAppHelper : public MyNonConstructableAndNonCopyable
{
public: 
    /**                         
     * @brief                       获取sip app信息
     * @return                      获取结果
     * @param sipAppId              sip app id
     * @param info                  返回的sip app信息
     */                         
    static MY_COMMON::MyStatus_t    GetSipAppInfo(const MY_COMMON::MySipAppIdCfg_dt& sipAppId, std::string& info);

    /**                         
     * @brief                       获取sip app线程池名称
     * @return                      获取结果
     * @param sipAppId              sip app id
     * @param name                  返回的sip app线程池名称
     */                     
    static MY_COMMON::MyStatus_t    GetSipAppThdPoolName(const MY_COMMON::MySipAppIdCfg_dt& sipAppId, std::string& name);
};

}; // namespace MY_UTILS