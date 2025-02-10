#pragma once
#include <string>
#include <tinyxml2.h>
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

/**
 * xml帮助类
 */
class MyXmlHelper : public MyNonConstructableAndNonCopyable
{
public:
    /** 
     * @brief                       sip注册服务json文件解析
     * @return                      sip message消息体
     * @param idx                   消息索引编号
     * @param id                    本级服务id
     */
    static std::string              GenerateSipKeepAliveBody(const std::string& idx, const std::string& localServId);
};

}; // namespace MY_UTILS