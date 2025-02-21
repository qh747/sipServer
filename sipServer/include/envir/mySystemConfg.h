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
    static MY_COMMON::MyStatus_t                            GetServerLogCfg(MY_COMMON::MyServLogCfg_dt& cfg);

    //                                                      获取服务端线程池配置
    static MY_COMMON::MyStatus_t                            GetServerThdPoolCfg(MY_COMMON::MyServThdPoolCfg_dt& cfg);

    //                                                      获取SIP协议栈配置
    static MY_COMMON::MyStatus_t                            GetSipStackCfg(MY_COMMON::MySipStackCfg_dt& cfg);

    //                                                      获取SIP服务注册有效时间（秒）
    static MY_COMMON::MyStatus_t                            GetSipServRegExpiredTimeInterval(unsigned int& timeInterval);

    //                                                      sip本级服务判断上下级服务超时时间间隔（秒）
    static MY_COMMON::MyStatus_t                            GetSipServRegistJugdeTimeInterval(float& timeInterval);

    //                                                      获取sips事件线程内存配置
    static MY_COMMON::MyStatus_t                            GetSipEvThdMemCfg(MY_COMMON::MySipEvThdMemCfg_dt& cfg);

    //                                                      获取所有sip服务端地址配置
    static MY_COMMON::MyStatus_t                            GetSipServAddrCfgMap(MY_COMMON::MySipServAddrMap& cfg);

    //                                                      获取sip服务端地址配置
    static MY_COMMON::MyStatus_t                            GetSipServAddrCfg(const std::string&              localServId, 
                                                                              MY_COMMON::MySipServAddrCfg_dt& cfg);
                        
    //                                                      获取SIP上级注册服务配置                 
    static MY_COMMON::MyStatus_t                            GetSipUpRegServCfgMap(const std::string&               localServId, 
                                                                                  MY_COMMON::MySipRegUpServCfgMap& cfg);
                        
    //                                                      获取SIP下级注册服务配置                 
    static MY_COMMON::MyStatus_t                            GetSipLowRegServCfgMap(const std::string&                localServId, 
                                                                                   MY_COMMON::MySipRegLowServCfgMap& cfg);

    //                                                      获取SIP设备目录平台配置
    static MY_COMMON::MyStatus_t                            GetSipCatalogPlatCfgMap(const std::string&                 localServId, 
                                                                                    MY_COMMON::MySipCatalogPlatCfgMap& cfg);

    //                                                      获取SIP设备目录子平台配置
    static MY_COMMON::MyStatus_t                            GetSipCatalogSubPlatCfgMap(const std::string&                    localServId, 
                                                                                       MY_COMMON::MySipCatalogSubPlatCfgMap& cfg);
        
    //                                                      获取SIP设备目录子平台虚拟平台配置
    static MY_COMMON::MyStatus_t                            GetSipCatalogSubVirtualPlatCfgMap(const std::string&                           localServId, 
                                                                                              MY_COMMON::MySipCatalogSubVirtualPlatCfgMap& cfg);
        
    //                                                      SIP设备目录设备配置
    static MY_COMMON::MyStatus_t                            GetSipCatalogDeviceCfgMap(const std::string&                   localServId, 
                                                                                      MY_COMMON::MySipCatalogDeviceCfgMap& cfg);

private:
    //                                                      配置文件读写锁
    static boost::shared_mutex                              CfgMutex;

    //                                                      服务端日志配置
    static MY_COMMON::MyServLogCfg_dt                       ServLogCfg;

    //                                                      服务端线程池配置
    static MY_COMMON::MyServThdPoolCfg_dt                   ServThdPoolCfg;

    //                                                      SIP协议栈配置
    static MY_COMMON::MySipStackCfg_dt                      SipStackCfg;

    //                                                      SIP定时器配置
    static MY_COMMON::MySipTimerCfg_dt                      SipTimerCfg;

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