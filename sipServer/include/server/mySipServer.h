#pragma once
#include <string>
#include <atomic>
#include <memory>
#include <cstdbool>
#include "common/myTypeDef.h"
#include "envir/mySystemPjsip.h"

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
    static int                              SipServerThdFunc(MY_COMMON::MySipEvThdCbParamPtr arg);

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
     * @brief                               回调函数: sip注册请求接收
     * @return                              处理结果，0-success，-1-failed
     * @param regReqMsgPtr                  注册请求消息指针
     */
    MY_COMMON::MyStatus_t                   onRecvSipRegMsg(MY_COMMON::MySipRxDataPtr regReqMsgPtr);

    /**
     * @brief                               回调函数: sip保活请求接收
     * @return                              处理结果，0-success，-1-failed
     * @param regReqMsgPtr                  保活请求消息指针
     */
    MY_COMMON::MyStatus_t                   onRecvSipKeepAliveMsg(MY_COMMON::MySipRxDataPtr keepAliveReqMsgPtr);

public:     
    /**     
     * @brief                               sip服务是否启动
     * @return                              启动结果，0-success，-1-failed
     */         
    inline MY_COMMON::MyStatus_t            getState() { return m_status.load(); }

    /**
     * @brief                               获取sip服务实例
     * @return                              sip服务实例
     */
    inline MySipServer::SmtWkPtr            getSipServer() { return this->shared_from_this(); }

    /**
     * @brief                               获取sip服务地址配置
     * @return                              sip服务地址配置
     */
    inline MY_COMMON::MySipServAddrCfg_dt   getSipServAddrCfg() const { return m_servAddrCfg; }
    
    /**
     * @brief                               获取pjsip服务端点
     * @return                              pjsip服务端点
     */
    inline MY_COMMON::MySipEndptPtr         getSipEndptPtr() const { return m_servEndptPtr; }

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