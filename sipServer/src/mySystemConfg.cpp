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
MySipServAddrCfg_dt                     MySystemConfig::SipServAddrCfg;
MyHttpServAddrCfg_dt                    MySystemConfig::HttpServAddrCfg;
MySipRegServCfg_dt                      MySystemConfig::SipRegServCfg;
MySipCatalogCfg_dt                      MySystemConfig::SipCatalogCfg;

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
    ServLogCfg.logLevel                         = SysCfgIni.GetValue("log", "logLevel", "info");
    ServLogCfg.logPath                          = SysCfgIni.GetValue("log", "logPath", "./log");
    ServLogCfg.enableOutputToConsole            = SysCfgIni.GetBoolValue("log", "enableOutputToConsole", true);
    ServLogCfg.enableUseDiffColorDisplay        = SysCfgIni.GetBoolValue("log", "enableUseDiffColorDisplay", true);

    // 读取服务线程池配置   
    ServThdPoolCfg.threadNum                    = SysCfgIni.GetLongValue("threadPool", "threadNum", 4);
    ServThdPoolCfg.threadPriority               = SysCfgIni.GetLongValue("threadPool", "threadPriority", 4);
    ServThdPoolCfg.enableAutoRun                = SysCfgIni.GetBoolValue("threadPool", "threadAutoRun", true);
    ServThdPoolCfg.enableAffinity               = SysCfgIni.GetBoolValue("threadPool", "threadAffinity", false);

    // 读取SIP协议栈配置    
    SipStackCfg.sipStackSize                    = SysCfgIni.GetLongValue("sipStack", "sipStackSize", 1024*256);
    SipStackCfg.sipStackName                    = SysCfgIni.GetValue("sipStack", "sipStackName", "sipStack");

    // 读取SIP定时器配置
    SipTimerCfg.sipServRegExpiredTimeInterval   = SysCfgIni.GetLongValue("sipTimer", "sipServerRegistExpired", 3600);
    SipTimerCfg.sipServRegistJugdeTimeInterval  = SysCfgIni.GetLongValue("sipTimer", "sipServerRegistJudgeInterval", 30);
    
    // 读取事件线程内存配置
    SipEvThdMemCfg.sipEvThdInitSize             = SysCfgIni.GetLongValue("sipEventThread", "sipEventThreadInitSize", 1024*1024*1);
    SipEvThdMemCfg.sipEvThdIncreSize            = SysCfgIni.GetLongValue("sipEventThread", "sipEventThreadIncrementSize", 1024*1024*1);

    // 读取sip服务地址配置
    SipServAddrCfg.id                           = SysCfgIni.GetValue("sipServer", "id", "10000100001000010000");
    SipServAddrCfg.ipAddr                       = SysCfgIni.GetValue("sipServer", "ipAddr", "127.0.0.1");
    SipServAddrCfg.port                         = SysCfgIni.GetLongValue("sipServer", "port", 5060);
    SipServAddrCfg.regPort                      = SysCfgIni.GetLongValue("sipServer", "regPort", 5061);
    SipServAddrCfg.name                         = SysCfgIni.GetValue("sipServer", "name", "mySipServer");
    SipServAddrCfg.domain                       = SysCfgIni.GetValue("sipServer", "domain", "mySipServer.com");

    // 读取http服务地址配置
    HttpServAddrCfg.id                          = SysCfgIni.GetValue("httpServer", "id", "10000100001000010000");
    HttpServAddrCfg.ipAddr                      = SysCfgIni.GetValue("httpServer", "ipAddr", "127.0.0.1");
    HttpServAddrCfg.port                        = SysCfgIni.GetLongValue("httpServer", "port", 8080);
    HttpServAddrCfg.name                        = SysCfgIni.GetValue("httpServer", "name", "myHttpServer");
    HttpServAddrCfg.domain                      = SysCfgIni.GetValue("httpServer", "domain", "myHttpServer.com");

    // 读取sip服务注册文件配置
    std::string sipRegfilePath                  = SysCfgIni.GetValue("sipReg", "sipRegFilePath", ".");
    std::string sipRegfileName                  = SysCfgIni.GetValue("sipReg", "sipRegFileName", "reg.json");
    
    if (!sipRegfilePath.empty() && !sipRegfileName.empty()) {
        sipRegfileName = sipRegfilePath + std::string("/") + sipRegfileName;
        SipRegServCfg.localServId = SipServAddrCfg.id;

        // 解析sip服务注册文件
        if (MyStatus_t::SUCCESS != MyJsonHelper::ParseSipRegJsonFile(sipRegfileName, SipRegServCfg)) {
            return MyStatus_t::FAILED;
        }
    }
    
    // 读取sip服务设备目录文件配置
    std::string sipCatalogfilePath              = SysCfgIni.GetValue("sipCatalog", "sipCatalogFilePath", ".");
    std::string sipCatalogfileName              = SysCfgIni.GetValue("sipCatalog", "sipCatalogFileName", "catalog.json");
    
    if (!sipCatalogfilePath.empty() && !sipCatalogfileName.empty()) {
        sipCatalogfileName = sipCatalogfilePath + std::string("/") + sipCatalogfileName;

        // 解析sip服务设备目录文件
        if (MyStatus_t::SUCCESS != MyJsonHelper::ParseSipCatalogJsonFile(sipCatalogfileName, SipCatalogCfg)) {
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

MyStatus_t MySystemConfig::GetSipServAddrCfg(MySipServAddrCfg_dt& cfg)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    cfg = SipServAddrCfg;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemConfig::GetHttpServAddrCfg(MyHttpServAddrCfg_dt& cfg)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    cfg = HttpServAddrCfg;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemConfig::GetSipUpRegServCfgMap(MySipRegUpServCfgMap& cfg)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    cfg = SipRegServCfg.upRegSipServMap;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemConfig::GetSipLowRegServCfgMap(MySipRegLowServCfgMap& cfg)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    cfg = SipRegServCfg.lowRegSipServMap;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemConfig::GetSipCatalogCfg(MySipCatalogCfg_dt& cfg)
{
    boost::shared_lock<boost::shared_mutex> lock(CfgMutex);

    cfg = SipCatalogCfg;
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_ENVIR