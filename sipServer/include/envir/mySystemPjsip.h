#pragma once
#include <atomic>
#include <pjlib.h>
#include <pjlib-util.h>
#include <pjsip.h>
#include <pjmedia.h>
#include <pjsip_ua.h>
#include <pjsip/sip_auth.h>
#include "utils/myBaseHelper.h"
#include "common/myDataDef.h"

namespace MY_ENVIR {

/**
 * pjsip环境类
 */
class MySystemPjsip : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
     typedef pjsip_endpoint*                    SysPjsipEndpointPtr;
     typedef pjmedia_endpt*                     SysPjsipMediaEndpointPtr;

public:
    /**
     * @brief                                   初始化
     * @return                                  MyStatus_t状态码
     */                     
    static MY_COMMON::MyStatus_t                Init();

    /**                                     
     * @brief                                   关闭
     * @return                                  MyStatus_t状态码
     */                                         
    static MY_COMMON::MyStatus_t                Shutdown();

public: 
    /**         
     * @brief                                   状态
     * @return                                  0-success，-1-failed
     */             
    inline static MY_COMMON::MyStatus_t         State() { return SysPjsipState.load(); }

    /** 
     * @brief                                   获取pjsip endpoint指针
     * @return                                  pjsip endpoint指针
     */ 
    inline static SysPjsipEndpointPtr           GetPjsipEndptPtr() { return SysPjsipEndptPtr; }

    /** 
     * @brief                                   获取pjsip media endpoint指针
     * @return                                  pjsip media endpoint指针
     */ 
    inline static SysPjsipMediaEndpointPtr      GetPjsipMediaEndptPtr() { return SysPjsipMediaEndptPtr; }

private:    
    /**         
     * @brief                                   初始化pjsip库
     * @return                                  MyStatus_t状态码
     */                     
    static MY_COMMON::MyStatus_t                InitPjsipLib();

    /**         
     * @brief                                   初始化pjsip模块
     * @return                                  MyStatus_t状态码
     */                     
    static MY_COMMON::MyStatus_t                InitPjsipModule();

private:
    //                                          启动状态 
    static std::atomic<MY_COMMON::MyStatus_t>   SysPjsipState;

    //                                          pjsip内存池
    static pj_caching_pool                      SysPjsipCachingPool;

    //                                          pjsip endpoint指针
    static SysPjsipEndpointPtr                  SysPjsipEndptPtr;

    //                                          pjsip media endpoint指针
    static SysPjsipMediaEndpointPtr             SysPjsipMediaEndptPtr;
};

}; // namespace MY_ENVIR