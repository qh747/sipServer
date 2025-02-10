#pragma once
#include <string>
#include <memory>
#include "common/myTypeDef.h"
#include "utils/myBaseHelper.h"
#include "app/mySipRegApp.h"
#include "app/mySipMsgProcApp.h"
using namespace MY_APP; 

namespace MY_MANAGER {

/**
 * 应用管理类
 */
class MyAppManage : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    /**
     * 应用管理对象类
     */
    class MyAppManageObject;

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

public:
    /**
     * @brief                                       获取sip注册应用
     * @return                                      sip注册应用弱引用指针
     * @param servId                                sip服务id
     */
    static MySipRegApp::SmtWkPtr                    GetSipRegApp(const std::string& servId);

    /**
     * @brief                                       获取sip消息处理应用
     * @return                                      sip消息处理应用弱引用指针
     * @param servId                                sip服务id
     */
    static MySipMsgProcApp::SmtWkPtr                GetSipMsgProcApp(const std::string& servId);
};

}; // namespace MY_MANAGER