#pragma once
#include <atomic>
#include <memory>
#include <cstdbool>
#include "common/myTypeDef.h"

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
    static int                              SipServerThdFunc(MY_COMMON::MySipCbParamPtr arg);

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
    MY_COMMON::MyStatus_t                   onSipServRecvSipRegisterReqMsg(MY_COMMON::MySipRxDataPtr regReqMsgPtr);

    /**
     * @brief                               回调函数: sip保活请求消息接收
     * @return                              处理结果，0-success，-1-failed
     * @param keepAliveReqMsgPtr            保活请求消息
     */
    MY_COMMON::MyStatus_t                   onSipServRecvSipKeepAliveReqMsg(MY_COMMON::MySipRxDataPtr keepAliveReqMsgPtr);

    /**
     * @brief                               回调函数: sip设备目录查询请求消息接收
     * @return                              处理结果，0-success，-1-failed
     * @param catalogQueryReqMsgPtr         sip设备目录查询请求消息
     */
    MY_COMMON::MyStatus_t                   onSipServRecvSipCatalogQueryReqMsg(MY_COMMON::MySipRxDataPtr catalogQueryReqMsgPtr);

    /**
     * @brief                               回调函数: sip设备目录响应请求消息接收
     * @return                              处理结果，0-success，-1-failed
     * @param catalogResponseReqMsgPtr      sip设备目录响应请求消息
     */
    MY_COMMON::MyStatus_t                   onSipServRecvSipCatalogResponseReqMsg(MY_COMMON::MySipRxDataPtr catalogResponseReqMsgPtr);

public:
    /**
     * @brief                               向下级sip注册服务请求设备目录
     * @return                              处理结果，0-success，-1-failed
     * @param lowSipRegServAddrCfg          sip注册服务地址配置
     */
    MY_COMMON::MyStatus_t                   onReqLowSipServCatalog(const MY_COMMON::MySipRegLowServCfg_dt& lowSipRegServAddrCfg);

public:     
    /**     
     * @brief                               sip服务状态
     * @return                              获取结果，0-success，-1-failed
     * @param status                        sip服务状态
     */         
    MY_COMMON::MyStatus_t                   getState(MY_COMMON::MyStatus_t& status) const;

    /**
     * @brief                               获取sip服务
     * @return                              获取结果，0-success，-1-failed
     * @param sipServer                     sip服务
     */
     MY_COMMON::MyStatus_t                  getSipServer(MySipServer::SmtWkPtr& sipServer);

    /**
     * @brief                               获取sip服务地址配置
     * @return                              获取结果，0-success，-1-failed
     * @param cfg                           sip服务地址配置
     */
    MY_COMMON::MyStatus_t                   getSipServAddrCfg(MY_COMMON::MySipServAddrCfg_dt& cfg) const;

private:
    //                                      启动状态 
    std::atomic<MY_COMMON::MyStatus_t>      m_status;  

    //                                      sip服务地址配置
    MY_COMMON::MySipServAddrCfg_dt          m_servAddrCfg;

    //                                      pjsip服务端点
    MY_COMMON::MySipEndptPtr                m_servEndptPtr;

    //                                      pjsip事件回调函数所在线程
    MY_COMMON::MySipThdPtr                  m_servEvThdPtr;   

    //                                      pjsip事件回调函数所在线程使用的内存地址
    MY_COMMON::MySipPoolPtr                 m_servThdPoolPtr;
};

}; //namespace MY_SERVER