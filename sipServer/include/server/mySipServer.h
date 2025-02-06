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
    typedef MySipServer*                        SipServPtr;
    typedef std::shared_ptr<MySipServer>        SipServSmtPtr;
    typedef std::weak_ptr<MySipServer>          SipServSmtWkPtr;

public:         
    /**         
     * @brief                               回调函数: sip服务事件监听
     * @return                              0-success，-1-failed
     * @param args                          线程函数传入参数
     */         
    static int                              OnSipServerEvCb(MY_COMMON::SipEvThdCbParamPtr args);

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
    MY_COMMON::MyStatus_t                   init(const MY_COMMON::SipServAddrCfg& addrCfg, const MY_COMMON::SipEvThdMemCfg& evThdMemCfg);

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
    MY_COMMON::MyStatus_t                   onRecvSipRegMsg(MY_COMMON::SipRxDataPtr regReqMsgPtr);

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
    inline SipServSmtWkPtr                  getSipServer() { return this->shared_from_this(); }

    /**
     * @brief                               获取sip服务地址配置
     * @return                              sip服务地址配置
     */
    inline const MY_COMMON::SipServAddrCfg& getSipServAddrCfg() const { return *m_servAddrCfgPtr; }

    /**
     * @brief                               获取sip服务端点
     * @return                              sip服务端点
     */
    inline MY_COMMON::SipEndptPtr           getSipServEndptPtr() { return m_servEndptPtr; }

private:
    //                                      启动状态 
    std::atomic<MY_COMMON::MyStatus_t>      m_status;  

    //                                      sip服务地址配置
    MY_COMMON::SipServAddrCfgSmtPtr         m_servAddrCfgPtr;

    //                                      pjsip服务端点
    MY_COMMON::SipEndptPtr                  m_servEndptPtr;

    //                                      pjsip事件回调函数所在线程
    MY_COMMON::SipThdPtr                    m_servEvThdPtr;   

    //                                      pjsip事件回调函数所在线程使用的内存地址
    MY_COMMON::SipPoolPtr                   m_servThdPoolPtr;
};

}; //namespace MY_SERVER