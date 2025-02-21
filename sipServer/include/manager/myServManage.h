#pragma once
#include <string>
#include "common/myDataDef.h"
#include "utils/myBaseHelper.h"
#include "server/mySipServer.h"
using MY_SERVER::MySipServer;

namespace MY_MANAGER {

/**
 * 服务管理类
 */
class MyServManage : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    /**
     * 服务管理对象类
     */
    class MyServManageObject;

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
     * @brief                                       获取SipServer对象
     * @return                                      获取结果，0-success，-1-failed
     * @param servId                                sip服务ID
     * @param sipServWkPtr                          sip服务弱引用指针
     */
    static MY_COMMON::MyStatus_t                    GetSipServer(const std::string& servId, MySipServer::SmtWkPtr& sipServWkPtr);

    /**
     * @brief                                       获取SipServer地址配置
     * @return                                      获取结果，0-success，-1-failed
     * @param servId                                sip服务ID
     * @param sipServAddrCfg                        sip服务地址配置
     */
    static MY_COMMON::MyStatus_t                    GetSipServAddrCfg(const std::string& servId, MY_COMMON::MySipServAddrCfg_dt& sipServAddrCfg);

    /**
     * @brief                                       SipServer对象是否存在
     * @return                                      MyStatus_t状态码
     * @param servId                                sip服务ID
     */
    static MY_COMMON::MyStatus_t                    HasSipServer(const std::string& servId);
};

}; // namespace MY_MANAGER








