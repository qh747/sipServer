#include <SimpleIni.h>
#include <Poller/EventPoller.h>
#include "utils/myJsonHelper.h"
#include "envir/mySystemConfg.h"
using namespace MY_COMMON;
using MY_UTILS::MyJsonHelper;

namespace MY_ENVIR {

boost::shared_mutex                 MySystemConfig::CfgMutex;
MyServLogCfg_dt                     MySystemConfig::ServLogCfg;
MySipStackCfg_dt                    MySystemConfig::SipStackCfg;
MySipEvThdMemCfg_dt                 MySystemConfig::SipEvThdMemCfg;
MySipServAddrMap                    MySystemConfig::SipServAddrCfgMap;
MySipRegServCfgMap                  MySystemConfig::SipRegServCfgMap;

MyStatus_t MySystemConfig::Init(const std::string& path)
{
    boost::unique_lock<boost::shared_mutex> lock(CfgMutex);

    toolkit::EventPollerPool::setPoolSize(4);

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

    // 读取sip服务地址文件配置
    std::string sipServAddrfilePath        = SysCfgIni.GetValue("sipServerAddr", "sipServerAddrFilePath", ".");
    std::string sipServAddrfileName        = SysCfgIni.GetValue("sipServerAddr", "sipServerAddrFileName", "servAddr.json");
    sipServAddrfileName                    = sipServAddrfilePath + std::string("/") + sipServAddrfileName;

    // 解析sip服务地址文件
    if (MyStatus_t::SUCCESS != MyJsonHelper::ParseSipServAddrJsonFile(sipServAddrfileName, SipServAddrCfgMap)) {
        return MyStatus_t::FAILED;
    }

    // 读取sip服务注册文件配置
    std::string sipServRegfilePath         = SysCfgIni.GetValue("sipServerRegist", "sipServerRegistFilePath", ".");
    std::string sipServRegfileName         = SysCfgIni.GetValue("sipServerRegist", "sipServerRegistFileName", "servReg.json");
    sipServRegfileName                     = sipServRegfilePath + std::string("/") + sipServRegfileName;

    // 解析sip服务注册文件
    if (MyStatus_t::SUCCESS != MyJsonHelper::ParseSipServRegJsonFile(sipServRegfileName, SipRegServCfgMap)) {
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyServLogCfg_dt MySystemConfig::GetServerLogCfg()
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);
    return ServLogCfg;
}

MySipStackCfg_dt MySystemConfig::GetSipStackCfg()
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);
    return SipStackCfg;
}

MySipEvThdMemCfg_dt MySystemConfig::GetSipEvThdMemCfg()
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);
    return SipEvThdMemCfg;
}

MySipServAddrMap MySystemConfig::GetSipServAddrCfgMap()
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);
    return SipServAddrCfgMap;
}

MySipServAddrCfg_dt MySystemConfig::GetSipServAddrCfg(const std::string& localServId)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    auto iter = SipServAddrCfgMap.find(localServId);
    if (SipServAddrCfgMap.end() != iter) {
        return iter->second;
    }
    return MySipServAddrCfg_dt();
}

MySipRegUpServCfgMap MySystemConfig::GetSipUpRegServCfgMap(const std::string& localServId)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    auto iter = SipRegServCfgMap.find(localServId);
    if (SipRegServCfgMap.end() != iter) {
        return iter->second.upRegSipServMap;
    }
    return MySipRegUpServCfgMap();
}

MySipRegLowServCfgMap MySystemConfig::GetSipLowRegServCfgMap(const std::string& localServId)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    auto iter = SipRegServCfgMap.find(localServId);
    if (SipRegServCfgMap.end() != iter) {
        return iter->second.lowRegSipServMap;
    }
    return MySipRegLowServCfgMap();
}

}; // namespace MY_ENVIR