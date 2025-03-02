#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySystemConfg.h"
#include "envir/mySystemPjsip.h"
#include "envir/mySystemSigCatch.h"
#include "envir/mySystemThreadPool.h"
#include "manager/myServManage.h"
#include "manager/myAppManage.h"
#include "envir/mySystemEnvir.h"
using MY_COMMON::MyStatus_t;
using MY_COMMON::MyServLogCfg_dt;
using MY_MANAGER::MyAppManage;
using MY_MANAGER::MyServManage;

namespace MY_ENVIR {

DEFINE_string(cfg, "", "cfg");

MyStatus_t MySystemEnvir::Init(int argc, char** argv)
{
    // --------------------------------------------------- 命令行参数解析 -------------------------------------------------------
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    if (FLAGS_cfg.empty()) {
        LOG(ERROR) << "Server envirment init failed. servCfgPath is empty";
        return MyStatus_t::FAILED;
    }

    // --------------------------------------------------- 加载配置 ------------------------------------------------------------
    MyStatus_t status = MySystemConfig::Init(FLAGS_cfg);
    if (MyStatus_t::SUCCESS != status) {
        LOG(ERROR) << "Server envirment init failed. load config file failed. config path: " << FLAGS_cfg;
        return MyStatus_t::FAILED;
    }
 
    // --------------------------------------------------- 日志初始化 -----------------------------------------------------------
    google::InitGoogleLogging("sipServer");
    MyServLogCfg_dt logConfig;
    MySystemConfig::GetServerLogCfg(logConfig);
    
    // 是否将日志输出到文件和stderr
    FLAGS_alsologtostderr  = logConfig.enableOutputToConsole;
    // 是否启用不同颜色显示
    FLAGS_colorlogtostderr = logConfig.enableUseDiffColorDisplay;
    // 日志输出路径
    FLAGS_log_dir          = logConfig.logPath;
    // 日志等级
    if      ("INFO"    == logConfig.logLevel || "info"    == logConfig.logLevel)  { FLAGS_minloglevel = google::INFO;    } 
    else if ("WARNING" == logConfig.logLevel || "warning" == logConfig.logLevel)  { FLAGS_minloglevel = google::WARNING; } 
    else if ("ERROR"   == logConfig.logLevel || "error"   == logConfig.logLevel)  { FLAGS_minloglevel = google::ERROR;   } 
    else if ("FATAL"   == logConfig.logLevel || "fatal"   == logConfig.logLevel)  { FLAGS_minloglevel = google::FATAL;   } 
    else                                                                          { FLAGS_minloglevel = google::INFO;    }

    // --------------------------------------------------- 信号处理 ------------------------------------------------------------
    if (MyStatus_t::SUCCESS != MySystemSigCatch::Init()) {
        LOG(ERROR) << "Server envirment init failed. signal catcher init failed";
        return MyStatus_t::FAILED;
    }

    // --------------------------------------------------- 线程池初始化 ------------------------------------------------------
    if (MyStatus_t::SUCCESS != MySystemThdPool::Init()) {
        LOG(ERROR) << "Server envirment init failed. thread pool system environment init failed";
        return MyStatus_t::FAILED;
    }

    // --------------------------------------------------- pjsip环境初始化 ------------------------------------------------------
    if (MyStatus_t::SUCCESS != MySystemPjsip::Init()) {
        LOG(ERROR) << "Server envirment init failed. pjsip system environment init failed";
        return MyStatus_t::FAILED;
    }

    // --------------------------------------------------- 服务管理初始化 ------------------------------------------------------
    if (MyStatus_t::SUCCESS != MyServManage::Init()) {
        LOG(ERROR) << "Server envirment init failed. system server init failed";
        return MyStatus_t::FAILED;
    }

    // --------------------------------------------------- 应用管理初始化 ------------------------------------------------------
    if (MyStatus_t::SUCCESS != MyAppManage::Init()) {
        LOG(ERROR) << "Server envirment init failed. system app init failed";
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemEnvir::Run()
{
    // 运行服务管理
    if (MyStatus_t::SUCCESS != MyServManage::Run()) {
        LOG(ERROR) << "Server run failed. MyServManage::Run() failed";
        return MyStatus_t::FAILED;
    }

    // 运行应用管理
    if (MyStatus_t::SUCCESS != MyAppManage::Run()) {
        LOG(ERROR) << "Server run failed. MyAppManage::Run() failed";
        return MyStatus_t::FAILED;
    }

    // 信号处理阻塞程序退出
    if (MyStatus_t::SUCCESS != MySystemSigCatch::Run()) {
        LOG(ERROR) << "Server run failed. signal catcher failed";
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemEnvir::Shutdown()
{
    // 应用管理关闭
    MyAppManage::Shutdown();
    LOG(INFO) << "Server envirment shutdown. MyAppManage::Shutdown() success";

    // 服务管理关闭
    MyServManage::Shutdown();
    LOG(INFO) << "Server envirment shutdown. MyServManage::Shutdown() success";

    // 线程池关闭
    MySystemThdPool::Shutdown();
    LOG(INFO) << "Server envirment shutdown. MySystemThdPool::Shutdown() success";

    // pjsip环境关闭
    MySystemPjsip::Shutdown();
    LOG(INFO) << "Server envirment shutdown. MySystemThdPool::Shutdown() success";

    // glog环境关闭
    google::ShutdownGoogleLogging();
    LOG(INFO) << "Server envirment shutdown. google::ShutdownGoogleLogging() success";

    return MyStatus_t::SUCCESS;
}

}; // namespace MY_ENVIR