#pragma once
#include <string>
#include <memory>
#include <pjsip.h>
#include "common/myDataDef.h"
#include "utils/myBaseHelper.h"
using MY_COMMON::MyStatus_t;

namespace MY_APP {
    
/**
 * sip应用包装类
 */
class MySipAppWrapper : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    typedef pjsip_event*                    SipAppEvPtr;
    typedef pjsip_module*                   SipAppModPtr;
    typedef pjsip_transaction*              SipAppTsxPtr;
    typedef pjsip_endpoint*                 SipAppEndptPtr;
    typedef pjsip_rx_data*                  SipAppRxDataPtr;
    typedef pjsip_tx_data*                  SipAppTxDataPtr;

public:
    /**
     * @brief                               获取sip应用实例
     * @return                              sip应用实例
     */     
    static SipAppModPtr                     AppModule();

    /**     
     * @brief                               初始化
     * @return                              初始化结果，0-success，-1-failed
     * @param endpt                         pjsip endpoint
     * @param appName                       sip应用名称
     * @param priority                      sip应用优先级
     */     
    static MyStatus_t                       Init(SipAppEndptPtr endpt, const std::string& name, pjsip_module_priority priority);

    /**
     * @brief                               关闭
     * @return                              关闭结果，0-success，-1-failed
     * @param endpt                         pjsip endpoint
     */
    static MyStatus_t                       Shutdown(SipAppEndptPtr endpt);

    /**
     * @brief                               获取sip应用信息
     * @return                              sip应用信息
     */
    static std::string                      GetAppModuleInfo();

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
    // sip协议模块      
    static SipAppModPtr                     AppModulePtr; 
};
    
}; // namespace MY_APP