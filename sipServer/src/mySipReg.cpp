#include <thread>
#include <chrono>
#include <sstream>
#include <cstdbool>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "utils/myTimeHelper.h"
#include "app/mySipReg.h"
using MY_UTILS::MyTimeHelper;

namespace MY_APP {

MySipRegWrapper::RegUpServResMap        MySipRegWrapper::RegUpServMap;
MySipRegWrapper::RegLowServResMap       MySipRegWrapper::RegLowServMap;
std::atomic<MyStatus_t>                 MySipRegWrapper::StartState(MyStatus_t::FAILED);
std::mutex                              MySipRegWrapper::RegLock;
MySipRegWrapper::SipRegThreadPtr        MySipRegWrapper::RegManageThreadPtr = nullptr;
    
MyStatus_t MySipRegWrapper::Init(const std::string& regFilePath)
{
    // 初始化条件验证
    if (regFilePath.empty()) {
        LOG(WARNING) << "Sip reg module init failed. invalid reg file path.";
        return MyStatus_t::FAILED;
    }

    // 解析注册文件
    MyJsonHelper::RegUpServJsonMap  upRegServInfoMap;
    MyJsonHelper::RegLowServJsonMap lowRegServInfoMap;
    if (MyStatus_t::SUCCESS != MyJsonHelper::ParseServRegJsonFile(regFilePath, upRegServInfoMap, lowRegServInfoMap)) {
        LOG(WARNING) << "Sip reg module init failed. parse reg file failed.";
        return MyStatus_t::FAILED;
    }

    // 初始化注册map表
    for (const auto& iter : upRegServInfoMap) {
        MySipRegUpServResult_dt regUpServResult;
        regUpServResult.id       = iter.second.id;
        regUpServResult.ipAddr   = iter.second.ipAddr;
        regUpServResult.port     = iter.second.port;
        regUpServResult.name     = iter.second.name;
        regUpServResult.domain   = iter.second.domain;
        regUpServResult.duration = iter.second.duration;
        regUpServResult.result   = MyStatus_t::FAILED;
        regUpServResult.time     = "";
        RegUpServMap.insert(std::make_pair(iter.first, std::move(regUpServResult)));    
    }

    for (const auto& iter : lowRegServInfoMap) {
        MySipRegLowServResult_dt regLowServResult;
        regLowServResult.id       = iter.second.id;
        regLowServResult.ipAddr   = iter.second.ipAddr;
        regLowServResult.port     = iter.second.port;
        regLowServResult.name     = iter.second.name;
        regLowServResult.domain   = iter.second.domain;
        regLowServResult.duration = iter.second.duration;
        regLowServResult.result   = MyStatus_t::FAILED;
        regLowServResult.time     = "";
        RegLowServMap.insert(std::make_pair(iter.first, std::move(regLowServResult)));     
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegWrapper::Run(ServThreadPoolPtr threadPoolPtr)
{
    if (nullptr == threadPoolPtr) {
        LOG(ERROR) << "Sip reg module run failed. invalid thread pool ptr.";
        return MyStatus_t::FAILED;
    }
    else if (MyStatus_t::SUCCESS == StartState.load()) {
        LOG(ERROR) << "Sip reg module run failed. module already started.";
        return MyStatus_t::SUCCESS;
    }

    // 向上级服务发起注册
    for (auto& iter : RegUpServMap) {
        if (MyStatus_t::SUCCESS == MySipRegWrapper::RegUpServ(iter.second)) {
            LOG(INFO) << "Sip reg module register upper server success. " << MySipRegWrapper::GetUpRegServInfo(iter.second) << ".";
        }
        else {
            LOG(WARNING) << "Sip reg module register upper server failed. " << MySipRegWrapper::GetUpRegServInfo(iter.second) << ".";
        }
    }

    // 启动子线程，管理上下级服务注册状态
    if (PJ_SUCCESS != pj_thread_create(threadPoolPtr, "MySipRegisterThread", &MySipRegWrapper::OnRegServManageCb, nullptr, 
                                       PJ_THREAD_DEFAULT_STACK_SIZE, 0, &RegManageThreadPtr)) {
        LOG(ERROR) << "Sip reg module run failed. pj_thread_create() error.";
        return MyStatus_t::FAILED;
    }

    StartState.store(MyStatus_t::SUCCESS);
}

MyStatus_t MySipRegWrapper::Shutdown()
{
    if (MyStatus_t::SUCCESS != StartState.load()) {
        LOG(WARNING) << "Sip reg module shutdown failed. module not started.";
        return MyStatus_t::SUCCESS;
    }

    // 等待子线程退出
    StartState.store(MyStatus_t::FAILED);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    if (nullptr != RegManageThreadPtr) {
        pj_thread_join(RegManageThreadPtr);
        pj_thread_destroy(RegManageThreadPtr);
        RegManageThreadPtr = nullptr;
    }
}

MyStatus_t MySipRegWrapper::RegUpServ(MySipRegUpServResult_dt& regUpServ)
{
    
    return MyStatus_t::SUCCESS;
}

int MySipRegWrapper::OnRegServManageCb(SipRegCbParamPtr args)
{
    while (true) {
        if (MyStatus_t::SUCCESS != StartState.load()) {
            LOG(INFO) << "Sip reg module manage thread exit.";
            return 0;
        }

        {
            // 减小临界区
            std::lock_guard<std::mutex> lock(RegLock);

            // 检查上级服务注册状态
            for (auto& iter : RegUpServMap) {
                MySipRegUpServResult_dt& curUpServer = iter.second;
                if (MyStatus_t::SUCCESS != curUpServer.result) {
                    continue;
                }
                // 上级服务注册时间异常
                if (curUpServer.time.empty() || (MyStatus_t::SUCCESS != MyTimeHelper::IsFormatTimeStr(curUpServer.time))) {
                    LOG(WARNING) << "Register upper server invalid. " << MySipRegWrapper::GetUpRegServInfo(curUpServer) << ".";
                    curUpServer.result = MyStatus_t::FAILED;
                    continue;
                }
                // 上级服务注册超时
                if (MyTimeHelper::CompareWithCurrentTime(curUpServer.time, curUpServer.duration) <= 0) {
                    LOG(WARNING) << "Register upper server offline. " << MySipRegWrapper::GetUpRegServInfo(curUpServer) << ".";
                    curUpServer.result = MyStatus_t::FAILED;
                    continue;
                }
            }

            // 检查下级服务注册状态
            for (auto& iter : RegLowServMap) {
                MySipRegLowServResult_dt& curLowServer = iter.second;
                if (MyStatus_t::SUCCESS != curLowServer.result) {
                    continue;
                }
                // 下级服务注册时间异常
                if (curLowServer.time.empty() || (MyStatus_t::SUCCESS != MyTimeHelper::IsFormatTimeStr(curLowServer.time))) {
                    LOG(WARNING) << "Register lower server invalid. " << MySipRegWrapper::GetLowRegServInfo(curLowServer) << ".";
                    curLowServer.result = MyStatus_t::FAILED;
                    continue;
                }
                // 下级服务注册超时
                if (MyTimeHelper::CompareWithCurrentTime(curLowServer.time, curLowServer.duration) <= 0) {
                    LOG(WARNING) << "Register lower server offline. " << MySipRegWrapper::GetLowRegServInfo(curLowServer) << ".";
                    curLowServer.result = MyStatus_t::FAILED;
                    continue;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}

std::string MySipRegWrapper::GetUpRegServInfo(const MySipRegUpServResult_dt& regUpServ)
{
    std::stringstream ss;
    ss << "id: " << regUpServ.id << ", ip: " << regUpServ.ipAddr << ", port: " << regUpServ.port 
       << ", name: " << regUpServ.name << ", domain: " << regUpServ.domain;
    return ss.str();
}

std::string MySipRegWrapper::GetLowRegServInfo(const MySipRegLowServResult_dt& regLowServ)
{
    std::stringstream ss;
    ss << "id: " << regLowServ.id << ", ip: " << regLowServ.ipAddr << ", port: " << regLowServ.port 
       << ", name: " << regLowServ.name << ", domain: " << regLowServ.domain;
    return ss.str();
}

}; // namespace MY_APP