#pragma once
#include <string>
#include <memory>
#include <pjsip.h>
#include "common/myDataDef.h"

namespace MY_SERVER { class MySipServer; };

namespace MY_APP {

/**
 * sip应用消息处理类
 */
class MySipMsgProcApp : public std::enable_shared_from_this<MySipMsgProcApp>
{
public:
    typedef pjsip_event*                                    SipAppEvPtr;
    typedef pjsip_transaction*                              SipAppTsxPtr;
    typedef pjsip_endpoint*                                 SipAppEndptPtr;
    typedef pjsip_rx_data*                                  SipAppRxDataPtr;
    typedef pjsip_tx_data*                                  SipAppTxDataPtr;
    typedef std::weak_ptr<MY_SERVER::MySipServer>           SipServSmtWkPtr;
    typedef std::shared_ptr<MY_SERVER::MySipServer>         SipServSmtPtr;
    typedef std::shared_ptr<pjsip_module>                   SipAppModSmtPtr;
    typedef std::shared_ptr<MY_COMMON::MySipAppIdCfg_dt>    SipAppIdSmtPtr;
    typedef std::shared_ptr<MySipMsgProcApp>                SipMsgProcAppSmtPtr;
    typedef std::weak_ptr<MySipMsgProcApp>                  SipMsgProcAppSmtWkPtr;

public:
    MySipMsgProcApp();
    ~MySipMsgProcApp();

public:
    /**
     * @brief                               应用初始化
     * @return                              初始化结果，0-success，-1-failed
     * @param servPtr                       sip服务对象
     * @param name                          应用名称
     * @param priority                      应用优先级
     */
    MY_COMMON::MyStatus_t                   init(SipServSmtWkPtr servPtr, const std::string& name, pjsip_module_priority priority);

    /**
     * @brief                               应用启动
     * @return                              启动结果，0-success，-1-failed
     */
    MY_COMMON::MyStatus_t                   run();

    /**
     * @brief                               应用停止
     * @return                              停止结果，0-success，-1-failed
     */
    MY_COMMON::MyStatus_t                   shutdown();

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
    inline std::string                      getId() const { return (nullptr != m_appIdPtr ? m_appIdPtr->id : ""); }

    /**
     * @brief                               获取sip服务实例
     * @return                              sip服务实例
     */
    inline SipMsgProcAppSmtWkPtr            getSipMsgProcApp() { return this->shared_from_this(); }

public:
    /**
     * @brief                               回调函数：模块加载
     * @return                              加载结果，0-success，-1-failed
     * @param endpt                         pjsip endpoint
     */
    static pj_status_t                      OnAppModuleLoadCb(SipAppEndptPtr endpt);

    /**
     * @brief                               回调函数：模块卸载
     * @return                              卸载结果，0-success，-1-failed
     */
    static pj_status_t                      OnAppModuleUnLoadCb();

    /**
     * @brief                               回调函数：模块启动
     * @return                              启动结果，0-success，-1-failed
     */
    static pj_status_t                      OnAppModuleStartCb();

    /**
     * @brief                               回调函数：模块停止
     * @return                              停止结果，0-success，-1-failed
     */
    static pj_status_t                      OnAppModuleStopCb();

    /**
     * @brief                               回调函数：模块接收请求消息
     * @return                              接收请求消息结果，0-success，-1-failed
     * @param rdata                         接收到的请求消息
     */
    static pj_bool_t                        OnAppModuleRecvReqCb(SipAppRxDataPtr rdata);

    /**
     * @brief                               回调函数：模块接收应答消息
     * @return                              接收应答消息结果，0-success，-1-failed
     * @param rdata                         接收到的应答消息
     */
    static pj_bool_t                        OnAppModuleRecvRespCb(SipAppRxDataPtr rdata);

    /**
     * @brief                               回调函数：模块发送请求消息
     * @return                              发送请求消息结果，0-success，-1-failed
     * @param tdata                         待发送的请求消息
     */
    static pj_status_t                      OnAppModuleSendReqCb(SipAppTxDataPtr tdata);

    /**
     * @brief                               回调函数：模块发送应答消息
     * @return                              发送应答消息结果，0-success，-1-failed
     * @param tdata                         待发送的应答消息
     */
    static pj_status_t                      OnAppModuleSendRespCb(SipAppTxDataPtr tdata);

    /**
     * @brief                               回调函数：模块事务状态改变
     * @param tsx                           事务
     * @param event                         事件
     */
    static void                             OnAppModuleTsxStateChangeCb(SipAppTsxPtr tsx, SipAppEvPtr event);

private:
    //                                      应用模块ID
    SipAppIdSmtPtr                          m_appIdPtr;

    //                                      启动状态 
    std::atomic<MY_COMMON::MyStatus_t>      m_status; 

    //                                      应用模块指针
    SipAppModSmtPtr                         m_appModPtr;

    //                                      服务指针
    SipServSmtWkPtr                         m_servSmtWkPtr;
};

}; // namespace MY_APP