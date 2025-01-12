#pragma once
#include <mutex>
#include <memory>
#include <atomic>
#include <cstdbool>
#include <pjlib.h>
#include <pjlib-util.h>
#include <pjsip.h>
#include <pjmedia.h>
#include <pjsip_ua.h>
#include <pjsip/sip_auth.h>
#include "common/myDataDef.h"
using MY_COMMON::MyStatus_t;
using MY_COMMON::MyServerAddrConfig_dt;

namespace MY_SERVER {

/**
 * sip服务类
 */
class MySipServer 
{
public:
    typedef MySipServer*                            ServPtr;
    typedef pj_pool_t*                              ServThreadPoolPtr;
    typedef std::shared_ptr<MyServerAddrConfig_dt>  ServCfgPtr;
    typedef pjsip_endpoint*                         ServEndpointPtr;
    typedef pjmedia_endpt*                          ServMediaEndpointPtr;
    typedef void*                                   ServEvThreadCbParamPtr;   
    typedef pj_thread_t*                            ServEvThreadPtr;  

public:
    MySipServer(bool autoInit = true);
    ~MySipServer();

public:
    /**
     * @brief                   回调函数: sip服务事件监听
     * @return                  0-success，-1-failed
     * @param args              线程函数传入参数
     */
    static int                  OnSipServerEvCb(ServEvThreadCbParamPtr args);
;
public:
    /**
     * @brief                   初始化sip服务
     * @return                  初始化结果，0-success，-1-failed
     */             
    MyStatus_t                  init();

    /**
     * @brief                   启动sip服务
     * @return                  启动结果，0-success，-1-failed
     */             
    MyStatus_t                  run();

    /**     
     * @brief                   关闭sip服务
     * @return                  初始化结果，0-success，-1-failed
     */         
    MyStatus_t                  shutdown();

public:
    /**
     * @brief                   获取服务信息
     * @return                  服务信息
     */
    std::string                 getServerInfo();

public:     
    /**     
     * @brief                   sip服务是否启动
     * @return                  启动结果，0-success，-1-failed
     */
    inline MyStatus_t           isStarted() const { return m_startState.load(); }

    /**
     * @brief                   获取服务名称
     * @return                  服务名称
     */
    inline std::string          getServerName() const { return (nullptr != m_cfgPtr ? m_cfgPtr->name : ""); }

    /**
     * @brief                   获取服务域名
     * @return                  服务域名
     */
    inline std::string          getServeDomain() const { return (nullptr != m_cfgPtr ? m_cfgPtr->domain : ""); }

    /**
     * @brief                   获取服务ID
     * @return                  服务ID
     */
    inline std::string          getServerId() const { return (nullptr != m_cfgPtr ? m_cfgPtr->id : ""); }

    /**
     * @brief                   获取服务IP地址
     * @return                  服务IP地址
     */
    inline std::string          getServerIp() const { return (nullptr != m_cfgPtr ? m_cfgPtr->ipAddr : ""); }

    /**
     * @brief                   获取服务端口
     * @return                  服务端口
     */
    inline uint16_t             getServerPort() const { return (nullptr != m_cfgPtr ? m_cfgPtr->port : 0); }

private:
    /**
     * @brief                   初始化配置
     * @return                  初始化结果，0-success，-1-failed
     */     
    MyStatus_t                  initConfig();

    /**     
     * @brief                   初始化pjsip库
     */     
    MyStatus_t                  initPjsipLib();

    /**     
     * @brief                   初始化pjsip模块
     * @return                  初始化结果，0-success，-1-failed
     */     
    MyStatus_t                  initModule(); 

private:
    //                          启动状态 
    std::atomic<MyStatus_t>     m_startState;

    //                          服务多线程互斥量
    std::recursive_mutex        m_lock;

    //                          服务配置
    ServCfgPtr                  m_cfgPtr;

    //                          pjsip内存池
    pj_caching_pool             m_cachingPool;

    //                          pjsip endpoint
    ServEndpointPtr             m_endpointPtr;

    //                          pjsip media endpoint
    ServMediaEndpointPtr        m_mediaEndptPtr;

    //                          pjsip事件回调函数所在线程使用的内存地址
    ServThreadPoolPtr           m_servThreadPoolPtr;

    //                          pjsip事件回调函数所在线程
    ServEvThreadPtr             m_evThreadPtr;
};

}; //namespace MY_SERVER