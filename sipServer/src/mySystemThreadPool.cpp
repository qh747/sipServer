#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySystemConfg.h"
#include "envir/mySystemThreadPool.h"
using namespace toolkit;
using namespace MY_COMMON;

namespace MY_ENVIR {

std::atomic<MyStatus_t>         MySystemThdPool::SysThdPoolState {MyStatus_t::FAILED};
MySystemThdPool::ThreadPoolPtr  MySystemThdPool::SysThdPool      {nullptr};

MyStatus_t MySystemThdPool::Init()
{
    if (MyStatus_t::SUCCESS == SysThdPoolState.load()) {
        LOG(WARNING) << "System thread pool environment has been inited.";
        return MyStatus_t::SUCCESS;
    }

    // 获取线程池配置
    MyServThdPoolCfg_dt thdCfg;
    if (MyStatus_t::SUCCESS != MySystemConfig::GetServerThdPoolCfg(thdCfg)) {
        LOG(ERROR) << "System thread pool environment init failed. get thread pool config error.";
        return MyStatus_t::FAILED;
    }

    if (thdCfg.threadPriority < ThreadPool::PRIORITY_LOWEST || thdCfg.threadPriority > ThreadPool::PRIORITY_HIGHEST) {
        LOG(ERROR) << "System thread pool environment init failed. thread priority error.";
        return MyStatus_t::FAILED;
    }

    SysThdPool = std::make_shared<ThreadPool>(thdCfg.threadNum, (ThreadPool::Priority)thdCfg.threadPriority, thdCfg.enableAutoRun, thdCfg.enableAffinity);
    if (nullptr == SysThdPool) {
        LOG(ERROR) << "System thread pool environment init failed. create thread pool error.";
        return MyStatus_t::FAILED;
    }

    SysThdPoolState.store(MyStatus_t::SUCCESS);
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemThdPool::Shutdown()
{
    if (MyStatus_t::FAILED == SysThdPoolState.load()) {
        LOG(WARNING) << "System thread pool environment is not init.";
        return MyStatus_t::SUCCESS;
    }

    SysThdPoolState.store(MyStatus_t::FAILED);
    SysThdPool.reset();

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemThdPool::GetState(MyStatus_t& status)
{
    status = SysThdPoolState.load();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemThdPool::ExecuteTask(ThreadPoolTaskFunc task)
{
    if (MyStatus_t::FAILED == SysThdPoolState.load() || nullptr == SysThdPool) {
        LOG(WARNING) << "System thread pool execute task failed. thread pool is not init.";
        return MyStatus_t::FAILED;
    }

    SysThdPool->async(task);
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemThdPool::ExecuteHighPrioityTask(ThreadPoolTaskFunc task)
{
    if (MyStatus_t::FAILED == SysThdPoolState.load() || nullptr == SysThdPool) {
        LOG(WARNING) << "System thread pool execute task failed. thread pool is not init.";
        return MyStatus_t::FAILED;
    }

    SysThdPool->async_first(task);
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_ENVIR