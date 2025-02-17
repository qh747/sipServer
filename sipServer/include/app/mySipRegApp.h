#pragma once
#include <map>
#include <memory>
#include <string>
#include <atomic>
#include <cstdbool>
#include <Poller/Timer.h>
#include <Poller/EventPoller.h>
#include "common/myTypeDef.h"

namespace MY_APP {
    
/**
 * sip应用注册类
 */
class MySipRegApp : public std::enable_shared_from_this<MySipRegApp>
{
public:
    typedef std::shared_ptr<MySipRegApp>    SmtPtr;
    typedef std::weak_ptr<MySipRegApp>      SmtWkPtr;

public:
    /**
     * @brief                               回调函数：sip注册响应
     * @param regParamPtr                   回调参数
     */                         
    static void                             OnRegRespCb(MY_COMMON::MySipRegCbParamPtr regParamPtr);

    /**             
     * @brief                               回调函数：sip注册鉴权信息填充
     * @return                              填充结果，0-success，-1-failed
     * @param pool                          内存池
     * @param realm                         鉴权域
     * @param name                          用户名
     * @param credInfo                      鉴权信息
     */                 
    static pj_status_t                      OnRegFillAuthInfoCb(MY_COMMON::MySipPoolPtr   poolPtr, MY_COMMON::MySipStrCstPtr   realmPtr, 
                                                                MY_COMMON::MySipStrCstPtr namePtr, MY_COMMON::MySipCredInfoPtr credInfoPtr);

    /**             
     * @brief                               回调函数：sip保活应答处理
     * @param evParamPtr                    回调参数
     * @param evPtr                         回调事件
     */ 
    static void                             OnKeepAliveRespCb(MY_COMMON::MySipEvThdCbParamPtr evParamPtr, 
                                                              MY_COMMON::MySipEvPtr           evPtr);

public:
    MySipRegApp();
    ~MySipRegApp();

public:
    /**     
     * @brief                               初始化
     * @return                              初始化结果，0-success，-1-failed
     * @param servId                        sip服务ID
     * @param id                            应用ID
     * @param name                          应用名称
     * @param priority                      应用优先级
     */                         
    MY_COMMON::MyStatus_t                   init(const std::string&    servId, 
                                                 const std::string&    id, 
                                                 const std::string&    name, 
                                                 pjsip_module_priority priority);

    /**                 
     * @brief                               运行
     * @return                              运行结果，0-success，-1-failed
     */                 
    MY_COMMON::MyStatus_t                   run();

    /**                 
     * @brief                               关闭
     * @return                              关闭结果，0-success，-1-failed
     */                 
    MY_COMMON::MyStatus_t                   shutdown();

public:             
    /**             
     * @brief                               处理下级sip服务的sip注册请求消息
     * @return                              处理结果，0-success，-1-failed
     * @param rxDataPtr                     sip注册请求消息
     */
    MY_COMMON::MyStatus_t                   onRecvSipRegReqMsg(MY_COMMON::MySipRxDataPtr rxDataPtr);

    /**             
     * @brief                               处理下级sip服务的sip保活请求消息
     * @return                              处理结果，0-success，-1-failed
     * @param rxDataPtr                     sip保活请求消息
     */
    MY_COMMON::MyStatus_t                   onRecvSipKeepAliveReqMsg(MY_COMMON::MySipRxDataPtr rxDataPtr);   

public:             
    /**                     
     * @brief                               sip app是否启动
     * @return                              启动结果，0-success，-1-failed
     */                             
    inline MY_COMMON::MyStatus_t            getState() const { return m_status.load(); }

    /**             
     * @brief                               获取应用id
     * @return                              应用id
     */                 
    inline std::string                      getId() const { return m_appIdCfg.id; }

    /**             
     * @brief                               获取sipRegApp实例
     * @return                              sipRegApp实例
     */             
    inline SmtWkPtr                         getSipRegApp() { return this->shared_from_this(); }

private:
    /**             
     * @brief                               定时器回调
     * @return                              是否继续触发定时器，true-继续，false-停止
     */             
    bool                                    onTimer();
    
    /**
     * @brief                               向上级sip服务发起注册
     * @return                              注册结果，0-success，-1-failed
     * @param regUpServCfg                  上级sip注册服务配置
     * @param localServCfg                  本地sip服务地址配置
     */             
    MY_COMMON::MyStatus_t                   regUpServ(const MY_COMMON::MySipRegUpServCfg_dt& regUpServCfg, 
                                                      const MY_COMMON::MySipServAddrCfg_dt&  localServCfg);

    /**
     * @brief                               向上级sip服务发起保活
     * @return                              保活结果，0-success，-1-failed
     * @param regUpServCfg                  上级sip注册服务配置
     * @param localServCfg                  本地sip服务地址配置
     */             
    MY_COMMON::MyStatus_t                   keepAliveUpServ(const MY_COMMON::MySipRegUpServCfg_dt& regUpServCfg,
                                                            const MY_COMMON::MySipServAddrCfg_dt&  localServCfg);                                                       

private:
    //                                      sip服务ID
    std::string                             m_servId;

    //                                      应用模块ID
    MY_COMMON::MySipAppIdCfg_dt             m_appIdCfg;

    //                                      启动状态 
    std::atomic<MY_COMMON::MyStatus_t>      m_status; 

    //                                      定时器
    toolkit::Timer::Ptr                     m_timePtr;
};

}; // namespace MY_APP