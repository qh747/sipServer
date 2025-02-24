#pragma once
#include <memory>
#include <functional>
#include <Thread/ThreadPool.h>
#include "common/myTypeDef.h"
#include "utils/myBaseHelper.h"

namespace MY_ENVIR {

/**
 * 线程池类
 */
class MySystemThdPool : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    typedef std::shared_ptr<toolkit::ThreadPool>    ThreadPoolPtr;
    typedef std::function<void()>                   ThreadPoolTaskFunc;

public:
    /**
     * @brief                                       初始化
     * @return                                      MyStatus_t状态码
     */                         
    static MY_COMMON::MyStatus_t                    Init();

    /**                                         
     * @brief                                       关闭
     * @return                                      MyStatus_t状态码
     */                                             
    static MY_COMMON::MyStatus_t                    Shutdown();

public:     
    /**             
     * @brief                                       获取状态
     * @return                                      获取结果，0-success，-1-failed
     * @param status                                状态
     */                 
    static MY_COMMON::MyStatus_t                    GetState(MY_COMMON::MyStatus_t& status);

    /**
     * @brief                                       执行任务
     * @param task                                  任务
     * @return                                      MyStatus_t状态码
     */                         
    static MY_COMMON::MyStatus_t                    ExecuteTask(ThreadPoolTaskFunc task);

    /**
     * @brief                                       执行高优先级任务
     * @param task                                  高优先级任务
     * @return                                      MyStatus_t状态码
     */                         
    static MY_COMMON::MyStatus_t                    ExecuteHighPrioityTask(ThreadPoolTaskFunc task);

private:    
    //                                              启动状态 
    static MY_COMMON::MySipStatusAtomic             SysThdPoolState;

    //                                              系统线程池
    static ThreadPoolPtr                            SysThdPool;
};

}; // namespace MY_ENVIR