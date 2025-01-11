#pragma once
#include <memory>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "utils/myBaseHelper.h"
#include "common/myDataDef.h"
using MY_COMMON::MyStatus_t;

// 前置声明MySipServer
namespace MY_SERVER { class MySipServer; };
using MY_SERVER::MySipServer;

namespace MY_ENVIR {
/**
 * 系统环境类
 */
class MySystemEnvir : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    typedef std::shared_ptr<MySipServer>    MySipServerPtr;

public:         
    /**         
     * @brief                               初始化系统环境
     * @return                              MyStatus_t状态码
     * @param  argc                         参数个数
     * @param  argv                         参数列表
     */                 

    static MyStatus_t                       Init(int argc, char** argv);
    /**                 
     * @brief                               运行程序
     * @return                              MyStatus_t状态码
     */                 

    static MyStatus_t                       Run();
    /**                 
     * @brief                               关闭系统环境
     * @return                              MyStatus_t状态码
     */                 
    static MyStatus_t                       Shutdown();

private:
    static MySipServerPtr                   SipServerPtr;
};

}; // namespace MY_ENVIR