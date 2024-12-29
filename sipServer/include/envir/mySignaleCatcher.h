#pragma once
#include <csignal>
#include <boost/thread/detail/singleton.hpp>

namespace MY_TOOLS
{
/**
 * 信号处理类
 */
class MySignalCatcher : public boost::detail::thread::singleton<MySignalCatcher>
{
public:


};
} // namespace MY_TOOLS

