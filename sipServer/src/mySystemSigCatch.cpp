#define GLOG_USE_GLOG_EXPORT
#include <thread>
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySystemSigCatch.h"
using MY_COMMON::MyStatus_t;

namespace MY_ENVIR {
    
std::recursive_mutex         MySystemSigCatch::Mutex;
std::condition_variable_any  MySystemSigCatch::Condition;

MyStatus_t MySystemSigCatch::Init()
{
    // 捕获ctrl + c信号
    signal(SIGINT, MySystemSigCatch::SignalHandle);
    // 捕获ctrl + z信号
    signal(SIGSTOP, MySystemSigCatch::SignalHandle);
    // 捕获ctrl + \信号
    signal(SIGQUIT, MySystemSigCatch::SignalHandle);
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemSigCatch::Run()
{ 
    // 阻塞程序退出
    std::unique_lock<std::recursive_mutex> lock(Mutex);
    Condition.wait(lock);
    
    // 等待1秒，确保所有线程都退出
    std::this_thread::sleep_for(std::chrono::seconds(1)); 
    return MyStatus_t::SUCCESS;
}

void MySystemSigCatch::SignalHandle(int signum)
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