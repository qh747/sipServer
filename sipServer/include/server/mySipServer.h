#pragma once
#include <memory>
#include <cstdbool>
#include <boost/thread/shared_mutex.hpp>
#include "common/myTypeDef.h"
#include "event/myEvent.h"

namespace MY_SERVER {

/**
 * sip服务类
 */
class MySipServer : public std::enable_shared_from_this<MySipServer>
{
public:
    typedef MySipServer*                    Ptr;
    typedef std::shared_ptr<MySipServer>    SmtPtr;
    typedef std::weak_ptr<MySipServer>      SmtWkPtr;

public:
    /**
     * @brief                               sip服务事件线程回调函数
     * @return                              回调结果，0-success，-1-failed
     * @param arg                           回调参数
     */
    static int                              ServThdFunc(MY_COMMON::MyFuncCbParamPtr arg);

    /**
     * @brief                               sip服务监听事件回调函数
     * @param deviceId                      设备id
     * @param status                        处理状态
     * @param info                          处理信息
     */
    static void                             ServMediaReqEvFunc(MY_MEDIA_REQ_EV_ARGS);

public:         
    MySipServer();          
    ~MySipServer();         

public:         
    /**         
     * @brief                               初始化sip服务
     * @return                              初始化结果，0-success，-1-failed
     * @param addrCfg                       sip服务地址配置
     * @param evThdMemCfg                   sip服务事件线程内存配置
     */             
    MY_COMMON::MyStatus_t                   init(const MY_COMMON::MySipServAddrCfg_dt& addrCfg, 
                                                 const MY_COMMON::MySipEvThdMemCfg_dt& evThdMemCfg);

    /**
     * @brief                               启动sip服务
     * @return                              启动结果，0-success，-1-failed
     */             
    MY_COMMON::MyStatus_t                   run();

    /**     
     * @brief                               关闭sip服务
     * @return                              初始化结果，0-success，-1-failed
     */         
    MY_COMMON::MyStatus_t                   shutdown();

public:
    /**
     * @brief                               回调函数: sip注册请求消息接收
     * @return                              处理结果，0-success，-1-failed
     * @param regReqMsgPtr                  注册请求消息
     */
    MY_COMMON::MyStatus_t                   onServRecvSipRegReqMsg(MY_COMMON::MySipRxDataPtr regReqMsgPtr);

    /**
     * @brief                               回调函数: sip保活请求消息接收
     * @return                              处理结果，0-success，-1-failed
     * @param keepAliveReqMsgPtr            保活请求消息
     */
    MY_COMMON::MyStatus_t                   onServRecvSipKeepAliveReqMsg(MY_COMMON::MySipRxDataPtr keepAliveReqMsgPtr);

    /**
     * @brief                               回调函数: sip设备目录查询请求消息接收
     * @return                              处理结果，0-success，-1-failed
     * @param catalogQueryReqMsgPtr         sip设备目录查询请求消息
     */
    MY_COMMON::MyStatus_t                   onServRecvSipCatalogQueryReqMsg(MY_COMMON::MySipRxDataPtr catalogQueryReqMsgPtr);

    /**
     * @brief                               回调函数: sip设备目录响应请求消息接收
     * @return                              处理结果，0-success，-1-failed
     * @param catalogResponseReqMsgPtr      sip设备目录响应请求消息
     */
    MY_COMMON::MyStatus_t                   onServRecvSipCatalogRespMsg(MY_COMMON::MySipRxDataPtr catalogResponseReqMsgPtr);

public:
    /**
     * @brief                               向下级sip注册服务请求设备目录
     * @return                              请求结果，0-success，-1-failed
     * @param lowSipRegServAddrCfg          sip注册服务地址配置
     */
    MY_COMMON::MyStatus_t                   onReqLowServCatalog(const MY_COMMON::MySipRegLowServCfg_dt& lowSipRegServAddrCfg);

