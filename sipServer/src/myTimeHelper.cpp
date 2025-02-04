#include "utils/myTimeHelper.h"

namespace MY_UTILS {

std::string MyTimeHelper::GetCurrentFormatTime()
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
    return oss.str();
}

std::string MyTimeHelper::GetCurrentSipHeaderTime()
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
    return oss.str();
}

}; // namespace MY_UTILS