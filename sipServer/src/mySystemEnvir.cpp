#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySystemConfg.h"
#include "envir/mySystemPjsip.h"
#include "envir/mySystemSigCatch.h"
#include "envir/mySystemServManage.h"
#include "envir/mySystemAppManage.h"
#include "envir/mySystemEnvir.h"
using MY_COMMON::MyStatus_t;
using MY_COMMON::MyServLogCfg_dt;

namespace MY_ENVIR {

DEFINE_string(servCfgPath, "", "servCfgPath");

MyStatus_t MySystemEnvir::Init(int argc, char** argv)
{
    // --------------------------------------------------- 命令行参数解析 -------------------------------------------------------
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    if (FLAGS_servCfgPath.empty()) {
        LOG(ERROR) << "Server envirment init failed. servCfgPath is empty";
        return MyStatus_t::FAILED;
    }

    // --------------------------------------------------- 加载配置 ------------------------------------------------------------
    MyStatus_t status = MySystemConfig::Init(FLAGS_servCfgPath);
    if (MyStatus_t::SUCCESS != status) {
        LOG(ERROR) << "Server envirment init failed. load config file failed. config path: " << FLAGS_servCfgPath;
        return MyStatus_t::FAILED;
    }
 
    // --------------------------------------------------- 日志初始化 -----------------------------------------------------------
    google::InitGoogleLogging("sipServer");
    const MyServLogCfg_dt& logConfig = MySystemConfig::GetServerLogCfg();
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

    // --------------------------------------------------- pjsip环境初始化 ------------------------------------------------------
    if (MyStatus_t::SUCCESS != MySystemPjsip::Init()) {
        LOG(ERROR) << "Server envirment init failed. pjsip system environment init failed";
        return MyStatus_t::FAILED;
    }

    // --------------------------------------------------- 服务管理初始化 ------------------------------------------------------
    if (MyStatus_t::SUCCESS != MySystemServManage::Init()) {
        LOG(ERROR) << "Server envirment init failed. system server init failed";
        return MyStatus_t::FAILED;
    }

    // --------------------------------------------------- 应用管理初始化 ------------------------------------------------------
    if (MyStatus_t::SUCCESS != MySystemAppManage::Init()) {
        LOG(ERROR) << "Server envirment init failed. system app init failed";
        return MyStatus_t::FAILED;
    }

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemEnvir::Run()
{
    // 运行服务管理
    if (MyStatus_t::SUCCESS != MySystemServManage::Run()) {
        LOG(ERROR) << "Server run failed. MySystemServManage::Run() failed";
        return MyStatus_t::FAILED;
    }

    // 运行应用管理
    if (MyStatus_t::SUCCESS != MySystemAppManage::Run()) {
        LOG(ERROR) << "Server run failed. MySystemAppManage::Run() failed";
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
    MySystemAppManage::Shutdown();

    // 服务管理关闭
    MySystemServManage::Shutdown();

    // pjsip环境关闭
    MySystemPjsip::Shutdown();

    // glog环境关闭
    google::ShutdownGoogleLogging();
    
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_ENVIR