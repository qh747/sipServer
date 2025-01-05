#pragma once
#include <string>
#include <boost/noncopyable.hpp>
#include "common/myDataDef.h"
using MY_COMMON::ThreadState_t;

namespace MY_UTILS {
/**
 * 禁止构造类
 */
class MyNonConstructable
{
private:
    MyNonConstructable() = delete;
    ~MyNonConstructable() = delete;
};

/**
 * 禁止构造和拷贝类
 */
class MyNonConstructableAndNonCopyable : public boost::noncopyable
{
private:
    MyNonConstructableAndNonCopyable() = delete;
    ~MyNonConstructableAndNonCopyable() = delete;
};

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