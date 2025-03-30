#pragma once
#include "common/myTypeDef.h"
#include "utils/myBaseHelper.h"

namespace MY_ENVIR {

/**
 * pjsip环境类
 */
class MySystemPjsip : public MY_UTILS::MyNonConstructableAndNonCopyable
{
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

    /**
     * @brief                                   注册自定义线程
     */
    static void                                 RegistSelfThread();

public: 
    /**         
     * @brief                                   获取状态
     * @return                                  获取结果，0-success，-1-failed
     * @param status                            状态
     */             
    static MY_COMMON::MyStatus_t                GetState(MY_COMMON::MyStatus_t& status);

    /** 
     * @brief                                   获取pjsip endpoint
     * @return                                  获取结果
     * @param endptPtrAddr                      pjsip endpoint地址
     */ 
    static MY_COMMON::MyStatus_t                GetPjsipEndptPtr(MY_COMMON::MySipEndptPtrAddr endptPtrAddr);

    /** 
     * @brief                                   获取pjsip media endpoint
     * @return                                  获取结果，0-success，-1-failed
     * @param mediaEndptPtrAddr                 pjsip media endpoint地址
     */ 
    static MY_COMMON::MyStatus_t                GetPjsipMediaEndptPtr(MY_COMMON::MySipMediaEndptPtrAddr mediaEndptPtrAddr);

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
    static MY_COMMON::MyAtomicStatus            SysPjsipState;

    //                                          pjsip内存池
    static pj_caching_pool                      SysPjsipCachingPool;

    //                                          pjsip endpoint指针
    static MY_COMMON::MySipEndptPtr             SysPjsipEndptPtr;

    //                                          pjsip media endpoint指针
    static MY_COMMON::MySipMediaEndptPtr        SysPjsipMediaEndptPtr;
};

}; // namespace MY_ENVIR