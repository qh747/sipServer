#pragma once
#include <boost/noncopyable.hpp>

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

}; // namespace MY_UTILS