#pragma once
#include "common/myDataDef.h"
#include "utils/myBaseHelper.h"

namespace MY_ENVIR {

/**
 * 系统环境类
 */
class MySystemEnvir : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:         
    /**         
     * @brief                               初始化
     * @return                              MyStatus_t状态码
     * @param  argc                         参数个数
     * @param  argv                         参数列表
     */                                     

    static MY_COMMON::MyStatus_t            Init(int argc, char** argv);
    /**                                     
     * @brief                               运行
     * @return                              MyStatus_t状态码
     */                                     

    static MY_COMMON::MyStatus_t            Run();
    
    /**                                     
     * @brief                               关闭
     * @return                              MyStatus_t状态码
     */                                     
    static MY_COMMON::MyStatus_t            Shutdown();
};

}; // namespace MY_ENVIR