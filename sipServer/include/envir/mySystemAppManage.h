#pragma once
#include <string>
#include <memory>
#include "common/myDataDef.h"
#include "utils/myBaseHelper.h"

namespace MY_APP { 

class MySipMsgProcApp;
class MySipRegApp;

}; // namespace MY_APP

namespace MY_ENVIR {

/**
 * 应用管理类
 */
class MySystemAppManage : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    /**
     * 应用管理对象类
     */
    class MySystemAppManageObject;

public:         
    /**         
     * @brief                                       初始化
     * @return                                      MyStatus_t状态码
     */                                             

    static MY_COMMON::MyStatus_t                    Init();
    /**                                             
     * @brief                                       运行
     * @return                                      MyStatus_t状态码
     */                                             

    static MY_COMMON::MyStatus_t                    Run();

    /**                                             
     * @brief                                       关闭
     * @return                                      MyStatus_t状态码
     */                                             
    static MY_COMMON::MyStatus_t                    Shutdown();
};

}; // namespace MY_ENVIR