    /**
     * @brief                               请求设备媒体流
     * @return                              请求结果，0-success，-1-failed
     * @param deviceId                      设备id
     * @param respInfo                      响应信息
     */
    MY_COMMON::MyStatus_t                   onReqDeviceMedia(const std::string& deviceId, std::string& respInfo);

public:     
    /**     
     * @brief                               sip服务状态
     * @return                              获取结果，0-success，-1-failed
     * @param status                        sip服务状态
     */         
    MY_COMMON::MyStatus_t                   getState(MY_COMMON::MyStatus_t& status) const;

    /**
     * @brief                               获取sip服务地址配置
     * @return                              获取结果，0-success，-1-failed
     * @param cfg                           sip服务地址配置
     */
    MY_COMMON::MyStatus_t                   getServAddrCfg(MY_COMMON::MySipServAddrCfg_dt& cfg);

    /**
     * @brief                               获取sip服务udp transport
     * @return                              获取结果，0-success，-1-failed
     * @param udpTpPtrAddr                  sip服务udp transport
     */
    MY_COMMON::MyStatus_t                   getServUdpTp(MY_COMMON::MySipTransportPtrAddr udpTpPtrAddr);

    /**
     * @brief                               获取sip服务注册使用的udp transport
     * @return                              获取结果，0-success，-1-failed
     * @param udpTpPtrAddr                  sip服务udp transport
     */
    MY_COMMON::MyStatus_t                   getServRegUdpTp(MY_COMMON::MySipTransportPtrAddr udpTpPtrAddr);

    /**
     * @brief                               获取sip服务tcp transport
     * @return                              获取结果，0-success，-1-failed
     * @param tcpTpPtrAddr                  sip服务tcp transport
     * @param remoteIpAddr                  远端ip地址
     * @param remotePort                    远端端口
     */
    MY_COMMON::MyStatus_t                   getServTcpTp(MY_COMMON::MySipTransportPtrAddr tcpTpPtrAddr, 
                                                           const std::string&             remoteIpAddr, 
                                                           uint16_t                       remotePort);

    /**
     * @brief                               获取sip服务注册使用的tcp transport工厂
     * @return                              获取结果，0-success，-1-failed
     * @param tcpTpPtrAddr                  sip服务tcp transport
     * @param remoteIpAddr                  远端ip地址
     * @param remotePort                    远端端口
     */
    MY_COMMON::MyStatus_t                   getServRegTcpTp(MY_COMMON::MySipTransportPtrAddr tcpTpPtrAddr, 
                                                                     const std::string&      remoteIpAddr, 
                                                                     uint16_t                remotePort);

private:
    //                                      启动状态 
    MY_COMMON::MyAtomicStatus               m_status; 
    
    //                                      sip服务多线程互斥量
    boost::shared_mutex                     m_rwMutex;

    //                                      sip服务地址配置
    MY_COMMON::MySipServAddrCfg_dt          m_servAddrCfg;

    //                                      pjsip服务端点
    MY_COMMON::MySipEndptPtr                m_servEndptPtr;

    //                                      pjsip事件回调函数所在线程
    MY_COMMON::MySipThdPtr                  m_servEvThdPtr;   

    //                                      pjsip事件回调函数所在线程使用的内存地址
    MY_COMMON::MySipPoolPtr                 m_servThdPoolPtr;

    //                                      pjsip local udp transport 
    MY_COMMON::MySipTransportPtr            m_servUdpTpPtr;

    //                                      pjsip local tcp transport factory
    MY_COMMON::MySipTransportFactoryPtr     m_servTcpTpFactoryPtr;

    //                                      pjsip local tcp transport map
    MY_COMMON::MySipServTpMap               m_servTcpTpMap;
    
    //                                      pjsip regist udp transport 
    MY_COMMON::MySipTransportPtr            m_servUdpRegTpPtr;
    
    //                                      pjsip regist tcp transport 
    MY_COMMON::MySipTransportFactoryPtr     m_servTcpRegTpFactoryPtr;

    //                                      pjsip regist tcp transport map
    MY_COMMON::MySipServTpMap               m_servTcpRegTpMap;
};

}; //namespace MY_SERVER