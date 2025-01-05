#pragma once
#include <string>
#include <SimpleIni.h>
#include "utils/myHelper.h"
#include "common/myDataDef.h"
using MY_COMMON::MyStatus_t;
using MY_COMMON::MyServerAddrConfig_dt;
using MY_COMMON::MyServerLogConfig_dt;
using MY_COMMON::MyServerThreadConfig_dt;

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

private:
    static CSimpleIniA                              SystemCfgIni;
    static MyServerAddrConfig_dt                    ServerAddrConfig;
    static MyServerLogConfig_dt                     ServerLogConfig;
    static MyServerThreadConfig_dt                  ServerThreadConfig;
};
}; // namespace MY_ENVIR