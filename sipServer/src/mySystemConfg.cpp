#include <SimpleIni.h>
#include <Poller/EventPoller.h>
#include "utils/myJsonHelper.h"
#include "envir/mySystemConfg.h"
using namespace MY_COMMON;
using MY_UTILS::MyJsonHelper;

namespace MY_ENVIR {

boost::shared_mutex                     MySystemConfig::CfgMutex;
MyServLogCfg_dt                         MySystemConfig::ServLogCfg;
MyServThdPoolCfg_dt                     MySystemConfig::ServThdPoolCfg;
MySipStackCfg_dt                        MySystemConfig::SipStackCfg;
MySipTimerCfg_dt                        MySystemConfig::SipTimerCfg;
MySipEvThdMemCfg_dt                     MySystemConfig::SipEvThdMemCfg;
MySipServAddrMap                        MySystemConfig::SipServAddrCfgMap;
MySipRegServCfgMap                      MySystemConfig::SipRegServCfgMap;
MySipServCatalogPlatCfgMap              MySystemConfig::SipCatalogPlatCfgMap;
MySipServCatalogSubPlatCfgMap           MySystemConfig::SipCatalogSubPlatCfgMap;
MySipServCatalogSubVirtualPlatCfgMap    MySystemConfig::SipCatalogSubVirtualPlatCfgMap;
MySipServCatalogDeviceCfgMap            MySystemConfig::SipCatalogDeviceCfgMap;

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
    ServLogCfg.logLevel                     = SysCfgIni.GetValue("log", "logLevel", "info");
    ServLogCfg.logPath                      = SysCfgIni.GetValue("log", "logPath", "./log");
    ServLogCfg.enableOutputToConsole        = SysCfgIni.GetBoolValue("log", "enableOutputToConsole", true);
    ServLogCfg.enableUseDiffColorDisplay    = SysCfgIni.GetBoolValue("log", "enableUseDiffColorDisplay", true);

    // 读取服务线程池配置
    ServThdPoolCfg.threadNum                = SysCfgIni.GetLongValue("threadPool", "threadNum", 4);
    ServThdPoolCfg.threadPriority           = SysCfgIni.GetLongValue("threadPool", "threadPriority", 4);
    ServThdPoolCfg.enableAutoRun            = SysCfgIni.GetBoolValue("threadPool", "threadAutoRun", true);
    ServThdPoolCfg.enableAffinity           = SysCfgIni.GetBoolValue("threadPool", "threadAffinity", false);

    // 读取SIP协议栈配置
    SipStackCfg.sipStackSize                = SysCfgIni.GetLongValue("sipStack", "sipStackSize", 1024*256);
    SipStackCfg.sipStackName                = SysCfgIni.GetValue("sipStack", "sipStackName", "sipStack");

    // 读取SIP定时器配置
    SipTimerCfg.sipServRegExpiredTimeInterval  = SysCfgIni.GetLongValue("sipTimer", "sipServerRegistExpired", 3600);
    SipTimerCfg.sipServRegistJugdeTimeInterval = SysCfgIni.GetLongValue("sipTimer", "sipServerRegistJudgeInterval", 30);
    
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

    // 读取sip服务设备目录文件配置
    std::string sipServCatalogfilePath     = SysCfgIni.GetValue("sipServerCatalog", "sipServerCatalogFilePath", ".");
    std::string sipServCatalogfileName     = SysCfgIni.GetValue("sipServerCatalog", "sipServerCatalogFileName", "servCatalog.json");
    
