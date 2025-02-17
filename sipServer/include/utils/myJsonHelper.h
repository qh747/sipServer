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
     * @param cfgMap                sip服务地址配置
     */
    static MY_COMMON::MyStatus_t    ParseSipServAddrJsonFile(const std::string& filePath, MY_COMMON::MySipServAddrMap& cfgMap);
    
    /** 
     * @brief                       sip注册服务json文件解析
     * @return                      解析结果，0-success, -1-failed
     * @param filePath              json文件路径
     * @param cfgMap                sip注册服务配置
     */
    static MY_COMMON::MyStatus_t    ParseSipServRegJsonFile(const std::string& filePath, MY_COMMON::MySipRegServCfgMap& cfgMap);

    /** 
     * @brief                       sip服务设备目录json文件解析
     * @return                      解析结果，0-success, -1-failed
     * @param filePath              json文件路径
     * @param platCfgMap            平台配置
     * @param subPlatCfgMap         子平台配置
     * @param subVirtualPlatCfgMap  虚拟子平台配置
     * @param deviceCfgMap          设备配置
     */
    static MY_COMMON::MyStatus_t    ParseSipServCatalogJsonFile(const std::string&                           filePath, 
                                                            MY_COMMON::MySipServCatalogPlatCfgMap&           platCfgMap,
                                                            MY_COMMON::MySipServCatalogSubPlatCfgMap&        subPlatCfgMap,
                                                            MY_COMMON::MySipServCatalogSubVirtualPlatCfgMap& subVirtualPlatCfgMap,
                                                            MY_COMMON::MySipServCatalogDeviceCfgMap&         deviceCfgMap);
};

}; // namespace MY_UTILS