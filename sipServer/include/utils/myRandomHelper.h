#pragma once
#include <string>
#include "common/myDataDef.h"
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

/**
 * @brief 随机数帮助类
 */
class MyRandomHelper : public MyNonConstructableAndNonCopyable
{
public:
    /** 
     * @brief                       获取32位长度随机数字符串
     * @return                      获取结果
     * @param str                   32位长度随机数字符串
     */         
    static MY_COMMON::MyStatus_t    Get32BytesLenRandomStr(std::string& str);

    /** 
     * @brief                       获取4位长度随机数字符串
     * @return                      获取结果
     * @param str                   4位长度随机数字符串
     */         
    static MY_COMMON::MyStatus_t    Get4BytesLenRandomStr(std::string& str);
};

}; // namespace MY_UTILS