#pragma once
#include <string>
#include "utils/myBaseHelper.h"
#include "common/myDataDef.h"
using MY_COMMON::MyStatus_t;

namespace MY_UTILS {

/**
 * 时间帮助类
 */
class MyTimeHelper : public MyNonConstructableAndNonCopyable
{
public:
    /** 
     * @brief                   与当前时间比较
     * @return                  解析结果，0-相等, -1-小于当前时间, 1-大于当前时间
     * @param fmtTimeStr        格式化的时间字符串，如：2020-01-01 00:00:00
     * @param addSeconds        增加的秒数
     */
    static int                  CompareWithCurrentTime(const std::string& fmtTimeStr, double addSeconds = 0.0);

    /** 
     * @brief                   获取当前时间比较字符串
     * @return                  当前时间比较字符串，如：2020-01-01 00:00:00
     */
    static std::string          GetNowTimeStr();

    /** 
     * @brief                   是否为格式化时间字符串
     * @return                  是为0，否则为-1
     * @param fmtTimeStr        时间字符串，如：2020-01-01 00:00:00
     */
    static MyStatus_t           IsFormatTimeStr(const std::string& fmtTimeStr);
};

}; // namespace MY_UTILS