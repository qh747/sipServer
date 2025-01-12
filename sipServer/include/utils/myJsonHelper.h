#pragma once
#include <map>
#include <string>
#include <json/json.h>
#include "common/myDataDef.h"
#include "utils/myBaseHelper.h"
using MY_COMMON::MyStatus_t;
using MY_COMMON::MySipRegUpServInfo_dt;
using MY_COMMON::MySipRegLowServInfo_dt;

namespace MY_UTILS {

/**
 * json帮助类
 */
class MyJsonHelper : public MyNonConstructableAndNonCopyable
{
public:
    // key = reg up server id, value = reg up server info
    typedef std::map<std::string, MySipRegUpServInfo_dt>    RegUpServJsonMap;
    // key = reg up server id, value = reg up server info
    typedef std::map<std::string, MySipRegLowServInfo_dt>   RegLowServJsonMap;

public:
    /** 
     * @brief                   注册服务json文件解析
     * @return                  解析结果，0-success, -1-failed
     * @param filePath          json文件路径
     * @param regUpServMap      上级注册服务信息map
     * @param regLowServMap     下级注册服务信息map
     */
    static MyStatus_t           ParseServRegJsonFile(const std::string& filePath, RegUpServJsonMap& regUpServMap, RegLowServJsonMap& regLowServMap);
};

}; // namespace MY_UTILS