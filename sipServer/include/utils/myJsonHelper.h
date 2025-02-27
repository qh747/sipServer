#pragma once
#include <string>
#include "common/myDataDef.h"
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

/**
 * json帮助类
 */
class MyJsonHelper : public MyNonConstructableAndNonCopyable
{
public:
    /** 
     * @brief                       sip注册json文件解析
     * @return                      解析结果，0-success, -1-failed
     * @param filePath              json文件路径
     * @param regCfg                sip注册配置
     */
    static MY_COMMON::MyStatus_t    ParseSipRegJsonFile(const std::string& filePath, MY_COMMON::MySipRegServCfg_dt& regCfg);

    /** 
     * @brief                       sip服务设备目录json文件解析
     * @return                      解析结果，0-success, -1-failed
     * @param filePath              json文件路径
     * @param catalogCfg            设备目录配置
     */
    static MY_COMMON::MyStatus_t    ParseSipCatalogJsonFile(const std::string& filePath, MY_COMMON::MySipCatalogCfg_dt& catalogCfg);
};

}; // namespace MY_UTILS