#include "envir/mySignaleCatch.h"
#include "envir/mySystemConfg.h"
#include "utils/myLogHelper.h"
#include "server/mySipServer.h"
#include "envir/mySystemEnvir.h"
using MY_UTILS::MyLogHelper;
using MY_SERVER::MySipServer;

namespace MY_ENVIR {

DEFINE_string(servCfgPath, "", "servCfgPath");
MySystemEnvir::MySipServerPtr MySystemEnvir::SipServerPtr = nullptr;

MyStatus_t MySystemEnvir::Init(int argc, char** argv)
{
    // --------------------------------------------------- 命令行参数解析 -------------------------------------------------------
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    if (FLAGS_servCfgPath.empty()) {
        LOG(ERROR) << "Server envirment init failed. servCfgPath is empty";
        return MyStatus_t::FAILED;
    }

    // --------------------------------------------------- 加载配置 ------------------------------------------------------------
    MyStatus_t status = MySystemConfig::load(FLAGS_servCfgPath);
    if (MyStatus_t::SUCCESS != status) {
        LOG(ERROR) << "Server envirment init failed. Load config file failed. Config path: " << FLAGS_servCfgPath;
        return MyStatus_t::FAILED;
    }
 
    // --------------------------------------------------- 日志初始化 -----------------------------------------------------------
    const MyServerLogConfig_dt& logConfig = MySystemConfig::GetServerLogConfig();
    google::InitGoogleLogging("sipServer");
    // 是否将日志输出到文件和stderr
    FLAGS_alsologtostderr  = logConfig.enableOutputToConsole;
    // 是否启用不同颜色显示
    FLAGS_colorlogtostderr = logConfig.enableUseDiffColorDisplay;
    // 日志输出路径
    FLAGS_log_dir          = logConfig.logPath;
    // 日志等级
    FLAGS_minloglevel      = MyLogHelper::LogLevelConvert(logConfig.logLevel);

    // --------------------------------------------------- sip服务初始化 --------------------------------------------------------
    SipServerPtr = std::make_shared<MySipServer>(false);
    if (MyStatus_t::SUCCESS != SipServerPtr->init()) {
        LOG(INFO) << "Server envirment init failed. sip server init failed.";
        return MyStatus_t::FAILED;
    }

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemEnvir::Run()
{
    // sip服务启动
    if (nullptr != SipServerPtr) {
        // server运行在子线程，不会阻塞主线程
        if (MyStatus_t::SUCCESS != SipServerPtr->run()) {
            // server运行失败不退出，继续处理信号
            LOG(ERROR) << "Server run failed. sip server run failed.";
        }
    }
    
    // 信号处理阻塞程序退出
    if (MyStatus_t::SUCCESS != MySignalCatch::Run()) {
        LOG(ERROR) << "Server run failed. Signal catcher failed";
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemEnvir::Shutdown()
{
    // 关闭sipServer
    if (nullptr != SipServerPtr) {
        SipServerPtr->shutdown();
    }

    // 当要结束glog时必须关闭库，否则会内存溢出
    google::ShutdownGoogleLogging();
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_ENVIR