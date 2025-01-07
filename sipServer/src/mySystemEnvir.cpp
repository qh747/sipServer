#include "envir/mySignaleCatch.h"
#include "envir/mySystemConfg.h"
#include "server/mySipServer.h"
#include "envir/mySystemEnvir.h"
using MY_SERVER::MySipServer;

namespace MY_ENVIR {

DEFINE_string(serverConfigPath, "", "serverConfigPath");

google::LogSeverity MySystemEnvir::LogLevelConvert(const std::string& level)
{
    if ("INFO" == level || "info" == level) {
        return google::INFO;
    } 
    else if ("WARNING" == level || "warning" == level) {
        return google::WARNING;
    } 
    else if ("ERROR" == level || "error" == level) {
        return google::ERROR;
    } 
    else if ("FATAL" == level || "fatal" == level) {
        return google::FATAL;
    } 
    else {
        return google::INFO;
    }
}

MyStatus_t MySystemEnvir::Init(int argc, char** argv)
{
    // --------------------------------------------------- 命令行参数解析 ------------------------------------------------------------
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    if (FLAGS_serverConfigPath.empty()) {
        LOG(ERROR) << "Server init failed. serverConfigPath is empty";
        return MyStatus_t::FAILED;
    }

    // --------------------------------------------------- 加载配置 ------------------------------------------------------------
    MyStatus_t status = MySystemConfig::load(FLAGS_serverConfigPath);
    if (MyStatus_t::SUCCESS != status) {
        LOG(ERROR) << "Server init failed. Load config file failed. Config path: " << FLAGS_serverConfigPath;
        return MyStatus_t::FAILED;
    }
 
    // --------------------------------------------------- 日志初始化 ------------------------------------------------------------
    const MyServerLogConfig_dt& logConfig = MySystemConfig::GetServerLogConfig();
    google::InitGoogleLogging("sipServer");
    // 是否将日志输出到文件和stderr
    FLAGS_alsologtostderr  = logConfig.enableOutputToConsole;
    // 是否启用不同颜色显示
    FLAGS_colorlogtostderr = logConfig.enableUseDiffColorDisplay;
    // 日志输出路径
    FLAGS_log_dir          = logConfig.logPath;
    // 日志等级
    FLAGS_minloglevel      = MySystemEnvir::LogLevelConvert(logConfig.logLevel);

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemEnvir::Run()
{
    // 启动sipServer(暂时)
    std::shared_ptr<MySipServer> sipServerPtr = std::make_shared<MySipServer>(false);
    if (sipServerPtr->init()) {
        LOG(INFO) << "Server run success.";
    }

    // 信号处理阻塞程序退出
    MyStatus_t status = MySignalCatcher::Run();
    if (MyStatus_t::SUCCESS != status) {
        LOG(ERROR) << "Server run failed. Signal catcher failed";
        return MyStatus_t::FAILED;
    }

    // 关闭sipServer(暂时)
    if (sipServerPtr->shutdown()) {
        LOG(INFO) << "Server shutdown success.";
    }

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