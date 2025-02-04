#pragma once
#include <iomanip>
#include <sstream>
#include <string>
#include <chrono>
#include <ctime>
#include "common/myDataDef.h"
#include "common/myConfigDef.h"
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

/**
 * 时间帮助类
 */
class MyTimeHelper : public MyNonConstructableAndNonCopyable
{
public:
    /** 
     * @brief             获取格式化后的当前时间（格式：yyyy-mm-dd hh:mm:ss）
     * @return            格式化后的当前时间
     */
    static std::string    GetCurrentFormatTime();

    /** 
     * @brief             获取sip消息首部支持格式的当前时间（格式：yyyy-mm-ddhh:mm:ss）
     * @return            sip消息首部支持格式的当前时间
     */
    static std::string    GetCurrentSipHeaderTime();
};

}; // namespace MY_UTILS