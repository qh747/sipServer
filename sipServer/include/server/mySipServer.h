#pragma once
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
using MY_COMMON::MyServerAddrConfig_dt;

namespace MY_SERVER {
/**
 * sip服务类
 */
class MySipServer 
{
public:
    typedef std::shared_ptr<MyServerAddrConfig_dt> servCfgPtr;

public:
    MySipServer(bool autoInit = true);
    ~MySipServer();

public:
    /**
     * @brief           初始化sip服务
     * @return          初始化结果，true-初始化成功，false-初始化失败
     */     
    bool                init();

    /**
     * @brief           关闭sip服务
     * @return          初始化结果，true-关闭成功，false-关闭失败
     */  
    bool                shutdown();

public:
    /**
     * @brief           sip服务是否启动
     * @return          启动结果，true-启动成功，false-启动失败
     */
    inline bool         isStarted() const { return m_isStarted.load(); }

private:
    /**
     * @brief           初始化配置
     * @return          初始化结果，true-初始化成功，false-初始化失败
     */
    pj_status_t         initConfig();

    /**
     * @brief           初始化pjsip库
     */
    pj_status_t         initPjsipLib();

    /**
     * @brief           初始化pjsip模块
     * @return          初始化结果，true-初始化成功，false-初始化失败
     */
    pj_status_t         initModule();

    /**
     * @brief           注册pjsip模块
     * @return          注册结果，true-注册成功，false-注册失败
     */
    pj_status_t         registerModule();      

private:
    //                  服务是否启动 
    std::atomic<bool>   m_isStarted;

    //                  服务配置
    servCfgPtr          m_cfgPtr;
    
    //                  pjsip内存池
    pj_caching_pool     m_cachingPool;

    //                  pjsip endpoint
    pjsip_endpoint*     m_endpointPtr;

    //                  pjsip media endpoint
    pjmedia_endpt*      m_mediaEndptPtr;
};

}; //namespace MY_SERVER