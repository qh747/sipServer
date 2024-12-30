#include "envir/mySystemConfg.h"

namespace MY_ENVIR {
CSimpleIniA         MySystemConfig::SystemCfgIni;
MyServerConfig_dt   MySystemConfig::ServerConfig;

MyStatus_t MySystemConfig::load(const std::string& path)
{
    // 加载INI文件
    if (0 < SystemCfgIni.LoadFile(path.c_str())) {
        return MyStatus_t::FAILED;
    }

    // 读取值
    ServerConfig.port   = SystemCfgIni.GetLongValue("sipServer", "serverPort", 5060);
    ServerConfig.ipAddr = SystemCfgIni.GetValue("sipServer", "serverIp", "127.0.0.1");
    ServerConfig.name   = SystemCfgIni.GetValue("sipServer", "serverName", "mySipServer");
    ServerConfig.domain = SystemCfgIni.GetValue("sipServer", "serverDomain", "mySipServer.com");
    
    return MyStatus_t::SUCCESS;
}
}; // namespace MY_ENVIR