#pragma once
#include <string>
#include <atomic>
#include <memory>
#include <cstdbool>
#include "common/myDataDef.h"
#include "common/myConfigDef.h"
#include "envir/mySystemPjsip.h"

namespace MY_SERVER {

/**
 * sip服务类
 */
class MySipServer : public std::enable_shared_from_this<MySipServer>
{
public:
    typedef pjsip_endpoint*                     SipServEndptPtr;
    typedef pj_thread_t*                        SipServEvThreadPtr;  
    typedef pj_pool_t*                          SipServThreadPoolPtr;
    typedef void*                               SipServEvThreadCbParamPtr;
    typedef MY_COMMON::MySipServAddrCfg_dt      SipServAddrCfg;
    typedef std::shared_ptr<SipServAddrCfg>     SipServAddrCfgPtr;
    typedef MY_COMMON::MySipEvThdMemCfg_dt      SipServEvThdMemCfg;
    typedef std::shared_ptr<MySipServer>        SipServSmtPtr;
    typedef std::weak_ptr<MySipServer>          SipServSmtWkPtr;

private:
    typedef MySipServer*                        SipServPtr;

public:         
    /**         
     * @brief                               回调函数: sip服务事件监听
     * @return                              0-success，-1-failed
     * @param args                          线程函数传入参数
     */         
    static int                              OnSipServerEvCb(SipServEvThreadCbParamPtr args);

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
    MY_COMMON::MyStatus_t                   init(const SipServAddrCfg& addrCfg, const SipServEvThdMemCfg& evThdMemCfg);

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
    inline const SipServAddrCfg&            getSipServAddrCfg() const { return *m_servAddrCfgPtr; }

    /**
     * @brief                               获取sip服务端点
     * @return                              sip服务端点
     */
    inline SipServEndptPtr                  getSipServEndptPtr() { return m_servEndptPtr; }

private:
    //                                      启动状态 
    std::atomic<MY_COMMON::MyStatus_t>      m_status;  

    //                                      sip服务地址配置
    SipServAddrCfgPtr                       m_servAddrCfgPtr;

    //                                      pjsip服务端点
    SipServEndptPtr                         m_servEndptPtr;

    //                                      pjsip事件回调函数所在线程
    SipServEvThreadPtr                      m_servEvThdPtr;   

    //                                      pjsip事件回调函数所在线程使用的内存地址
    SipServThreadPoolPtr                    m_servThdPoolPtr;
};

}; //namespace MY_SERVER