#pragma once
#include <string>
#include "common/myTypeDef.h"
#include "utils/myBaseHelper.h"

namespace MY_MANAGER {

/**
 * sip设备目录管理类
 */
class MySipCatalogManage : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    /**
     * sip设备目录管理对象类
     */
    class MySipCatalogManageObject;

    /**
     * sip上/下级服务设备目录管理对象类
     */
    class MySipCatalogInfoManageObject;

public:
    /** 
     * @brief                      添加设备目录信息
     * @return                     添加结果，success-0, failed-非0
     * @param servId               服务id
     * @param servCatalogInfo      服务设备目录信息
     */                         
    static MY_COMMON::MyStatus_t   AddSipCatalogInfo(const std::string& servId, const MY_COMMON::MySipCatalogInfo_dt& servCatalogInfo);

    /** 
     * @brief                      更新设备目录信息
     * @return                     更新结果，success-0, failed-非0
     * @param servId               服务id
     * @param servCatalogInfo      服务设备目录信息
     */                         
    static MY_COMMON::MyStatus_t   UptSipCatalogInfo(const std::string& servId, const MY_COMMON::MySipCatalogInfo_dt& servCatalogInfo);

    /**             
     * @brief                      删除设备目录信息
     * @return                     删除结果，success-0, failed-非0
     * @param servId               服务id
     */                         
    static MY_COMMON::MyStatus_t   DelSipCatalogInfo(const std::string& servId);

public:
    /**             
     * @brief                      更新服务设备目录SN号
     * @return                     更新结果，success-0, failed-非0
     * @param servId               服务id
     * @param sn                   SN号
     */                         
    static MY_COMMON::MyStatus_t   UpdateSipCatalogSN(const std::string& servId, const std::string& sn);

public:
    /** 
     * @brief                      添加查询本级服务设备目录的上级服务信息
     * @return                     添加结果，success-0, failed-非0
     * @param servAddr             上级服务地址
     */                         
    static MY_COMMON::MyStatus_t   AddSipQueryInfo(const MY_COMMON::MySipServRegAddrCfg_dt& servAddr);

    /**             
     * @brief                      删除查询本级服务设备目录的上级服务信息
     * @return                     删除结果，success-0, failed-非0
     * @param upServId             上级服务id
     */                         
    static MY_COMMON::MyStatus_t   DelSipQueryInfo(const std::string& upServId);

public:
    /** 
     * @brief                      添加向本级服务推送设备目录的下级服务信息
     * @return                     添加结果，success-0, failed-非0
     * @param servAddr             下级服务地址
     */                         
    static MY_COMMON::MyStatus_t   AddSipRespInfo(const MY_COMMON::MySipServRegAddrCfg_dt& servAddr);

    /**             
     * @brief                      删除向本级服务推送设备目录的下级服务信息
     * @return                     删除结果，success-0, failed-非0
     * @param lowServId            下级服务id
     */                         
    static MY_COMMON::MyStatus_t   DelSipRespInfo(const std::string& lowServId);
};

/**
 * sip设备目录管理视图类
 */
class MySipCatalogManageView : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    /**
     * @brief                       获取sip设备列表信息
     * @return                      获取结果，success-0, failed-非0
     * @param deviceListInfo        sip设备列表信息
    
     */
    static MY_COMMON::MyStatus_t    GetDeviceListInfo(std::string& deviceListInfo);

    /**
     * @brief                       获取sip设备信息
     * @return                      获取结果，success-0, failed-非0
     * @param deviceId              sip设备id
     * @param deviceInfo            sip设备信息
     */
    static MY_COMMON::MyStatus_t    GetDeviceInfo(const std::string& deviceId, std::string& deviceInfo);

    /**             
     * @brief                       查询向本级服务推送设备目录的下级服务信息是否存在
     * @return                      信息是否存在，success-0, failed-非0
     * @param lowServId             下级服务id
     */                         
    static MY_COMMON::MyStatus_t    HasSipRespInfo(const std::string& lowServId);

    /**             
     * @brief                       获取向本级服务推送设备目录的下级服务
     * @return                      信息是否存在，success-0, failed-非0
     * @param servAddrMap           下级服务地址
     */                         
    static MY_COMMON::MyStatus_t    GetSipRespInfoMap(MY_COMMON::MySipRegServAddrMap& servAddrMap);

    /**             
     * @brief                       查询本级服务设备目录的上级服务信息是否存在
     * @return                      信息是否存在，success-0, failed-非0
     * @param upServId              上级服务id
     */                         
    static MY_COMMON::MyStatus_t    HasSipQueryInfo(const std::string& upServId);

    /**             
     * @brief                       获取查询本级服务设备目录的上级服务信息
     * @return                      信息是否存在，success-0, failed-非0
     * @param servAddrMap           上级服务地址
     */                     
    static MY_COMMON::MyStatus_t    GetSipQueryInfo(MY_COMMON::MySipRegServAddrMap& servAddrMap);

    /**             
     * @brief                       获取服务设备目录SN号
     * @return                      更新结果，success-0, failed-非0
     * @param servId                服务id
     * @param sn                    SN号
     */                         
    static MY_COMMON::MyStatus_t    GetSipCatalogSN(const std::string& servId, std::string& sn);

    /**      
     * @brief                       获取设备目录设备信息map表
     * @return                      获取结果，success-0, failed-非0
     * @param servId                服务id
     * @param deviceCfgMap          设备目录设备信息map表
     */ 
    static MY_COMMON::MyStatus_t    GetSipCatalogDeviceCfgMap(const std::string& servId, MY_COMMON::MySipCatalogDeviceCfgMap& deviceCfgMap);

    /**      
     * @brief                       获取设备目录设备信息
     * @return                      获取结果，success-0, failed-非0
     * @param servId                服务id
     * @param deviceCfg             设备目录设备信息
     */ 
    static MY_COMMON::MyStatus_t    GetSipCatalogDeviceCfg(const std::string& servId, const std::string& deviceId,
                                        MY_COMMON::MySipCatalogDeviceCfg_dt& deviceCfg);

    /**     
     * @brief                       获取设备目录虚拟子平台信息
     * @return                      获取结果，success-0, failed-非0
     * @param servId                服务id
     * @param subVirtualPlatCfgMap  设备目录虚拟子平台信息
     */ 
    static MY_COMMON::MyStatus_t    GetSipCatalogSubVirtualPlatCfgMap(const std::string& servId,
                                        MY_COMMON::MySipCatalogSubVirtualPlatCfgMap& subVirtualPlatCfgMap);

    /**     
     * @brief                       获取设备目录子平台信息
     * @return                      获取结果，success-0, failed-非0
     * @param servId                服务id
     * @param subPlatCfgMap         设备目录子平台信息
     */     
    static MY_COMMON::MyStatus_t    GetSipCatalogSubPlatCfgMap(const std::string& servId, MY_COMMON::MySipCatalogSubPlatCfgMap& subPlatCfgMap);

    /**     
     * @brief                       获取设备目录平台信息map表
     * @return                      获取结果，success-0, failed-非0
     * @param servId                服务id
     * @param platCfgMap            设备目录平台信息map表
     */
    static MY_COMMON::MyStatus_t    GetSipCatalogPlatCfgMap(const std::string& servId, MY_COMMON::MySipCatalogPlatCfgMap& platCfgMap);

    /**
     * @brief                       设备目录信息是否存在
     * @return                      信息是否存在，success-0, failed-非0
     * @param servId                服务id
     */
    static MY_COMMON::MyStatus_t    HasSipCatalogInfo(const std::string& servId);
};

}; // namespace MY_MANAGER