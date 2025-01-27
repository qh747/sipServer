#include <SimpleIni.h>
#include "envir/mySystemConfg.h"
using MY_COMMON::MyStatus_t;
using MY_COMMON::MyServLogCfg_dt;    
using MY_COMMON::MySipStackCfg_dt;   
using MY_COMMON::MySipEvThdMemCfg_dt;   
using MY_COMMON::MySipServAddrCfg_dt;
using MY_UTILS::MyJsonHelper;

namespace MY_ENVIR {

MyServLogCfg_dt                     MySystemConfig::ServLogCfg;
MySipStackCfg_dt                    MySystemConfig::SipStackCfg;
MySipEvThdMemCfg_dt                 MySystemConfig::SipEvThdMemCfg;
MySipServAddrCfg_dt                 MySystemConfig::SipServAddrCfg;
MyJsonHelper::SipRegUpServJsonMap   MySystemConfig::SipUpRegServCfgMap;
MyJsonHelper::SipRegLowServJsonMap  MySystemConfig::SipLowRegServCfgMap;

MyStatus_t MySystemConfig::Init(const std::string& path)
{
    // 加载INI文件
    CSimpleIniA SysCfgIni;
    if (path.empty() || (0 < SysCfgIni.LoadFile(path.c_str()))) {
        return MyStatus_t::FAILED;
    }

    // 读取服务端日志配置                 
    ServLogCfg.logLevel                    = SysCfgIni.GetValue("log", "logLevel", "info");
    ServLogCfg.logPath                     = SysCfgIni.GetValue("log", "logPath", "./log");
    ServLogCfg.enableOutputToConsole       = SysCfgIni.GetBoolValue("log", "enableOutputToConsole", true);
    ServLogCfg.enableUseDiffColorDisplay   = SysCfgIni.GetBoolValue("log", "enableUseDiffColorDisplay", true);

    // 读取SIP协议栈配置
    SipStackCfg.sipStackSize               = SysCfgIni.GetLongValue("sipStack", "sipStackSize", 1024*256);
    SipStackCfg.sipStackName               = SysCfgIni.GetValue("sipStack", "sipStackName", "sipStack");

    // 读取事件线程内存配置
    SipEvThdMemCfg.sipEvThdInitSize        = SysCfgIni.GetLongValue("sipEventThread", "sipEventThreadInitSize", 1024*1024*1);
    SipEvThdMemCfg.sipEvThdIncreSize       = SysCfgIni.GetLongValue("sipEventThread", "sipEventThreadIncrementSize", 1024*1024*1);

    // 读取sip服务端地址配置
    SipServAddrCfg.port                    = SysCfgIni.GetLongValue("sipServer", "sipServerPort", 5060);
    SipServAddrCfg.regPort                 = SysCfgIni.GetLongValue("sipServer", "sipServerRegPort", 5061);
    SipServAddrCfg.id                      = SysCfgIni.GetValue("sipServer", "sipServerId", "");
    SipServAddrCfg.ipAddr                  = SysCfgIni.GetValue("sipServer", "sipServerIp", "127.0.0.1");
    SipServAddrCfg.name                    = SysCfgIni.GetValue("sipServer", "sipServerName", "sipServer");
    SipServAddrCfg.domain                  = SysCfgIni.GetValue("sipServer", "sipServerDomain", "sipServer.com");

    // 读取sip服务注册文件配置
    std::string filePath                   = SysCfgIni.GetValue("sipServerRegister", "sipServerRegisterFilePath", ".");
    std::string fileName                   = SysCfgIni.GetValue("sipServerRegister", "sipServerRegisterFileName", "servReg.json");
    fileName                               = filePath + std::string("/") + fileName;

    // 解析sip服务注册文件
    if (MyStatus_t::SUCCESS != MyJsonHelper::ParseSipServRegJsonFile(fileName, SipUpRegServCfgMap, SipLowRegServCfgMap)) {
        return MyStatus_t::FAILED;
    }
    
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_ENVIR