#pragma once
#include <string>
#include <boost/thread/shared_mutex.hpp>
#include "common/myTypeDef.h"
#include "utils/myBaseHelper.h"

namespace MY_ENVIR {

/**
 * 系统配置类
 */
class MySystemConfig : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    /**
     * @brief                                               初始化
     * @return                                              MyStatus_t状态码
     * @param path                                          配置文件路径
     */                                                 
    static MY_COMMON::MyStatus_t                            Init(const std::string& path);

public:                     
    //                                                      获取服务端日志配置
    static MY_COMMON::MyServLogCfg_dt                       GetServerLogCfg();

    //                                                      获取SIP协议栈配置
    static MY_COMMON::MySipStackCfg_dt                      GetSipStackCfg();

    //                                                      获取sips事件线程内存配置
    static MY_COMMON::MySipEvThdMemCfg_dt                   GetSipEvThdMemCfg();

    //                                                      获取所有sip服务端地址配置
    static MY_COMMON::MySipServAddrMap                      GetSipServAddrCfgMap();

    //                                                      获取sip服务端地址配置
    static MY_COMMON::MySipServAddrCfg_dt                   GetSipServAddrCfg(const std::string& localServId);

    //                                                      获取SIP上级注册服务配置
    static MY_COMMON::MySipRegUpServCfgMap                  GetSipUpRegServCfgMap(const std::string& localServId);

    //                                                      获取SIP下级注册服务配置
    static MY_COMMON::MySipRegLowServCfgMap                 GetSipLowRegServCfgMap(const std::string& localServId);

    //                                                      获取SIP设备目录平台配置
    static MY_COMMON::MySipCatalogPlatCfgMap                GetSipCatalogPlatCfgMap(const std::string& localServId);

    //                                                      获取SIP设备目录子平台配置
    static MY_COMMON::MySipCatalogSubPlatCfgMap             GetSipCatalogSubPlatCfgMap(const std::string& localServId);
        
    //                                                      获取SIP设备目录子平台虚拟平台配置
    static MY_COMMON::MySipCatalogSubVirtualPlatCfgMap      GetSipCatalogSubVirtualPlatCfgMap(const std::string& localServId);
        
    //                                                      SIP设备目录设备配置
    static MY_COMMON::MySipCatalogDeviceCfgMap              GetSipCatalogDeviceCfgMap(const std::string& localServId);

private:
    //                                                      配置文件读写锁
    static boost::shared_mutex                              CfgMutex;

    //                                                      服务端日志配置
    static MY_COMMON::MyServLogCfg_dt                       ServLogCfg;

    //                                                      SIP协议栈配置
    static MY_COMMON::MySipStackCfg_dt                      SipStackCfg;

    //                                                      sips事件线程内存配置
    static MY_COMMON::MySipEvThdMemCfg_dt                   SipEvThdMemCfg;

    //                                                      sip服务端地址配置
    static MY_COMMON::MySipServAddrMap                      SipServAddrCfgMap;

    //                                                      SIP注册服务配置
    static MY_COMMON::MySipRegServCfgMap                    SipRegServCfgMap;

    //                                                      SIP设备目录平台配置
    static MY_COMMON::MySipServCatalogPlatCfgMap            SipCatalogPlatCfgMap;

    //                                                      SIP设备目录子平台配置
    static MY_COMMON::MySipServCatalogSubPlatCfgMap         SipCatalogSubPlatCfgMap;

    //                                                      SIP设备目录子平台虚拟平台配置
    static MY_COMMON::MySipServCatalogSubVirtualPlatCfgMap  SipCatalogSubVirtualPlatCfgMap;

    //                                                      SIP设备目录设备配置
    static MY_COMMON::MySipServCatalogDeviceCfgMap          SipCatalogDeviceCfgMap;
};

}; // namespace MY_ENVIR