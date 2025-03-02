#pragma once
#include <string>
#include <memory>
#include "common/myTypeDef.h"

namespace MY_APP {

/**
 * sip应用会话邀请处理类
 */
class MySipInviteApp : public std::enable_shared_from_this<MySipInviteApp>
{
public:
    typedef std::shared_ptr<MySipInviteApp>     SmtPtr;
    typedef std::weak_ptr<MySipInviteApp>       SmtWkPtr;

public:
    /**
     * @brief                                   邀请会话状态改变回调
     * @param invSessPtr                        邀请会话对象
     * @param evPtr                             事件对象
     */
    static void                                 OnInviteStateChanged(MY_COMMON::MySipInvSessionPtr invSessPtr, MY_COMMON::MySipEvPtr evPtr);

    /**
     * @brief                                   邀请会话创建回调
     * @param invSessPtr                        邀请会话对象
     * @param evPtr                             事件对象
     */
    static void                                 OnNewInviteSession(MY_COMMON::MySipInvSessionPtr invSessPtr, MY_COMMON::MySipEvPtr evPtr);

    /**
     * @brief                                   邀请会话媒体更新回调
     * @param invSessPtr                        邀请会话对象
     * @param status                            状态
     */
    static void                                 OnInviteMediaUpdate(MY_COMMON::MySipInvSessionPtr invSessPtr, pj_status_t status);

    /**
     * @brief                                   邀请会话响应Ack回调
     * @param invSessPtr                        邀请会话对象
     * @param rxDataPtr                         接收数据
     */
    static void                                 OnInviteSendAck(MY_COMMON::MySipInvSessionPtr invSessPtr, MY_COMMON::MySipRxDataPtr rxDataPtr);

public:
    MySipInviteApp();
    ~MySipInviteApp();

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
     MY_COMMON::MyStatus_t                      getSipInviteApp(SmtWkPtr& wkPtr);

private:    
    //                                          sip服务ID
    std::string                                 m_servId;

    //                                          应用模块ID
    MY_COMMON::MySipAppIdCfg_dt                 m_appIdCfg;

    //                                          启动状态 
    MY_COMMON::MyAtomicStatus                   m_status; 

    //                                          invite回调函数地址
    MY_COMMON::MySipInvCbSmtPtr                 m_invCbPtr;
};

}; // namespace MY_APP