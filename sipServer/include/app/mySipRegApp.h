#pragma once
#include <map>
#include <memory>
#include <string>
#include <atomic>
#include <cstdbool>
#include <Poller/Timer.h>
#include <Poller/EventPoller.h>
#include <boost/thread/shared_mutex.hpp>
#include "common/myTypeDef.h"

namespace MY_SERVER { class MySipServer; };

namespace MY_APP {
    
/**
 * sip应用注册类
 */
class MySipRegApp : public std::enable_shared_from_this<MySipRegApp>
{
public:
    typedef std::weak_ptr<MY_SERVER::MySipServer>               SipServSmtWkPtr;
    typedef std::shared_ptr<MY_SERVER::MySipServer>             SipServSmtPtr;
    typedef std::shared_ptr<MySipRegApp>                        SipRegAppSmtPtr;
    typedef std::weak_ptr<MySipRegApp>                          SipRegAppSmtWkPtr;

public:
    /**
     * @brief                                                   回调函数：sip注册响应
     * @param regParamPtr                                       回调参数
     */                         
    static void                                                 OnRegRespCb(MY_COMMON::SipRegCbParamPtr regParamPtr);

    /**             
     * @brief                                                   回调函数：sip注册鉴权信息填充
     * @return                                                  填充结果，0-success，-1-failed
     * @param pool                                              内存池
     * @param realm                                             鉴权域
     * @param name                                              用户名
     * @param credInfo                                          鉴权信息
     */                 
    static pj_status_t                                          OnRegFillAuthInfoCb(MY_COMMON::SipPoolPtr pool, MY_COMMON::SipStrCstPtr realm, 
                                                                                    MY_COMMON::SipStrCstPtr name, MY_COMMON::SipCredInfoPtr credInfo);

public:
    MySipRegApp();
    ~MySipRegApp();

public:
    /**     
     * @brief                                                   初始化
     * @return                                                  初始化结果，0-success，-1-failed
     * @param servPtr                                           sip服务对象
     * @param id                                                应用ID
     * @param name                                              应用名称
     * @param priority                                          应用优先级
     */                         
    MY_COMMON::MyStatus_t                                       init(SipServSmtWkPtr servPtr, 
                                                                     const std::string& id, 
                                                                     const std::string& name, 
                                                                     pjsip_module_priority priority);

    /**                 
     * @brief                                                   运行
     * @return                                                  运行结果，0-success，-1-failed
     */                 
    MY_COMMON::MyStatus_t                                       run();

    /**                 
     * @brief                                                   关闭
     * @return                                                  关闭结果，0-success，-1-failed
     */                 
    MY_COMMON::MyStatus_t                                       shutdown();

public:
    /**
     * @brief                                                   向上级sip服务发起注册
     * @return                                                  注册结果，0-success，-1-failed
     * @param cfg                                               上级sip服务配置
     * @param servAddr                                          本地sip服务地址配置
     * @param endptPtr                                          endpoint
     */             
    MY_COMMON::MyStatus_t                                       regUpServ(const MY_COMMON::SipRegUpServCfg& cfg, 
                                                                          const MY_COMMON::SipServAddrCfg& servAddr, 
                                                                          MY_COMMON::SipEndptPtr endptPtr);

public:             

    /**             
     * @brief                                                   定时器回调
     * @return                                                  是否继续触发定时器，true-继续，false-停止
     */             
    bool                                                        onTimer();

    /**             
     * @brief                                                   处理下级sip服务的sip注册请求消息
     * @return                                                  处理结果，0-success，-1-failed
     * @param rxDataPtr                                         sip注册请求消息
     */
    MY_COMMON::MyStatus_t                                       onRecvSipRegReqMsg(MY_COMMON::SipRxDataPtr rxDataPtr);

public:             
    /**                     
     * @brief                                                   sip app是否启动
     * @return                                                  启动结果，0-success，-1-failed
     */                             
    inline MY_COMMON::MyStatus_t                                getState() const { return m_status.load(); }

    /**             
     * @brief                                                   获取应用id
     * @return                                                  应用id
     */                 
    inline std::string                                          getId() const { return (nullptr != m_appIdPtr ? m_appIdPtr->id : ""); }

    /**             
     * @brief                                                   获取sip服务实例
     * @return                                                  sip服务实例
     */             
    inline SipRegAppSmtWkPtr                                    getSipRegApp() { return this->shared_from_this(); }

private:    
    //                                                          应用模块ID
    MY_COMMON::SipIdSmtPtr                                      m_appIdPtr;

    //                                                          启动状态 
    std::atomic<MY_COMMON::MyStatus_t>                          m_status; 

    //                                                          服务指针
    SipServSmtWkPtr                                             m_servSmtWkPtr;

    //                                                          读写互斥量
    boost::shared_mutex                                         m_rwMutex;

    //                                                          定时器
    toolkit::Timer::Ptr                                         m_timePtr;

    // key = up ref server id, value = sip up reg serv info ptr
    std::map<std::string, MY_COMMON::SipUpRegServInfoSmtPtr>    m_regUpServMap;

    // key = low ref server id, value = sip low reg serv info ptr
    std::map<std::string, MY_COMMON::SipLowRegServInfoSmtPtr>   m_regLowServMap;
    
};

}; // namespace MY_APP