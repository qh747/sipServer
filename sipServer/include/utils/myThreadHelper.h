#pragma once
#include <string>
#include "common/myDataDef.h"
#include "utils/myBaseHelper.h"
using MY_COMMON::ThreadState_t;

namespace MY_UTILS {
/**
 * 线程状态转换为字符串类
 */
class MyThreadHelper : public MyNonConstructableAndNonCopyable
{
public:
    /** 
     * @brief           将线程状态转换为字符串   
     * @return          线程状态字符串 
     * @param state     线程状态
     */
    static std::string  ThreadStateToStr(ThreadState_t state);
};
}; // namespace MY_UTILS