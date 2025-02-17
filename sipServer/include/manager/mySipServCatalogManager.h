#pragma once
#include <map>
#include <string>
#include "common/myTypeDef.h"
#include "utils/myBaseHelper.h"

namespace MY_MANAGER
{

/**
 * sip服务设备目录管理类
 */
class MySipServCatalogManage : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    /**
     * sip服务设备目录管理对象类
     */
    class MySipServCatalogManageObject;

public:
    // key = local server id, value = sip serv catalog info ptr
    typedef std::map<std::string, MY_COMMON::MySipCatalogInfo_dt>      MySipServCatalogInfoMap;

public:
    /** 
     * @brief                                           添加服务设备目录信息
     * @return                                          添加结果，success-0, failed-非0
     * @param servId                                    本地服务id
     * @param upRegServInfo                             本地服务设备目录信息
     */                         
    static MY_COMMON::MyStatus_t                        AddSipServCatalogInfo(const std::string& servId, const MY_COMMON::MySipCatalogInfo_dt& servCatalogInfo);

    /**             
     * @brief                                           删除服务设备目录信息
     * @return                                          删除结果，success-0, failed-非0
     * @param servId                                    本地服务id
     */                         
    static MY_COMMON::MyStatus_t                        DelSipServCatalogInfo(const std::string& servId);

    /**             
     * @brief                                           服务设备目录信息是否存在
     * @return                                          信息是否存在，success-0, failed-非0
     * @param servId                                    本地服务id
     */                         
    static MY_COMMON::MyStatus_t                        HasSipServCatalogInfo(const std::string& servId);

    public:
    /**     
     * @brief                                           获取设备目录平台信息
     * @return                                          设备目录平台信息
     * @param servId                                    本地服务id
     */     
    static MY_COMMON::MySipCatalogPlatCfgMap            GetSipServCatalogPlatCfgMap(const std::string& servId);

    /**     
     * @brief                                           获取设备目录子平台信息
     * @return                                          设备目录子平台信息
     * @param servId                                    本地服务id
     */     
    static MY_COMMON::MySipCatalogSubPlatCfgMap         GetSipServCatalogSubPlatCfgMap(const std::string& servId);

    /**     
     * @brief                                           获取设备目录虚拟子平台信息
     * @return                                          设备目录虚拟子平台信息
     * @param servId                                    本地服务id
     */ 
    static MY_COMMON::MySipCatalogSubVirtualPlatCfgMap  GetSipServCatalogSubVirtualPlatCfgMap(const std::string& servId);
 
    /**      
     * @brief                                           获取设备目录设备信息
     * @return                                          设备目录设备信息
     * @param servId                                    本地服务id
     */ 
    static MY_COMMON::MySipCatalogDeviceCfgMap          GetSipServCatalogDeviceCfgMap(const std::string& servId);

public:
    /**             
     * @brief                                           更新服务设备目录SN号
     * @return                                          更新结果，success-0, failed-非0
     * @param servId                                    本地服务id
     * @param sn                                        SN号
     */                         
    static MY_COMMON::MyStatus_t                        UpdateSipServCatalogSN(const std::string& servId, const std::string& sn);
};

}; // namespace MY_MANAGER