#include <iomanip>
#include <chrono>
#include <ctime>
#include "utils/myTimeHelper.h"
using namespace MY_COMMON;

namespace MY_UTILS {

MyStatus_t MyTimeHelper::GetCurrentFormatTime(std::string& timeStr)
{
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();

    // 转换为time_t
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    // 转换为tm结构
    std::tm* now_tm = std::localtime(&now_time_t);

    // 创建一个stringstream对象用于格式化时间
    std::ostringstream oss;
    
    // 使用put_time将时间格式化为指定格式
    oss << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");

    // 返回格式化后的字符串
    timeStr = oss.str();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyTimeHelper::GetCurrentSipHeaderTime(std::string& timeStr)
{
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();
    // 转换为time_t
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    // 转换为tm结构
    std::tm* now_tm = std::localtime(&now_time_t);

    // 获取毫秒部分
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    // 创建一个stringstream对象用于格式化时间
    std::ostringstream oss;
    // 使用put_time将时间格式化为指定格式
    oss << std::put_time(now_tm, "%Y-%m-%dT%H:%M:%S.") << std::setw(3) << std::setfill('0') << now_ms.count();

    // 返回格式化后的字符串
    timeStr = oss.str();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyTimeHelper::CompareWithCurrentTime(const std::string& timeStr, unsigned int secToAdd, int& timeDiff)
{
    // 时间字符串解析
    std::tm tm = {};
    std::istringstream ss(timeStr);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        return MyStatus_t::FAILED;
    }
    
    auto cmpTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    // 增加指定的秒数
    cmpTime += std::chrono::duration<unsigned int>(secToAdd);

    // 获取当前时间
    auto curTime = std::chrono::system_clock::now();

    // 计算时间差
    timeDiff = std::chrono::duration_cast<std::chrono::duration<int>>(cmpTime - curTime).count();
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_UTILS