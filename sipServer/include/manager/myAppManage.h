#pragma once
#include <string>
#include "utils/myBaseHelper.h"
#include "app/mySipRegApp.h"
#include "app/mySipInviteApp.h"
#include "app/mySipCatalogApp.h"
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
     * @return                                      获取结果，0-success，-1-failed
     * @param servId                                sip服务id
     * @param appSmtWkPtr                           sip注册应用弱引用指针
     */
    static MY_COMMON::MyStatus_t                    GetSipRegApp(const std::string& servId, MySipRegApp::SmtWkPtr& appSmtWkPtr);

    /**
     * @brief                                       获取sip消息处理应用
     * @return                                      获取结果，0-success，-1-failed
     * @param servId                                sip服务id
     * @param appSmtWkPtr                           sip消息处理应用弱引用指针
     */
    static MY_COMMON::MyStatus_t                    GetSipMsgProcApp(const std::string& servId, MySipMsgProcApp::SmtWkPtr& appSmtWkPtr);

    /**
     * @brief                                       获取sip设备目录应用
     * @return                                      获取结果，0-success，-1-failed
     * @param servId                                sip服务id
     * @param appSmtWkPtr                           sip设备目录应用弱引用指针
     */
    static MY_COMMON::MyStatus_t                    GetSipCatalogApp(const std::string& servId, MySipCatalogApp::SmtWkPtr& appSmtWkPtr);

    /**
     * @brief                                       获取sip会话邀请应用
     * @return                                      获取结果，0-success，-1-failed
     * @param servId                                sip服务id
     * @param appSmtWkPtr                           sip会话邀请应用弱引用指针
     */
    static MY_COMMON::MyStatus_t                    GetSipInviteApp(const std::string& servId, MySipInviteApp::SmtWkPtr& appSmtWkPtr);
};

}; // namespace MY_MANAGER