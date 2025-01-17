#pragma once
#include <map>
#include <memory>
#include <string>
#include <atomic>
#include <pjsip.h>
#include <pjlib.h>
#include <pjsip_ua.h>
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
    typedef pjsip_tx_data*                                  SipAppTxDataPtr;
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
    MY_COMMON::MyStatus_t                       startRegUpServ(const SipRegUpServCfg& cfg, const SipServAddrCfg& servAddr, SipAppEndptPtr endptPtr);

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

    // key = up ref server id, value = sip up reg serv info ptr
    std::map<std::string, SipUpRegServInfoPtr>  m_regUpServMap;
};

}; // namespace MY_APP