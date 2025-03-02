#pragma once
#include <string>
#include <memory>
#include "common/myTypeDef.h"

namespace MY_APP {

/**
 * sip应用消息处理类
 */
class MySipMsgProcApp : public std::enable_shared_from_this<MySipMsgProcApp>
{
public:
    typedef std::shared_ptr<MySipMsgProcApp>    SmtPtr;
    typedef std::weak_ptr<MySipMsgProcApp>      SmtWkPtr;

public:
    MySipMsgProcApp();
    ~MySipMsgProcApp();

public:
    /**
     * @brief                                   应用初始化
     * @return                                  初始化结果，0-success，-1-failed
     * @param servId                            sip服务ID
     * @param name                              应用名称
     * @param priority                          应用优先级
     */ 
    MY_COMMON::MyStatus_t                       init(const std::string& servId, const std::string& name, pjsip_module_priority priority);

    /** 
     * @brief                                   应用启动
     * @return                                  启动结果，0-success，-1-failed
     */ 
    MY_COMMON::MyStatus_t                       run();

    /** 
     * @brief                                   应用停止
     * @return                                  停止结果，0-success，-1-failed
     */ 
    MY_COMMON::MyStatus_t                       shutdown();

public:     
    /**     
     * @brief                                   应用是否启动
     * @return                                  获取结果，0-success，-1-failed
     * @param status                            启动结果，0-success，-1-failed
     */             
    MY_COMMON::MyStatus_t                       getState(MY_COMMON::MyStatus_t& status) const;

    /** 
     * @brief                                   获取应用id
     * @return                                  获取结果，0-success，-1-failed
     * @param id                                应用id
     */ 
    MY_COMMON::MyStatus_t                       getId(std::string& id) const;

    /** 
     * @brief                                   获取应用
     * @return                                  获取结果，0-success，-1-failed
     * @param wkPtr                             应用实例
     */
     MY_COMMON::MyStatus_t                      getSipMsgProcApp(SmtWkPtr& wkPtr);

public:
    /**
     * @brief                                   回调函数：处理sip注册请求消息
     * @return                                  处理结果，0-success，-1-failed
     * @param rdata                             接收到的sip注册请求消息
     */
    MY_COMMON::MyStatus_t                       onProcSipRegisterReqMsg(MY_COMMON::MySipRxDataPtr rdataPtr);

    /**
     * @brief                                   回调函数：处理sip保活请求消息
     * @return                                  处理结果，0-success，-1-failed
     * @param rdata                             接收到的sip保活请求消息
     */
    MY_COMMON::MyStatus_t                       onProcSipKeepAliveReqMsg(MY_COMMON::MySipRxDataPtr rdataPtr);

    /**
     * @brief                                   回调函数：处理sip设备目录查询请求消息
     * @return                                  处理结果，0-success，-1-failed
     * @param rdata                             接收到的sip设备目录查询请求消息
     */
    MY_COMMON::MyStatus_t                       onProcSipCatalogQueryReqMsg(MY_COMMON::MySipRxDataPtr rdataPtr);

    /**
     * @brief                                   回调函数：处理sip设备目录响应请求消息
     * @return                                  处理结果，0-success，-1-failed
     * @param rdata                             接收到的sip设备目录响应请求消息
     */
    MY_COMMON::MyStatus_t                       onProcSipCatalogResponseReqMsg(MY_COMMON::MySipRxDataPtr rdataPtr);

public: 
    /** 
     * @brief                                   回调函数：模块加载
     * @return                                  加载结果，0-success，-1-failed
     * @param endpt                             pjsip endpoint
     */
    static pj_status_t                          OnAppModuleLoadCb(MY_COMMON::MySipEndptPtr endptPtr);

    /** 
     * @brief                                   回调函数：模块卸载
     * @return                                  卸载结果，0-success，-1-failed
     */ 
    static pj_status_t                          OnAppModuleUnLoadCb();

    /** 
     * @brief                                   回调函数：模块启动
     * @return                                  启动结果，0-success，-1-failed
     */ 
    static pj_status_t                          OnAppModuleStartCb();

    /** 
     * @brief                                   回调函数：模块停止
     * @return                                  停止结果，0-success，-1-failed
     */ 
    static pj_status_t                          OnAppModuleStopCb();

    /** 
     * @brief                                   回调函数：模块接收请求消息
     * @return                                  接收请求消息结果，0-success，-1-failed
     * @param rdata                             接收到的请求消息
     */ 
    static pj_bool_t                            OnAppModuleRecvReqCb(MY_COMMON::MySipRxDataPtr rdataPtr);

    /**
     * @brief                                   回调函数：模块接收应答消息
     * @return                                  接收应答消息结果，0-success，-1-failed
     * @param rdata                             接收到的应答消息
     */ 
    static pj_bool_t                            OnAppModuleRecvRespCb(MY_COMMON::MySipRxDataPtr rdataPtr);

    /** 
     * @brief                                   回调函数：模块发送请求消息
     * @return                                  发送请求消息结果，0-success，-1-failed
     * @param tdata                             待发送的请求消息
     */ 
    static pj_status_t                          OnAppModuleSendReqCb(MY_COMMON::MySipTxDataPtr tdataPtr);

    /** 
     * @brief                                   回调函数：模块发送应答消息
     * @return                                  发送应答消息结果，0-success，-1-failed
     * @param tdata                             待发送的应答消息
     */ 
    static pj_status_t                          OnAppModuleSendRespCb(MY_COMMON::MySipTxDataPtr tdataPtr);

    /** 
     * @brief                                   回调函数：模块事务状态改变
     * @param tsx                               事务
     * @param event                             事件
     */
    static void                                 OnAppModuleTsxStateChangeCb(MY_COMMON::MySipTsxPtr tsxPtr, 
                                                                            MY_COMMON::MySipEvPtr evPtr);

private:    
    //                                          sip服务ID
    std::string                                 m_servId;

    //                                          应用模块ID
    MY_COMMON::MySipAppIdCfg_dt                 m_appIdCfg;

    //                                          启动状态 
    MY_COMMON::MyAtomicStatus                   m_status; 

    //                                          应用模块指针
    MY_COMMON::MySipModSmtPtr                   m_appModPtr;
};  

}; // namespace MY_APP