#pragma once
#include <string>
#include "common/myDataDef.h"
#include "utils/myBaseHelper.h"

namespace MY_MANAGER {

/**
 * sip会话管理类
 */
class MySipSessionManage : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    /**
     * sip会话管理对象类
     */
    class MySipSessionManageObject;

public:
    /** 
     * @brief                      添加sip会话信息
     * @return                     添加结果，success-0, failed-非0
     * @param servId               设备id
     * @param servCatalogInfo      sip会话信息
     */                         
    static MY_COMMON::MyStatus_t   AddSipSessionInfo(const std::string& deviceId, const MY_COMMON::MySipSessionInfo_dt& sessionInfo);

    /** 
     * @brief                      删除sip会话信息
     * @return                     删除结果，success-0, failed-非0
     * @param servId               设备id
     */                         
    static MY_COMMON::MyStatus_t   DelSipSessionInfo(const std::string& deviceId);

    /** 
     * @brief                      更新sip会话信息
     * @return                     更新结果，success-0, failed-非0
     * @param servId               设备id
     * @param servCatalogInfo      sip会话信息
     */                         
    static MY_COMMON::MyStatus_t   UptSipSessionInfo(const std::string& deviceId, const MY_COMMON::MySipSessionInfo_dt& sessionInfo);
};

/**
 * sip设备目录管理视图类
 */
class MySipSessionManageView : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    /** 
     * @brief                      判断sip会话信息是否存在
     * @return                     判断结果，success-0, failed-非0
     * @param servId               设备id
     */                         
    static MY_COMMON::MyStatus_t   HasSipSessionInfo(const std::string& deviceId);

    /** 
     * @brief                      获取sip会话信息
     * @return                     获取结果，success-0, failed-非0
     * @param servId               设备id
     * @param servCatalogInfo      sip会话信息
     */                         
    static MY_COMMON::MyStatus_t   GetSipSessionInfo(const std::string& deviceId, MY_COMMON::MySipSessionInfo_dt& sessionInfo);
};

}; // namespace MY_MANAGER