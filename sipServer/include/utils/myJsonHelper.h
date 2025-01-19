#pragma once
#include <map>
#include <string>
#include "common/myDataDef.h"
#include "common/myConfigDef.h"
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

/**
 * json帮助类
 */
class MyJsonHelper : public MyNonConstructableAndNonCopyable
{
public:
    // key = reg up server id, value = reg up server info
    typedef std::map<std::string, MY_COMMON::MySipRegUpServCfg_dt>    SipRegUpServJsonMap;
    // key = reg up server id, value = reg up server info
    typedef std::map<std::string, MY_COMMON::MySipRegLowServCfg_dt>   SipRegLowServJsonMap;

public:
    /** 
     * @brief                       sip注册服务json文件解析
     * @return                      解析结果，0-success, -1-failed
     * @param filePath              json文件路径
     * @param regUpServMap          上级注册服务信息map
     * @param regLowServMap         下级注册服务信息map
     */
    static MY_COMMON::MyStatus_t    ParseSipServRegJsonFile(const std::string& filePath, SipRegUpServJsonMap& regUpServMap, SipRegLowServJsonMap& regLowServMap);
};

}; // namespace MY_UTILS