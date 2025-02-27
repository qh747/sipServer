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
     * @brief                                       SipServer对象是否有效
     * @return                                      MyStatus_t状态码
     */
    static MY_COMMON::MyStatus_t                    IsSipServValid();
    
    /**
     * @brief                                       获取SipServer对象
     * @return                                      获取结果，0-success，-1-failed
     * @param sipServWkPtr                          sip服务弱引用指针
     */
    static MY_COMMON::MyStatus_t                    GetSipServer(MySipServer::SmtWkPtr& sipServWkPtr);

    /**
     * @brief                                       获取SipServer ID
     * @return                                      获取结果，0-success，-1-failed
     * @param id                                    SipServer ID
     */
    static MY_COMMON::MyStatus_t                    GetSipServId(std::string& id);

    /**
     * @brief                                       获取SipServer地址配置
     * @return                                      获取结果，0-success，-1-failed
     * @param sipServAddrCfg                        sip服务地址配置
     */
    static MY_COMMON::MyStatus_t                    GetSipServAddrCfg(MY_COMMON::MySipServAddrCfg_dt& sipServAddrCfg);

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
    static MY_COMMON::MyStatus_t                    GetSipServTcpTp(MY_COMMON::MySipTransportPtrAddr tcpTpPtrAddr, const std::string& remoteIp, uint16_t remotePort);
 
    /**
     * @brief                                       获取sip服务注册使用的tcp transport工厂
     * @return                                      获取结果，0-success，-1-failed
     * @param tcpTpPtrAddr                          sip服务tcp transport
     * @param remoteIpAddr                          远端ip地址
     * @param remotePort                            远端端口
     */
    static MY_COMMON::MyStatus_t                    GetSipServRegTcpTp(MY_COMMON::MySipTransportPtrAddr tcpTpPtrAddr, const std::string& remoteIp, uint16_t remotePort);
};

}; // namespace MY_MANAGER








