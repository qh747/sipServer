#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "common/myConfigDef.h"
#include "envir/mySystemConfg.h"
#include "envir/mySystemPjsip.h"
using namespace MY_COMMON;
using MY_ENVIR::MySystemConfig;

namespace MY_ENVIR {

std::atomic<MyStatus_t>     MySystemPjsip::SysPjsipState(MyStatus_t::FAILED);
pj_caching_pool             MySystemPjsip::SysPjsipCachingPool;
MySipEndptPtr               MySystemPjsip::SysPjsipEndptPtr      = nullptr;
MySipMediaEndptPtr          MySystemPjsip::SysPjsipMediaEndptPtr = nullptr;

MyStatus_t MySystemPjsip::Init()
{
    if (MyStatus_t::SUCCESS == SysPjsipState.load()) {
        LOG(WARNING) << "System pjsip environment has been inited.";
        return MyStatus_t::SUCCESS;
    }

    // pjsip库初始化
    if (MyStatus_t::SUCCESS != InitPjsipLib()) {
        LOG(ERROR) << "System pjsip environment init pjsip lib failed. InitPjsipLib() error.";
        return MyStatus_t::FAILED;
    }

    // pjsip库初始化
    if (MyStatus_t::SUCCESS != InitPjsipModule()) {
        LOG(ERROR) << "System pjsip environment init pjsip lib failed. InitPjsipModule() error.";
        return MyStatus_t::FAILED;
    }

    SysPjsipState.store(MyStatus_t::SUCCESS);
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemPjsip::Shutdown()
{
    if (MyStatus_t::FAILED == SysPjsipState.load()) {
        LOG(WARNING) << "System pjsip environment is not init.";
        return MyStatus_t::SUCCESS;
    }
    SysPjsipState.store(MyStatus_t::FAILED);

    if (nullptr != SysPjsipMediaEndptPtr) {
        pjmedia_endpt_destroy(SysPjsipMediaEndptPtr);
        SysPjsipMediaEndptPtr = nullptr;
    }

    if (nullptr != SysPjsipEndptPtr) {
        pjsip_endpt_destroy(SysPjsipEndptPtr);
        SysPjsipEndptPtr = nullptr;
    }

    pj_caching_pool_destroy(&SysPjsipCachingPool);
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemPjsip::InitPjsipLib()
{
    // pjlib 初始化
    if (PJ_SUCCESS != pj_init()) {
        LOG(ERROR) << "System pjsip environment init pjsip lib failed. pj_init() error.";
        return MyStatus_t::FAILED;
    }

    // pjlib util初始化
    if (PJ_SUCCESS != pjlib_util_init()) {
        LOG(ERROR) << "System pjsip environment init pjsip lib failed. pjlib_util_init() error.";
        return MyStatus_t::FAILED;
    }

    const MySipStackCfg_dt& sipStackCfg = MySystemConfig::GetSipStackCfg();

    // pjsip 内存池初始化
    pj_bzero(&SysPjsipCachingPool, sizeof(pj_caching_pool));
    pj_caching_pool_init(&SysPjsipCachingPool, nullptr, sipStackCfg.sipStackSize);

    // pjsip endpoint初始化
    if (PJ_SUCCESS != pjsip_endpt_create(&SysPjsipCachingPool.factory, sipStackCfg.sipStackName.c_str(), &SysPjsipEndptPtr)) {
        LOG(ERROR) << "System pjsip environment init pjsip lib failed. pjsip_endpt_create() error.";
        return MyStatus_t::FAILED;
    }

    // pjsip media endpoint初始化
    if (PJ_SUCCESS != pjmedia_endpt_create(&SysPjsipCachingPool.factory, pjsip_endpt_get_ioqueue(SysPjsipEndptPtr), 0, &SysPjsipMediaEndptPtr)) {
        LOG(ERROR) << "System pjsip environment init pjsip lib failed. pjmedia_endpt_create() error.";
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemPjsip::InitPjsipModule()
{
    if (nullptr == SysPjsipEndptPtr) {
        LOG(ERROR) << "System pjsip environment init pjsip module failed. endpointPtr is null.";
        return MyStatus_t::FAILED;
    }

    // 事务层模块初始化
    if (PJ_SUCCESS != pjsip_tsx_layer_init_module(SysPjsipEndptPtr)) {
        LOG(ERROR) << "System pjsip environment init pjsip module failed. pjsip_tsx_layer_init_module() error.";
        return MyStatus_t::FAILED;
    }

    // 用户代理层模块初始化
    if (PJ_SUCCESS != pjsip_ua_init_module(SysPjsipEndptPtr, nullptr)) {
        LOG(ERROR) << "System pjsip environment init pjsip module failed. pjsip_ua_init_module() error.";
        return MyStatus_t::FAILED;
    }

    // sip 100 trying临时应答模块初始化
    if(PJ_SUCCESS != pjsip_100rel_init_module(SysPjsipEndptPtr)) {
        LOG(ERROR) << "System pjsip environment init pjsip module failed. pjsip_100rel_init_module() error.";
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_ENVIR