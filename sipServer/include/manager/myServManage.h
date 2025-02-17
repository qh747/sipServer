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
     * @return                                      std::weak_ptr<MY_SERVER::MySipServer>
     * @param servId                                sip服务ID
     */
    static MySipServer::SmtWkPtr                    GetSipServer(const std::string& servId);

    /**
     * @brief                                       获取SipServer地址配置
     * @return                                      MY_COMMON::MySipServAddrCfg_dt
     * @param servId                                sip服务ID
     */
    static MY_COMMON::MySipServAddrCfg_dt           GetSipServAddrCfg(const std::string& servId);

    /**
     * @brief                                       获取SipServer内存池
     * @return                                      MY_COMMON::MySipPoolPtr
     * @param servId                                sip服务ID
     */
    static MY_COMMON::MySipPoolPtr                  GetSipServThdPoolPtr(const std::string& servId);

    /**
     * @brief                                       SipServer对象是否存在
     * @return                                      MyStatus_t状态码
     * @param servId                                sip服务ID
     */
    static MY_COMMON::MyStatus_t                    HasSipServer(const std::string& servId);
};

}; // namespace MY_MANAGER








