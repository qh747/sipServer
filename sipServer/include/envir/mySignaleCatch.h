#pragma once
#include <map>
#include <mutex>
#include <csignal>
#include <cstdbool>
#include <condition_variable>
#include "utils/myBaseHelper.h"
#include "common/myDataDef.h"
using MY_COMMON::MyStatus_t;

namespace MY_ENVIR {
/**
 * 信号处理类
 */
class MySignalCatch : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    /**
     * @brief                               信号处理类对象运行
     * @return                              MyStatus_t状态码
     */                 
    static MyStatus_t                       Run();

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