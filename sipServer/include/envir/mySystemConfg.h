#pragma once
#include <map>
#include <string>
#include "common/myDataDef.h"
#include "common/myConfigDef.h"
#include "utils/myBaseHelper.h"
#include "utils/myJsonHelper.h"

namespace MY_ENVIR {

/**
 * 系统配置类
 */
class MySystemConfig : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    /**
     * @brief                                                           初始化
     * @return                                                          MyStatus_t状态码
     * @param path                                                      配置文件路径
     */                                             
    static MY_COMMON::MyStatus_t                                        Init(const std::string& path);

public:                     
    //                                                                  获取服务端日志配置
    inline static const MY_COMMON::MyServLogCfg_dt&                     GetServerLogCfg()       { return ServLogCfg; }

    //                                                                  获取SIP协议栈配置
    inline static const MY_COMMON::MySipStackCfg_dt&                    GetSipStackCfg()        { return SipStackCfg; }

    //                                                                  获取sips事件线程内存配置
    inline static const MY_COMMON::MySipEvThdMemCfg_dt&                 GetSipEvThdMemCfg()     { return SipEvThdMemCfg; }

    //                                                                  获取sip服务端地址配置
    inline static const MY_COMMON::MySipServAddrCfg_dt&                 GetSipServAddrCfg()     { return SipServAddrCfg; }

    //                                                                  获取SIP上级注册服务配置
    inline static const MY_UTILS::MyJsonHelper::SipRegUpServJsonMap&    GetSipUpRegServCfgMap() { return SipUpRegServCfgMap; }

    //                                                                  获取SIP下级注册服务配置
    inline static const MY_UTILS::MyJsonHelper::SipRegLowServJsonMap&   GetSipLowRegServCfgMap() { return SipLowRegServCfgMap; }

private:
    //                                                                  服务端日志配置
    static MY_COMMON::MyServLogCfg_dt                                   ServLogCfg;
            
    //                                                                  SIP协议栈配置
    static MY_COMMON::MySipStackCfg_dt                                  SipStackCfg;
            
    //                                                                  sips事件线程内存配置
    static MY_COMMON::MySipEvThdMemCfg_dt                               SipEvThdMemCfg;
            
    //                                                                  sip服务端地址配置
    static MY_COMMON::MySipServAddrCfg_dt                               SipServAddrCfg;
            
    //                                                                  SIP上级注册服务配置
    static MY_UTILS::MyJsonHelper::SipRegUpServJsonMap                  SipUpRegServCfgMap; 
            
    //                                                                  SIP下级注册服务配置
    static MY_UTILS::MyJsonHelper::SipRegLowServJsonMap                 SipLowRegServCfgMap;
};

}; // namespace MY_ENVIR