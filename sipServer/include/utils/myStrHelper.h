#pragma once
#include <string>
#include "common/myDataDef.h"
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
     * @return                      转换结果, success-0, fail-非0
     * @param strToLow              待转换的字符串
     * @param lowStr                转换后的小写字符串
     */         
    static MY_COMMON::MyStatus_t    ConvertToLowStr(const std::string& strToLow, std::string& lowStr);

    /** 
     * @brief                       转换字符串为大写
     * @return                      转换结果, success-0, fail-非0
     * @param strToUp               待转换的字符串
     * @param upStr                 转换后的大写字符串
     */         
    static MY_COMMON::MyStatus_t    ConvertToUpStr(const std::string& strToUp, std::string& upStr);

    /** 
     * @brief                       去除字符串的空白字符
     * @return                      去除结果
     * @param str                   待去除的字符串
     * @param resStr                去除后的字符串
     * @param trimStr               去除的字符
     */    
    static MY_COMMON::MyStatus_t    TrimEmptyStr(const std::string& str, std::string& resStr, const std::string& trimStr = " \r\n\t");
};

}; // namespace MY_UTILS