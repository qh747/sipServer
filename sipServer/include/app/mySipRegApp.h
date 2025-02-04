#pragma once
#include <map>
#include <memory>
#include <string>
#include <atomic>
#include <cstdbool>
#include <pjsip.h>
#include <pjlib.h>
#include <pjsip_ua.h>
#include <pjsip/sip_msg.h>
#include <Poller/Timer.h>
#include <Poller/EventPoller.h>
#include <boost/thread/shared_mutex.hpp>
#include "common/myDataDef.h"
#include "common/myConfigDef.h"

namespace MY_SERVER { class MySipServer; };

namespace MY_APP {
    
/**
 * sip应用注册类
 */
class MySipRegApp : public std::enable_shared_from_this<MySipRegApp>
{
public:
    typedef pjsip_endpoint*                                 SipAppEndptPtr; 
    typedef pjsip_regc*                                     SipAppRegcPtr;
    typedef pjsip_hdr*                                      SipAppMsgHdrPtr;
    typedef pjsip_rx_data*                                  SipAppRxDataPtr;
    typedef pjsip_tx_data*                                  SipAppTxDataPtr;
    typedef pjsip_date_hdr*                                 SipAppMsgDateHdrPtr;
    typedef struct pjsip_regc_cbparam*                      SipAppRegCbParamPtr;
    typedef std::shared_ptr<MY_COMMON::MySipAppIdCfg_dt>    SipAppIdSmtPtr;
    typedef std::weak_ptr<MY_SERVER::MySipServer>           SipServSmtWkPtr;
    typedef std::shared_ptr<MY_SERVER::MySipServer>         SipServSmtPtr;
    typedef std::shared_ptr<MySipRegApp>                    SipRegAppSmtPtr;
    typedef std::weak_ptr<MySipRegApp>                      SipRegAppSmtWkPtr;
    typedef MY_COMMON::MySipRegUpServCfg_dt                 SipRegUpServCfg;
    typedef MY_COMMON::MySipServAddrCfg_dt                  SipServAddrCfg;
    typedef MY_COMMON::MySipUpRegServInfo_dt                SipUpRegServInfo;
    typedef std::shared_ptr<SipUpRegServInfo>               SipUpRegServInfoPtr;
    typedef MY_COMMON::MySipLowRegServInfo_dt               SipLowRegServInfo;
    typedef std::shared_ptr<SipLowRegServInfo>              SipLowRegServInfoPtr;

public:
    /**
     * @brief                                   回调函数：sip注册响应
     * @param paramPtr                          回调参数
     */         
    static void                                 OnRegRespCb(SipAppRegCbParamPtr paramPtr);

public:
    MySipRegApp();
    ~MySipRegApp();

public:
    /**     
     * @brief                                   初始化
     * @return                                  初始化结果，0-success，-1-failed
     * @param servPtr                           sip服务对象
     * @param id                                应用ID
     * @param name                              应用名称
     * @param priority                          应用优先级
     */         
    MY_COMMON::MyStatus_t                       init(SipServSmtWkPtr servPtr, const std::string& id, const std::string& name, pjsip_module_priority priority);

    /** 
     * @brief                                   运行
     * @return                                  运行结果，0-success，-1-failed
     */ 
    MY_COMMON::MyStatus_t                       run();

    /** 
     * @brief                                   关闭
     * @return                                  关闭结果，0-success，-1-failed
     */ 
    MY_COMMON::MyStatus_t                       shutdown();

public:
    /**
     * @brief                                   向上级sip服务发起注册
     * @return                                  注册结果，0-success，-1-failed
     * @param cfg                               上级sip服务配置
     * @param servAddr                          本地sip服务地址配置
     * @param endptPtr                          endpoint
     */
    MY_COMMON::MyStatus_t                       regUpServ(const SipRegUpServCfg& cfg, const SipServAddrCfg& servAddr, SipAppEndptPtr endptPtr);

public:

    /**
     * @brief                                   定时器回调
     * @return                                  是否继续触发定时器，true-继续，false-停止
     */
    bool                                        onTimer();

    /**
     * @brief                                   处理下级sip服务的sip注册请求消息
     * @return                                  处理结果，0-success，-1-failed
     * @param rxDataPtr                         sip注册请求消息
     */
    MY_COMMON::MyStatus_t                       onRecvSipRegReqMsg(SipAppRxDataPtr rxDataPtr);

public:
    /**     
     * @brief                                   sip app是否启动
     * @return                                  启动结果，0-success，-1-failed
     */             
    inline MY_COMMON::MyStatus_t                getState() const { return m_status.load(); }

    /**
     * @brief                                   获取应用id
     * @return                                  应用id
     */ 
    inline std::string                          getId() const { return (nullptr != m_appIdPtr ? m_appIdPtr->id : ""); }

    /**
     * @brief                                   获取sip服务实例
     * @return                                  sip服务实例
     */
    inline SipRegAppSmtWkPtr                    getSipRegApp() { return this->shared_from_this(); }

private:    
    //                                          应用模块ID
    SipAppIdSmtPtr                              m_appIdPtr;

    //                                          启动状态 
    std::atomic<MY_COMMON::MyStatus_t>          m_status; 

    //                                          服务指针
    SipServSmtWkPtr                             m_servSmtWkPtr;

    //                                          读写互斥量
    boost::shared_mutex                         m_rwMutex;

    //                                          定时器
    toolkit::Timer::Ptr                         m_timePtr;

    // key = up ref server id, value = sip up reg serv info ptr
    std::map<std::string, SipUpRegServInfoPtr>  m_regUpServMap;

    // key = low ref server id, value = sip low reg serv info ptr
    std::map<std::string, SipLowRegServInfoPtr> m_regLowServMap;
    
};

}; // namespace MY_APP