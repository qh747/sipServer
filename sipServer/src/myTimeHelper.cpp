#include <ctime>
#include <regex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "utils/myTimeHelper.h"

namespace MY_UTILS {

int MyTimeHelper::CompareWithCurrentTime(const std::string& fmtTimeStr, double addSeconds)
{
    // 时间字符串转换为时间类对象
    std::istringstream ss {fmtTimeStr};
    std::tm tm = {};
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    auto fmtTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    
    // 增加秒数
    fmtTime = fmtTime + std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<double>(addSeconds));

    // 获取当前时间
    auto now = std::chrono::system_clock::now();

    // 比较时间
    if (fmtTime > now) {
        return 1;
    } 
    else if (fmtTime < now) {
        return -1;
    } 
    else {
        return 0;
    }
}

std::string MyTimeHelper::GetNowTimeStr()
{
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();
    // 转换为time_t以便使用ctime函数
    auto now_c = std::chrono::system_clock::to_time_t(now);
    // 转换为 tm 结构体
    std::tm now_tm = *std::localtime(&now_c);

    // 使用 stringstream 进行格式化
    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

MyStatus_t MyTimeHelper::IsFormatTimeStr(const std::string& fmtTimeStr)
{
    // 正则表达式匹配格式 yyyy-mm-dd hh:mm:ss
    std::regex date_time_regex(R"((\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2}))");

    // 使用正则表达式匹配字符串
    std::smatch matches;
    if (std::regex_match(fmtTimeStr, matches, date_time_regex)) {
        // 检查匹配到的日期和时间是否合理（例如，月份是否在1到12之间，小时是否在0到23之间等）
        int year   = std::stoi(matches[1]);
        int month  = std::stoi(matches[2]);
        int day    = std::stoi(matches[3]);
        int hour   = std::stoi(matches[4]);
        int minute = std::stoi(matches[5]);
        int second = std::stoi(matches[6]);

        // 简单的日期和时间有效性检查
        if (month  >= 1 && month  <= 12 &&
            day    >= 1 && day    <= 31 &&
            hour   >= 0 && hour   <= 23 &&
            minute >= 0 && minute <= 59 &&
            second >= 0 && second <= 59) {
            return MyStatus_t::SUCCESS;
        }
    }
    return MyStatus_t::FAILED;
}

}; // namespace MY_UTILS