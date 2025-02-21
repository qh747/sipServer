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

    /**
     * sip上级服务查询设备目录管理对象类
     */
    class MySipUpSevQueryCatalogManageObject;

public:
    // key = local server id, value = sip serv catalog info
    typedef std::map<std::string, MY_COMMON::MySipCatalogInfo_dt> MySipServCatalogInfoMap;

    // key = up server id, value = sip up serv addr info who query local server catalog
    typedef std::map<std::string, MY_COMMON::MySipServAddrCfg_dt> MySipUpServInfoSubMap;

    // key = local server id, value = sip up serv addr info map
    typedef std::map<std::string, MySipUpServInfoSubMap>          MySipUpServInfoMap;

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
     * @return                                          获取结果，success-0, failed-非0
     * @param servId                                    本地服务id
     * @param platCfgMap                                设备目录平台信息
     */     
    static MY_COMMON::MyStatus_t                        GetSipServCatalogPlatCfgMap(const std::string& servId, MY_COMMON::MySipCatalogPlatCfgMap& platCfgMap);

    /**     
     * @brief                                           获取设备目录子平台信息
     * @return                                          获取结果，success-0, failed-非0
     * @param servId                                    本地服务id
     * @param subPlatCfgMap                             设备目录子平台信息
     */     
    static MY_COMMON::MyStatus_t                        GetSipServCatalogSubPlatCfgMap(const std::string& servId, MY_COMMON::MySipCatalogSubPlatCfgMap& subPlatCfgMap);

    /**     
     * @brief                                           获取设备目录虚拟子平台信息
     * @return                                          获取结果，success-0, failed-非0
     * @param servId                                    本地服务id
     * @param subVirtualPlatCfgMap                      设备目录虚拟子平台信息
     */ 
    static MY_COMMON::MyStatus_t                        GetSipServCatalogSubVirtualPlatCfgMap(const std::string& servId, MY_COMMON::MySipCatalogSubVirtualPlatCfgMap& subVirtualPlatCfgMap);
 
    /**      
     * @brief                                           获取设备目录设备信息
     * @return                                          获取结果，success-0, failed-非0
     * @param servId                                    本地服务id
     * @param deviceCfgMap                              设备目录设备信息
     */ 
    static MY_COMMON::MyStatus_t                        GetSipServCatalogDeviceCfgMap(const std::string& servId, MY_COMMON::MySipCatalogDeviceCfgMap& deviceCfgMap);

    /**             
     * @brief                                           更新服务设备目录SN号
     * @return                                          更新结果，success-0, failed-非0
     * @param servId                                    本地服务id
     * @param sn                                        SN号
     */                         
    static MY_COMMON::MyStatus_t                        UpdateSipServCatalogSN(const std::string& servId, const std::string& sn);

public:
    /** 
     * @brief                                           添加查询本级服务设备目录的上级服务信息
     * @return                                          添加结果，success-0, failed-非0
     * @param servId                                    本地服务id
     * @param upServInfo                                上级服务信息
     */                         
    static MY_COMMON::MyStatus_t                        AddSipUpQueryServInfo(const std::string& servId, const MY_COMMON::MySipServAddrCfg_dt& upServInfo);

    /**             
     * @brief                                           删除查询本级服务设备目录的上级服务信息
     * @return                                          删除结果，success-0, failed-非0
     * @param servId                                    本地服务id
     * @param upServId                                  上级服务id
     */                         
    static MY_COMMON::MyStatus_t                        DelSipUpQueryServInfo(const std::string& servId, const std::string& upServId);
 
    /**             
     * @brief                                           查询本级服务设备目录的上级服务信息是否存在
     * @return                                          信息是否存在，success-0, failed-非0
     * @param servId                                    本地服务id
     * @param upServId                                  上级服务id
     */                         
    static MY_COMMON::MyStatus_t                        HasSipUpQueryServInfo(const std::string& servId, const std::string& upServId);

    /**             
     * @brief                                           获取查询本级服务设备目录的上级服务信息map
     * @return                                          信息是否存在，success-0, failed-非0
     * @param servId                                    本地服务id
     */                         
    static MY_COMMON::MyStatus_t                        GetSipUpQueryServInfoMap(const std::string& servId, MySipUpServInfoSubMap& upServInfoMap);
};

}; // namespace MY_MANAGER