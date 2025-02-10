#pragma once
#include <string>
#include "common/myTypeDef.h"
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

/**
 * json帮助类
 */
class MyJsonHelper : public MyNonConstructableAndNonCopyable
{
public:
    /** 
     * @brief                       sip服务地址json文件解析
     * @return                      解析结果，0-success, -1-failed
     * @param filePath              json文件路径
     * @param sipRegServCfg         sip服务地址配置
     */
    static MY_COMMON::MyStatus_t    ParseSipServAddrJsonFile(const std::string& filePath, MY_COMMON::MySipServAddrMap& cfgMap);
    
    /** 
     * @brief                       sip注册服务json文件解析
     * @return                      解析结果，0-success, -1-failed
     * @param filePath              json文件路径
     * @param sipRegServCfg         sip注册服务配置
     */
    static MY_COMMON::MyStatus_t    ParseSipServRegJsonFile(const std::string& filePath, MY_COMMON::MySipRegServCfgMap& cfgMap);
};

}; // namespace MY_UTILS