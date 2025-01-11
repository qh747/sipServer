#pragma once
#include <string>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

/**
 * 日志帮助类
 */
class MyLogHelper : public MyNonConstructableAndNonCopyable
{
public:
    /** 
     * @brief                   日志类型字符串转换
     * @return                  日志类型
     * @param level             日志类型字符串
     */
    static google::LogSeverity  LogLevelConvert(const std::string& level);
};

}; // namespace MY_UTILS