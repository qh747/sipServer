#pragma once
#include <mutex>
#include <csignal>
#include <condition_variable>
#include "utils/myBaseHelper.h"
#include "common/myDataDef.h"

namespace MY_ENVIR {
    
/**
 * 信号处理类
 */
class MySystemSigCatch : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    /**
     * @brief                               初始化
     * @return                              MyStatus_t状态码
     */                 
    static MY_COMMON::MyStatus_t            Init();

    /**
     * @brief                               运行
     * @return                              MyStatus_t状态码
     */                 
    static MY_COMMON::MyStatus_t            Run();

private:                    
    /**                 
     * @brief                               信号处理函数
     * @param signum                        信号id
     */                 
    static void                             SignalHandle(int signum);

private:
    static std::recursive_mutex             Mutex;
    static std::condition_variable_any      Condition;
};

}; // namespace MY_ENVIR