    if (!sipServCatalogfilePath.empty() && !sipServCatalogfileName.empty()) {
        sipServCatalogfileName = sipServCatalogfilePath + std::string("/") + sipServCatalogfileName;

        // 解析sip服务设备目录文件
        if (MyStatus_t::SUCCESS != MyJsonHelper::ParseSipServCatalogJsonFile(sipServCatalogfileName, 
                                                                          SipCatalogPlatCfgMap,
                                                                       SipCatalogSubPlatCfgMap,
                                                                SipCatalogSubVirtualPlatCfgMap,
                                                                        SipCatalogDeviceCfgMap)) {
            return MyStatus_t::FAILED;
        }
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemConfig::GetServerLogCfg(MyServLogCfg_dt& cfg)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    cfg = ServLogCfg;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemConfig::GetServerThdPoolCfg(MY_COMMON::MyServThdPoolCfg_dt& cfg)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    cfg = ServThdPoolCfg;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemConfig::GetSipStackCfg(MySipStackCfg_dt& cfg)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    cfg = SipStackCfg;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemConfig::GetSipServRegExpiredTimeInterval(unsigned int& timeInterval)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    timeInterval = SipTimerCfg.sipServRegExpiredTimeInterval;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemConfig::GetSipServRegistJugdeTimeInterval(float& timeInterval)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    timeInterval = SipTimerCfg.sipServRegistJugdeTimeInterval;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemConfig::GetSipEvThdMemCfg(MySipEvThdMemCfg_dt& cfg)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    cfg = SipEvThdMemCfg;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemConfig::GetSipServAddrCfgMap(MySipServAddrMap& cfg)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    cfg = SipServAddrCfgMap;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemConfig::GetSipServAddrCfg(const std::string& localServId, MySipServAddrCfg_dt& cfg)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    auto iter = SipServAddrCfgMap.find(localServId);
    if (SipServAddrCfgMap.end() == iter) {
        return MyStatus_t::FAILED;
    }
    cfg = iter->second;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemConfig::GetSipUpRegServCfgMap(const std::string& localServId, MySipRegUpServCfgMap& cfg)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    auto iter = SipRegServCfgMap.find(localServId);
    if (SipRegServCfgMap.end() == iter) {
        return MyStatus_t::FAILED;
    }
    cfg = iter->second.upRegSipServMap;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemConfig::GetSipLowRegServCfgMap(const std::string& localServId, MySipRegLowServCfgMap& cfg)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    auto iter = SipRegServCfgMap.find(localServId);
    if (SipRegServCfgMap.end() == iter) {
        return MyStatus_t::FAILED;
    }
    cfg = iter->second.lowRegSipServMap;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemConfig::GetSipCatalogPlatCfgMap(const std::string& localServId, MySipCatalogPlatCfgMap& cfg)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    auto iter = SipCatalogPlatCfgMap.find(localServId);
    if (SipCatalogPlatCfgMap.end() == iter) {
        return MyStatus_t::FAILED;
    }
    cfg = iter->second;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemConfig::GetSipCatalogSubPlatCfgMap(const std::string& localServId, MySipCatalogSubPlatCfgMap& cfg)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    auto iter = SipCatalogSubPlatCfgMap.find(localServId);
    if (SipCatalogSubPlatCfgMap.end() == iter) {
        return MyStatus_t::FAILED;
    }
    cfg = iter->second;
    return MyStatus_t::SUCCESS;
}
                       
MyStatus_t MySystemConfig::GetSipCatalogSubVirtualPlatCfgMap(const std::string& localServId, MySipCatalogSubVirtualPlatCfgMap& cfg)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    auto iter = SipCatalogSubVirtualPlatCfgMap.find(localServId);
    if (SipCatalogSubVirtualPlatCfgMap.end() == iter) {
        return MyStatus_t::FAILED;
    }
    cfg = iter->second;
    return MyStatus_t::SUCCESS;
}
                              
MyStatus_t MySystemConfig::GetSipCatalogDeviceCfgMap(const std::string& localServId, MySipCatalogDeviceCfgMap& cfg)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    auto iter = SipCatalogDeviceCfgMap.find(localServId);
    if (SipCatalogDeviceCfgMap.end() == iter) {
        return MyStatus_t::FAILED;
    }
    cfg = iter->second;
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_ENVIR