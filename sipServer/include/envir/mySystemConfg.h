#pragma once
#include <string>
#include <SimpleIni.h>
#include "utils/myHelper.h"
#include "common/myDataDef.h"
using MY_COMMON::MyStatus_t;
using MY_COMMON::MyServerConfig_dt;

namespace MY_ENVIR {
/**
 * 系统配置类
 */
class MySystemConfig : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    /**
     * @brief                               加载系统配置
     * @return                              MyStatus_t状态码
     * @param path                          配置文件路径
     */                 
    static MyStatus_t                       load(const std::string& path);

public:
    //                                      获取服务端配置
    inline static const MyServerConfig_dt&  GetServerConfig() { return ServerConfig; }

private:
    static CSimpleIniA                      SystemCfgIni;
    static MyServerConfig_dt                ServerConfig;
};
}; // namespace MY_ENVIR