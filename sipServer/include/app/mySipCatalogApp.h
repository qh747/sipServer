#pragma once
#include <memory>
#include <string>
#include <atomic>
#include <cstdbool>
#include "common/myTypeDef.h"

namespace MY_APP {
    
/**
 * sip应用设备目录类
 */
class MySipCatalogApp : public std::enable_shared_from_this<MySipCatalogApp>
{
public:
    typedef std::shared_ptr<MySipCatalogApp>    SmtPtr;
    typedef std::weak_ptr<MySipCatalogApp>      SmtWkPtr;

public:
    /**
     * @brief                               sip catalog应用推送设备目录线程回调函数
     * @return                              回调结果，0-success，-1-failed
     * @param arg                           回调参数
     */
    static int                              SipCatalogAppPushCatalogThdFunc(MY_COMMON::MySipCbParamPtr arg);

    /**
     * @brief                               sip catalog应用推送设备目录回调函数
     * @param arg                           回调参数
     * @param ev                            回调事件
     */
    static void                             OnSipCatalogAppPushCatalogCb(MY_COMMON::MySipCbParamPtr arg, MY_COMMON::MySipEvPtr ev);

    /**
     * @brief                               sip catalog应用更新设备目录线程回调函数
     * @return                              回调结果，0-success，-1-failed
     * @param arg                           回调参数
     */
    static int                              SipCatalogAppUpdateCatalogThdFunc(MY_COMMON::MySipCbParamPtr arg);

    /**
     * @brief                               sip catalog应用更新设备目录回调函数
     * @param arg                           回调参数
     * @param ev                            回调事件
     */
    static void                             OnSipCatalogAppUpdateCatalogCb(MY_COMMON::MySipCbParamPtr arg, MY_COMMON::MySipEvPtr ev);

public:
    MySipCatalogApp();
    ~MySipCatalogApp();

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
     * @brief                               处理下级sip服务的sip设备目录查询请求消息
     * @return                              处理结果，0-success，-1-failed
     * @param rxDataPtr                     sip设备目录查询请求消息
     */
    MY_COMMON::MyStatus_t                   onSipCatalogAppRecvSipCatalogQueryReqMsg(MY_COMMON::MySipRxDataPtr rxDataPtr);  

    /**             
     * @brief                               处理下级sip服务的sip设备目录响应请求消息
     * @return                              处理结果，0-success，-1-failed
     * @param rxDataPtr                     sip设备目录响应请求消息
     */
    MY_COMMON::MyStatus_t                   onSipCatalogAppRecvSipCatalogResponseReqMsg(MY_COMMON::MySipRxDataPtr rxDataPtr);  

public:             
    /**                     
     * @brief                               应用是否启动
     * @return                              获取结果，0-success，-1-failed
     * @param status                        启动结果，0-success，-1-failed
     */                             
    MY_COMMON::MyStatus_t                   getState(MY_COMMON::MyStatus_t& status) const;

    /**             
     * @brief                               获取应用id
     * @return                              获取结果，0-success，-1-failed
     * @param id                            应用id
     */                 
    MY_COMMON::MyStatus_t                   getId(std::string& id) const;

    /**             
     * @brief                               获取应用
     * @return                              获取结果，0-success，-1-failed
     * @param wkPtr                         应用实例
     */             
     MY_COMMON::MyStatus_t                  getSipCatalogApp(SmtWkPtr& wkPtr);

    /**
     * @brief                               向下级sip服务请求设备目录
     * @return                              请求结果，0-success，-1-failed
     * @param regLowServCfg                 下级sip注册服务配置
     * @param localServCfg                  本地sip服务地址配置
     */             
    MY_COMMON::MyStatus_t                   onSipCatalogAppReqLowServCatalog(const MY_COMMON::MySipRegLowServCfg_dt& regLowServCfg, 
                                                                             const MY_COMMON::MySipServAddrCfg_dt&   localServCfg);

    /**
     * @brief                               发送本级sip服务设备目录
     * @return                              发送结果，0-success，-1-failed
     * @param sn                            sip设备目录请求消息中的sn号
     * @param regUpServCfg                  上级sip注册服务配置
     * @param localServCfg                  本地sip服务地址配置
     */             
    MY_COMMON::MyStatus_t                   sendLocalServCatalog(const std::string&                      sn,
                                                                 const MY_COMMON::MySipRegUpServCfg_dt&  regUpServCfg, 
                                                                 const MY_COMMON::MySipServAddrCfg_dt&   localServCfg);

private:
    //                                      sip服务ID
    std::string                             m_servId;

    //                                      应用模块ID
    MY_COMMON::MySipAppIdCfg_dt             m_appIdCfg;

    //                                      启动状态 
    std::atomic<MY_COMMON::MyStatus_t>      m_status; 
};

}; // namespace MY_APP