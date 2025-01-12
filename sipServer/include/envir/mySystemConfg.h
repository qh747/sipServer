#pragma once
#include <string>
#include <SimpleIni.h>
#include "utils/myBaseHelper.h"
#include "common/myDataDef.h"
using MY_COMMON::MyStatus_t;
using MY_COMMON::MyServerAddrConfig_dt;
using MY_COMMON::MyServerLogConfig_dt;
using MY_COMMON::MyServerThreadConfig_dt;
using MY_COMMON::MySipStackConfig_dt;

namespace MY_ENVIR {

/**
 * 系统配置类
 */
class MySystemConfig : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    /**
     * @brief                                       加载系统配置
     * @return                                      MyStatus_t状态码
     * @param path                                  配置文件路径
     */                         
    static MyStatus_t                               load(const std::string& path);

public:     
    //                                              获取服务端地址配置
    inline static const MyServerAddrConfig_dt&      GetServerAddrConfig() { return ServerAddrConfig; }

    //                                              获取服务端日志配置
    inline static const MyServerLogConfig_dt&       GetServerLogConfig() { return ServerLogConfig; }

    //                                              获取服务端线程池配置
    inline static const MyServerThreadConfig_dt&    GetServerThreadConfig() { return ServerThreadConfig; }

    //                                              获取SIP协议栈配置
    inline static const MySipStackConfig_dt&        GetSipStackConfig() { return SipStackConfig; }

    //                                              获取SIP注册文件路径
    inline static const std::string&                GetServerRegisterFile() { return ServerRegisterFile; }

private:
    //                                              ini配置文件处理对象
    static CSimpleIniA                              SystemCfgIni;

    //                                              服务端地址配置
    static MyServerAddrConfig_dt                    ServerAddrConfig;

    //                                              服务端日志配置
    static MyServerLogConfig_dt                     ServerLogConfig;
    
    //                                              服务端线程池配置
    static MyServerThreadConfig_dt                  ServerThreadConfig;

    //                                              SIP协议栈配置
    static MySipStackConfig_dt                      SipStackConfig;

    //                                              SIP注册文件路径
    static std::string                              ServerRegisterFile; 
};

}; // namespace MY_ENVIR