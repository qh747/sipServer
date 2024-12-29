#pragma once
#include <map>
#include <csignal>
#include <cstdbool>
#include "utils/myHelper.h"
#include "common/myDataDef.h"

using MY_COMMON::MyStatus_t;

namespace MY_ENVIR 
{
    /**
     * 信号处理类
     */
    class MySignalCatcher : public MY_UTILS::MyNonConstructableAndNonCopyable
    {
    public:
        /**
         * @brief           信号处理类对象初始化
         * @return          MyStatus_t状态码
         */
        static MyStatus_t   Init();

    private:
        /**
         * @brief           信号处理函数
         * @param signum    信号id
         */
        static void         SignalHandler(int signum);
    };
}; // namespace MY_ENVIR