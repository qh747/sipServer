#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySignaleCatch.h"
#include "envir/mySystemConfg.h"
#include "envir/mySystemEnvir.h"

namespace MY_ENVIR {

DEFINE_string(serverConfigPath, "", "serverConfigPath");
DEFINE_string(serverLogPath,    "", "serverLogPath");

MyStatus_t MySystemEnvir::Init(int argc, char** argv)
{
    // --------------------------------------------------- 命令行参数解析 ------------------------------------------------------------
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    if (FLAGS_serverLogPath.empty() || FLAGS_serverConfigPath.empty()) {
        LOG(ERROR) << "Server init failed. ServerLogPath or serverConfigPath is empty";
        return MyStatus_t::FAILED;
    }
 
    // --------------------------------------------------- 日志初始化 ------------------------------------------------------------
    google::InitGoogleLogging("sipServer");
    // 是否将日志输出到文件和stderr
    FLAGS_alsologtostderr  = true;
    // 是否启用不同颜色显示
    FLAGS_colorlogtostderr = true;
    // 日志输出路径
    FLAGS_log_dir          = FLAGS_serverLogPath;
    // 日志等级
    FLAGS_minloglevel      = google::INFO;

    // --------------------------------------------------- 加载配置 ------------------------------------------------------------
    MyStatus_t status = MySystemConfig::load(FLAGS_serverConfigPath);
    if (MyStatus_t::SUCCESS != status) {
        LOG(ERROR) << "Server init failed. Load config file failed. Config path: " << FLAGS_serverConfigPath;
        return MyStatus_t::FAILED;
    }
    else {
        const MyServerConfig_dt& cfg = MySystemConfig::GetServerConfig();
        LOG(INFO) << "Load config file success: server ip: " << cfg.ipAddr << ", port: " << cfg.port << ", name: " << cfg.name << ", domain: " << cfg.domain;
    }

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemEnvir::Run()
{
    // 信号处理阻塞程序退出
    MyStatus_t status = MySignalCatcher::Run();
    // 资源销毁，后续补充
    status = MySystemEnvir::Shutdown();
    return status;
}

MyStatus_t MySystemEnvir::Shutdown()
{
    // 当要结束glog时必须关闭库，否则会内存溢出
    google::ShutdownGoogleLogging();
    return MyStatus_t::SUCCESS;
}
}; // namespace MY_ENVIR