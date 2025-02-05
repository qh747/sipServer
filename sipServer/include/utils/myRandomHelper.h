#pragma once
#include <string>
#include <algorithm>
#include <uuid/uuid.h>
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

/**
 * 随机数帮助类
 */
class MyRandomHelper : public MyNonConstructableAndNonCopyable
{
public:
    /** 
     * @brief                       获取32位长度随机数字符串
     * @return                      32位长度随机数字符串
     */         
    static std::string              Get32BitsLenRandomStr();
};

}; // namespace MY_UTILS