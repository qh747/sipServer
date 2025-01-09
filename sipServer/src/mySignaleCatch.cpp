#define GLOG_USE_GLOG_EXPORT
#include <thread>
#include <glog/logging.h>
#include "envir/mySignaleCatch.h"

namespace MY_ENVIR {
    
std::recursive_mutex         MySignalCatch::Mutex;
std::condition_variable_any  MySignalCatch::Condition;

MyStatus_t MySignalCatch::Run()
{
    // 捕获ctrl + c信号
    signal(SIGINT, MySignalCatch::SignalHandle);
    // 捕获ctrl + z信号
    signal(SIGSTOP, MySignalCatch::SignalHandle);
    // 捕获ctrl + \信号
    signal(SIGQUIT, MySignalCatch::SignalHandle);

    // 阻塞程序退出
    std::unique_lock<std::recursive_mutex> lock(Mutex);
    Condition.wait(lock);
    
    // 等待1秒，确保所有线程都退出
    std::this_thread::sleep_for(std::chrono::seconds(1)); 
    return MyStatus_t::SUCCESS;
}

void MySignalCatch::SignalHandle(int signum)
{
    switch (signum) {
        case SIGINT: 
        case SIGSTOP: 
        case SIGQUIT: {
            LOG(INFO) << "Interrupt signal (" << signum << ") received. server is shutting down...";
            signal(SIGINT, SIG_IGN);
            Condition.notify_one();
            break;
        } 
        default:
            LOG(INFO) << "Unknown signal (" << signum << ") received. Enter crtl + c to exit";
            break;
    };
}
}; // namespace MY_ENVIR