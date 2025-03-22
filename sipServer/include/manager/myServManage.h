#pragma once
#include <string>
#include "common/myDataDef.h"
#include "utils/myBaseHelper.h"
#include "server/mySipServer.h"
#include "server/myHttpServer.h"
using MY_SERVER::MySipServer;
using MY_SERVER::MyHttpServer;

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
     * @brief                                       sip server是否有效
     * @return                                      MyStatus_t状态码
     */
    static MY_COMMON::MyStatus_t                    IsSipServValid();
    
    /**
     * @brief                                       获取sip server
     * @return                                      获取结果，0-success，-1-failed
     * @param servWkPtr                             sip server弱引用
     */
    static MY_COMMON::MyStatus_t                    GetSipServer(MySipServer::SmtWkPtr& servWkPtr);

    /**
     * @brief                                       获取sip server id
     * @return                                      获取结果，0-success，-1-failed
     * @param id                                    sip server id
     */
    static MY_COMMON::MyStatus_t                    GetSipServId(std::string& id);

    /**
     * @brief                                       获取sip server地址
     * @return                                      获取结果，0-success，-1-failed
     * @param servAddrCfg                           sip server地址
     */
    static MY_COMMON::MyStatus_t                    GetSipServAddrCfg(MY_COMMON::MySipServAddrCfg_dt& servAddrCfg);

    /**
     * @brief                                       获取sip server内存
     * @return                                      获取结果，0-success，-1-failed
     * @param poolAddr                              sip server内存
     */
    static MY_COMMON::MyStatus_t                    GetSipServPool(MY_COMMON::MySipPoolPtrAddr poolAddr);

    /**
     * @brief                                       获取sip服务udp transport
     * @return                                      获取结果，0-success，-1-failed
     * @param udpTpPtrAddr                          sip服务udp transport
     */
    static MY_COMMON::MyStatus_t                    GetSipServUdpTp(MY_COMMON::MySipTransportPtrAddr udpTpPtrAddr);

    /**
     * @brief                                       获取sip服务注册使用的udp transport
     * @return                                      获取结果，0-success，-1-failed
     * @param udpTpPtrAddr                          sip服务udp transport
     */
    static MY_COMMON::MyStatus_t                    GetSipServRegUdpTp(MY_COMMON::MySipTransportPtrAddr udpTpPtrAddr);
 
    /**
     * @brief                                       获取sip服务tcp transport工厂
     * @return                                      获取结果，0-success，-1-failed
     * @param tcpTpPtrAddr                          sip服务tcp transport
     * @param remoteIp                              远端ip地址
     * @param remotePort                            远端端口
     */
    static MY_COMMON::MyStatus_t                    GetSipServTcpTp(MY_COMMON::MySipTransportPtrAddr tcpTpPtrAddr,
                                                        const std::string& remoteIp, uint16_t remotePort);
 
    /**
     * @brief                                       获取sip服务注册使用的tcp transport工厂
     * @return                                      获取结果，0-success，-1-failed
     * @param tcpTpPtrAddr                          sip服务tcp transport
     * @param remoteIp                              远端ip地址
     * @param remotePort                            远端端口
     */
    static MY_COMMON::MyStatus_t                    GetSipServRegTcpTp(MY_COMMON::MySipTransportPtrAddr tcpTpPtrAddr,
                                                        const std::string& remoteIp, uint16_t remotePort);

public:
    public:
    /**
     * @brief                                       http server是否有效
     * @return                                      MyStatus_t状态码
     */
    static MY_COMMON::MyStatus_t                    IsHttpServValid();
    
    /**
     * @brief                                       获取http server
     * @return                                      获取结果，0-success，-1-failed
     * @param servWkPtr                             http server弱引用
     */
    static MY_COMMON::MyStatus_t                    GetHttpServer(MyHttpServer::SmtWkPtr& servWkPtr);

    /**
     * @brief                                       获取http server id
     * @return                                      获取结果，0-success，-1-failed
     * @param id                                    http server id
     */
    static MY_COMMON::MyStatus_t                    GetHttpServId(std::string& id);

    /**
     * @brief                                       获取http server地址
     * @return                                      获取结果，0-success，-1-failed
     * @param servAddrCfg                           http server地址
     */
    static MY_COMMON::MyStatus_t                    GetHttpServAddrCfg(MY_COMMON::MyHttpServAddrCfg_dt& servAddrCfg);
};

}; // namespace MY_MANAGER








