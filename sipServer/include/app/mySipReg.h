#pragma once
#include <map>
#include <mutex>
#include <string>
#include <atomic>
#include <pjsip.h>
#include <pjlib.h>
#include "common/myDataDef.h"
#include "utils/myBaseHelper.h"
#include "utils/myJsonHelper.h"
using MY_COMMON::MyStatus_t;
using MY_COMMON::MySipRegUpServResult_dt;
using MY_COMMON::MySipRegLowServResult_dt;
using MY_UTILS::MyJsonHelper;

namespace MY_APP {
    
/**
 * sip注册包装类
 */
class MySipRegWrapper : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    // key = reg up server id, value = reg up server info
    typedef std::map<std::string, MySipRegUpServResult_dt>      RegUpServResMap;
    // key = reg up server id, value = reg up server info
    typedef std::map<std::string, MySipRegLowServResult_dt>     RegLowServResMap;
    // 服务端线程池
    typedef pj_pool_t*                                          ServThreadPoolPtr;
    // 回调函数参数
    typedef void*                                               SipRegCbParamPtr;   
    // 子线程对象地址
    typedef pj_thread_t*                                        SipRegThreadPtr; 

public:
    /**     
     * @brief                       初始化
     * @return                      初始化结果，0-success，-1-failed
     * @param regFilePath           注册文件路径
     */         
    static MyStatus_t               Init(const std::string& regFilePath);

    /** 
     * @brief                       运行
     * @return                      运行结果，0-success，-1-failed
     * @param threadPoolPtr         服务端线程池
     */ 
    static MyStatus_t               Run(ServThreadPoolPtr threadPoolPtr);

    /** 
     * @brief                       关闭
     * @return                      关闭结果，0-success，-1-failed
     */ 
    static MyStatus_t               Shutdown();

private:
    /**
     * @brief                       注册上级服务
     * @return                      注册结果，0-success，-1-failed
     * @param regUpServ             上级服务注册信息
     */
    static MyStatus_t               RegUpServ(MySipRegUpServResult_dt& regUpServ);  

    /**
     * @brief                       回调函数：注册服务管理
     * @return                      0-success，-1-failed
     * @param args                  线程函数传入参数
     */
    static int                      OnRegServManageCb(SipRegCbParamPtr args);

    /**
     * @brief                       获取上级注册服务信息字符串
     * @return                      上级注册服务信息
     * @param regUpServ             上级服务注册信息
     */
    static std::string              GetUpRegServInfo(const MySipRegUpServResult_dt& regUpServ);

    /**
     * @brief                       获取下级注册服务信息字符串
     * @return                      下级注册服务信息
     * @param regUpServ             下级服务注册信息
     */
    static std::string              GetLowRegServInfo(const MySipRegLowServResult_dt& regLowServ);

private:    
    //                              上级注册服务map
    static RegUpServResMap          RegUpServMap;

    //                              下级注册服务map
    static RegLowServResMap         RegLowServMap;

    //                              启动状态
    static std::atomic<MyStatus_t>  StartState;

    //                              多线程互斥量
    static std::mutex               RegLock;

    static SipRegThreadPtr          RegManageThreadPtr;
};

}; // namespace MY_APP