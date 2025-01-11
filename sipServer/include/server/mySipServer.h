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
    typedef pj_pool_t*                              ServPoolPtr;
    typedef std::shared_ptr<MyServerAddrConfig_dt>  ServCfgPtr;
    typedef pjsip_endpoint*                         ServEndpointPtr;
    typedef pjmedia_endpt*                          ServMediaEndpointPtr;
    typedef void*                                   ServEvThreadCbParamPtr;   
    typedef pj_thread_t*                            ServEvThreadPtr;  

public:
    MySipServer(bool autoInit = true);
    ~MySipServer();

public:
    //                          sip服务事件监听
    static int                  OnSipServerEvCb(ServEvThreadCbParamPtr args);
;
public:
    /**
     * @brief                   初始化sip服务
     * @return                  初始化结果，0-success，-1-failed
     */             
    MyStatus_t                  init();

    /**     
     * @brief                   关闭sip服务
     * @return                  初始化结果，0-success，-1-failed
     */         
    MyStatus_t                  shutdown();

public:     
    /**     
     * @brief                   sip服务是否启动
     * @return                  启动结果，0-success，-1-failed
     */
    inline MyStatus_t           isStarted() const { return m_isStarted.load(); }

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

    /**     
     * @brief                   初始化pjsip线程
     * @return                  初始化结果，0-success，-1-failed
     */     
    MyStatus_t                  initThread();    

private:
    //                          服务是否启动 
    std::atomic<MyStatus_t>     m_isStarted;

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
    ServPoolPtr                 m_evCbPoolPtr;

    //                          pjsip事件回调函数所在线程
    ServEvThreadPtr             m_evThreadPtr;
};

}; //namespace MY_SERVER