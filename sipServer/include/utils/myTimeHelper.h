#pragma once
#include <string>
#include <ctime>
#include "common/myDataDef.h"
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

/**
 * 时间帮助类
 */
class MyTimeHelper : public MyNonConstructableAndNonCopyable
{
public:
    /** 
     * @brief                       获取格式化后的当前时间（格式：yyyy-mm-dd hh:mm:ss）
     * @return                      获取结果，0-success，-1-failed
     * @param timeStr               格式化后的当前时间
     */         
    static MY_COMMON::MyStatus_t    GetCurrentFormatTime(std::string& timeStr);

    /**             
     * @brief                       获取sip消息首部支持格式的当前时间（格式：yyyy-mm-ddhh:mm:ss）
     * @return                      获取结果，0-success，-1-failed
     * @param timeStr               格式化后的当前时间
     */         
    static MY_COMMON::MyStatus_t    GetCurrentSipHeaderTime(std::string& timeStr);

    /**         
     * @brief                       比较时间字符串增加指定秒后与当前时间
     * @return                      比较结果
     * @param timeStr               时间字符串, 格式：yyyy-mm-dd hh:mm:ss
     * @param secToAdd              增加的秒数
     * @param timeDiff              时间差
     */
    static MY_COMMON::MyStatus_t    CompareWithCurrentTime(const std::string& timeStr, unsigned int secToAdd, int& timeDiff);
};

}; // namespace MY_UTILS