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
 * sipApp帮助类
 */
class MySipAppHelper : public MyNonConstructableAndNonCopyable
{
public: 
    /**                         
     * @brief                                           获取sip服务信息
     * @return                                          sip服务信息
     */                     
    static std::string                                  GetSipAppInfo(const MY_COMMON::MySipAppIdCfg_dt& sipAppId);
};

}; // namespace MY_UTILS