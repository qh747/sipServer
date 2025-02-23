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
     * sip上/下级服务设备目录管理对象类
     */
    class MySipServCatalogInfoManageObject;

public:
    // key = server id, value = sip serv catalog info
    typedef std::map<std::string, MY_COMMON::MySipCatalogInfo_dt> MySipServCatalogInfoMap;

    // key = local server id, value = sip up/low serv addr map
    typedef std::map<std::string, MY_COMMON::MySipServAddrMap>    MySipServCatalogInfoAddrMap;

public:
    /** 
     * @brief                                           添加服务设备目录信息
     * @return                                          添加结果，success-0, failed-非0
     * @param servId                                    服务id
     * @param upRegServInfo                             服务设备目录信息
     */                         
    static MY_COMMON::MyStatus_t                        AddSipServCatalogInfo(const std::string& servId, const MY_COMMON::MySipCatalogInfo_dt& servCatalogInfo);

    /** 
     * @brief                                           更新服务设备目录信息
     * @return                                          更新结果，success-0, failed-非0
     * @param servId                                    服务id
     * @param upRegServInfo                             服务设备目录信息
     */                         
    static MY_COMMON::MyStatus_t                        UptSipServCatalogInfo(const std::string& servId, const MY_COMMON::MySipCatalogInfo_dt& servCatalogInfo);

    /**             
     * @brief                                           删除服务设备目录信息
     * @return                                          删除结果，success-0, failed-非0
     * @param servId                                    服务id
     */                         
    static MY_COMMON::MyStatus_t                        DelSipServCatalogInfo(const std::string& servId);

    /**             
     * @brief                                           服务设备目录信息是否存在
     * @return                                          信息是否存在，success-0, failed-非0
     * @param servId                                    服务id
     */                         
    static MY_COMMON::MyStatus_t                        HasSipServCatalogInfo(const std::string& servId);

public:
    /**     
     * @brief                                           获取设备目录平台信息
     * @return                                          获取结果，success-0, failed-非0
     * @param servId                                    服务id
     * @param platCfgMap                                设备目录平台信息
     */     
    static MY_COMMON::MyStatus_t                        GetSipServCatalogPlatCfgMap(const std::string& servId, MY_COMMON::MySipCatalogPlatCfgMap& platCfgMap);

    /**     
     * @brief                                           获取设备目录子平台信息
     * @return                                          获取结果，success-0, failed-非0
     * @param servId                                    服务id
     * @param subPlatCfgMap                             设备目录子平台信息
     */     
    static MY_COMMON::MyStatus_t                        GetSipServCatalogSubPlatCfgMap(const std::string& servId, MY_COMMON::MySipCatalogSubPlatCfgMap& subPlatCfgMap);

    /**     
     * @brief                                           获取设备目录虚拟子平台信息
     * @return                                          获取结果，success-0, failed-非0
     * @param servId                                    服务id
     * @param subVirtualPlatCfgMap                      设备目录虚拟子平台信息
     */ 
    static MY_COMMON::MyStatus_t                        GetSipServCatalogSubVirtualPlatCfgMap(const std::string& servId, MY_COMMON::MySipCatalogSubVirtualPlatCfgMap& subVirtualPlatCfgMap);
 
    /**      
     * @brief                                           获取设备目录设备信息
     * @return                                          获取结果，success-0, failed-非0
     * @param servId                                    服务id
     * @param deviceCfgMap                              设备目录设备信息
     */ 
    static MY_COMMON::MyStatus_t                        GetSipServCatalogDeviceCfgMap(const std::string& servId, MY_COMMON::MySipCatalogDeviceCfgMap& deviceCfgMap);

    /**             
     * @brief                                           获取服务设备目录SN号
     * @return                                          更新结果，success-0, failed-非0
     * @param servId                                    服务id
     * @param sn                                        SN号
     */                         
    static MY_COMMON::MyStatus_t                        GetSipServCatalogSN(const std::string& servId, std::string& sn);

    /**             
     * @brief                                           更新服务设备目录SN号
     * @return                                          更新结果，success-0, failed-非0
     * @param servId                                    服务id
     * @param sn                                        SN号
     */                         
    static MY_COMMON::MyStatus_t                        UpdateSipServCatalogSN(const std::string& servId, const std::string& sn);

public:
    /** 
     * @brief                                           添加查询本级服务设备目录的上级服务信息
     * @return                                          添加结果，success-0, failed-非0
     * @param servId                                    本级服务id
     * @param servAddr                                  上级服务地址
     */                         
    static MY_COMMON::MyStatus_t                        AddSipUpQueryServInfo(const std::string& servId, const MY_COMMON::MySipServAddrCfg_dt& servAddr);

    /**             
     * @brief                                           删除查询本级服务设备目录的上级服务信息
     * @return                                          删除结果，success-0, failed-非0
     * @param servId                                    本级服务id
     * @param upServId                                  上级服务id
     */                         
    static MY_COMMON::MyStatus_t                        DelSipUpQueryServInfo(const std::string& servId, const std::string& upServId);
 
    /**             
     * @brief                                           查询本级服务设备目录的上级服务信息是否存在
     * @return                                          信息是否存在，success-0, failed-非0
     * @param servId                                    本级服务id
     * @param upServId                                  上级服务id
     */                         
    static MY_COMMON::MyStatus_t                        HasSipUpQueryServInfo(const std::string& servId, const std::string& upServId);

    /**             
     * @brief                                           获取查询本级服务设备目录的上级服务信息
     * @return                                          信息是否存在，success-0, failed-非0
     * @param servId                                    本级服务id
     * @param servAddrMap                               上级服务地址
     */                         
    static MY_COMMON::MyStatus_t                        GetSipUpQueryServInfo(const std::string& servId, MY_COMMON::MySipServAddrMap& servAddrMap);

public:
    /** 
     * @brief                                           添加向本级服务推送设备目录的下级服务信息
     * @return                                          添加结果，success-0, failed-非0
     * @param servId                                    本级服务id
     * @param servAddr                                  下级服务地址
     */                         
    static MY_COMMON::MyStatus_t                        AddSipLowRespServInfo(const std::string& servId, const MY_COMMON::MySipServAddrCfg_dt& servAddr);

    /**             
     * @brief                                           删除向本级服务推送设备目录的下级服务信息
     * @return                                          删除结果，success-0, failed-非0
     * @param servId                                    本级服务id
     * @param lowServId                                 下级服务id
     */                         
    static MY_COMMON::MyStatus_t                        DelSipLowRespServInfo(const std::string& servId, const std::string& lowServId);
 
    /**             
     * @brief                                           查询向本级服务推送设备目录的下级服务信息是否存在
     * @return                                          信息是否存在，success-0, failed-非0
     * @param servId                                    本级服务id
     * @param upServId                                  下级服务id
     */                         
    static MY_COMMON::MyStatus_t                        HasSipLowRespServInfo(const std::string& servId, const std::string& lowServId);

    /**             
     * @brief                                           获取向本级服务推送设备目录的下级服务
     * @return                                          信息是否存在，success-0, failed-非0
     * @param servId                                    本级服务id
     * @param servAddrMap                               下级服务地址
     */                         
    static MY_COMMON::MyStatus_t                        GetSipLowRespServInfoMap(const std::string& servId, MY_COMMON::MySipServAddrMap& servAddrMap);
};

}; // namespace MY_MANAGER