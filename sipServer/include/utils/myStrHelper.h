#pragma once
#include <string>
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

/**
 * 字符串帮助类
 */
class MyStrHelper : public MyNonConstructableAndNonCopyable
{
public:
    /** 
     * @brief                       转换字符串为小写
     * @return                      小写字符串
     */         
    static std::string              ConvertToLowStr(const std::string& strToLow);

    /** 
     * @brief                       转换字符串为大写
     * @return                      大写字符串
     */         
    static std::string              ConvertToUpStr(const std::string& strToUp);

    /** 
     * @brief                       去除字符串的空白字符
     * @return                      去除空白字符后的字符串
     */    
    static std::string              TrimEmptyStr(const std::string& str, const std::string& trimStr = " \r\n\t");
};

}; // namespace MY_UTILS