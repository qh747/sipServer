#include "envir/mySystemConfg.h"

namespace MY_ENVIR {
    
CSimpleIniA             MySystemConfig::SystemCfgIni;
MyServerAddrConfig_dt   MySystemConfig::ServerAddrConfig;
MyServerLogConfig_dt    MySystemConfig::ServerLogConfig;
MyServerThreadConfig_dt MySystemConfig::ServerThreadConfig;
MySipStackConfig_dt     MySystemConfig::SipStackConfig;
std::string             MySystemConfig::ServerRegisterFile;

MyStatus_t MySystemConfig::load(const std::string& path)
{
    // 加载INI文件
    if (0 < SystemCfgIni.LoadFile(path.c_str())) {
        return MyStatus_t::FAILED;
    }

    // 读取服务配置
    ServerAddrConfig.port                       = SystemCfgIni.GetLongValue("sipServer", "serverPort", 5060);
    ServerAddrConfig.id                         = SystemCfgIni.GetValue("sipServer", "serverId", "");
    ServerAddrConfig.ipAddr                     = SystemCfgIni.GetValue("sipServer", "serverIp", "127.0.0.1");
    ServerAddrConfig.name                       = SystemCfgIni.GetValue("sipServer", "serverName", "mySipServer");
    ServerAddrConfig.domain                     = SystemCfgIni.GetValue("sipServer", "serverDomain", "mySipServer.com");

    // 读取日志配置                 
    ServerLogConfig.logLevel                    = SystemCfgIni.GetValue("log", "logLevel", "info");
    ServerLogConfig.logPath                     = SystemCfgIni.GetValue("log", "logPath", "./log");
    ServerLogConfig.enableOutputToConsole       = SystemCfgIni.GetBoolValue("log", "enableOutputToConsole", true);
    ServerLogConfig.enableUseDiffColorDisplay   = SystemCfgIni.GetBoolValue("log", "enableUseDiffColorDisplay", true);

    // 读取线程池配置
    ServerThreadConfig.initIhreadCount          = SystemCfgIni.GetLongValue("threadPool", "initThreadCount", 4);

    // 读取SIP协议栈配置
    SipStackConfig.sipStackSize                 = SystemCfgIni.GetLongValue("sipStack", "sipStackSize", 1024*256);
    SipStackConfig.sipStackName                 = SystemCfgIni.GetValue("sipStack", "sipStackName", "mySipStack");

    // 读取服务注册文件配置
    std::string filePath                        = SystemCfgIni.GetValue("sipRegister", "sipRegisterFilePath", ".");
    std::string fileName                        = SystemCfgIni.GetValue("sipRegister", "sipRegisterFileName", "servReg.json");
    ServerRegisterFile                          = filePath + std::string("/") + fileName;
    
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_ENVIR