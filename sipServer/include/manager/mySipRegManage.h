#pragma once
#include <map>
#include <string>
#include <cstdbool>
#include "common/myTypeDef.h"
#include "utils/myBaseHelper.h"

namespace MY_MANAGER
{

/**
 * sip服务注册管理类
 */
class MySipRegManage : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    /**
     * sip服务注册管理对象类
     */
    class MySipServRegManageObject;

public:
    // key = up ref server id, value = sip up reg serv info ptr
    typedef std::map<std::string, MY_COMMON::MySipUpRegServInfo_dt>      MySipUpRegServInfoMap;
    // key = low ref server id, value = sip low reg serv info ptr
    typedef std::map<std::string, MY_COMMON::MySipLowRegServInfo_dt>     MySipLowRegServInfoMap;

public: 
    /** 
     * @brief                               添加上级服务注册信息
     * @return                              添加结果，success-0, failed-非0
     * @param upRegServInfo                 上级服务注册信息
     */             
    static MY_COMMON::MyStatus_t            AddSipRegUpServInfo(const MY_COMMON::MySipUpRegServInfo_dt& upRegServInfo);

    /**     
     * @brief                               删除上级服务注册信息
     * @return                              删除结果，success-0, failed-非0
     * @param upRegServId                   上级服务注册id
     */             
    static MY_COMMON::MyStatus_t            DelSipRegUpServInfo(const std::string& upRegServId);

    /**             
     * @brief                               上级服务注册信息是否存在
     * @return                              是否存在，success-0, failed-非0
     * @param upRegServId                   上级服务注册id
     */             
    static MY_COMMON::MyStatus_t            HasSipRegUpServInfo(const std::string& upRegServId);

    /**     
     * @brief                               获取上级服务注册配置map
     * @return                              获取结果，success-0, failed-非0
     * @param upRegCfgMap                   上级服务注册配置map
     */ 
    static MY_COMMON::MyStatus_t            GetSipRegUpServCfgMap(MY_COMMON::MySipRegUpServCfgMap& upRegCfgMap);

    /**     
     * @brief                               获取上级服务注册配置
     * @return                              获取结果，success-0, failed-非0
     * @param upRegServId                   上级服务注册id
     * @param upRegServCfg                  上级服务注册配置
     */ 
    static MY_COMMON::MyStatus_t            GetSipRegUpServCfg(const std::string& upRegServId, MY_COMMON::MySipRegUpServCfg_dt& upRegServCfg);

    /**     
     * @brief                               获取上级服务注册配置
     * @return                              获取结果，success-0, failed-非0
     * @param upRegServId                   上级服务注册id
     * @param proto                         上级服务协议类型
     */ 
    static MY_COMMON::MyStatus_t            GetSipRegUpServProto(const std::string& upRegServId, MY_COMMON::MyTpProto_t& proto);

    /**
     * @brief                               获取上级服务注册最后注册时间
     * @return                              获取结果，success-0, failed-非0
     * @param upRegServId                   上级服务注册id
     * @param time                          上级服务注册最后注册时间
     */
    static MY_COMMON::MyStatus_t            GetSipRegUpServLastRegTime(const std::string& upRegServId, std::string& time);

    /**
     * @brief                               获取上级服务注册时长
     * @return                              获取结果，success-0, failed-非0
     * @param upRegServId                   上级服务注册id
     * @param expired                       上级服务注册时长
     */ 
    static MY_COMMON::MyStatus_t            GetSipRegUpServExpired(const std::string& upRegServId, uint32_t& expired);

    /**
     * @brief                               获取上级服务保活索引
     * @return                              获取结果，success-0, failed-非0
     * @param upRegServId                   上级服务注册id
     * @param idx                           上级服务保活索引
     */
    static MY_COMMON::MyStatus_t            GetSipRegUpServKeepAliveIdx(const std::string& upRegServId, uint32_t& idx);

    /**
     * @brief                               更新上级服务注册最后注册时间
     * @return                              更新结果，success-0, failed-非0
     * @param upRegServId                   上级服务注册id
     * @param time                          上级服务注册最后注册时间
     */ 
    static MY_COMMON::MyStatus_t            UpdateSipRegUpServLastRegTime(const std::string& upRegServId, const std::string& time);

    /**
     * @brief                               更新上级服务注册时长
     * @return                              更新结果，success-0, failed-非0
     * @param upRegServId                   上级服务注册id
     * @param expired                       上级服务注册时长
     */ 
    static MY_COMMON::MyStatus_t            UpdateSipRegUpServExpired(const std::string& upRegServId, uint32_t expired);

    /**
     * @brief                               更新上级服务保活索引
     * @return                              更新结果，success-0, failed-非0
     * @param upRegServId                   上级服务注册id
     * @param idx                           上级服务保活索引
     */ 
    static MY_COMMON::MyStatus_t            UpdateSipRegUpServKeepAliveIdx(const std::string& upRegServId, uint32_t idx);

public:     
    /**     
     * @brief                               添加下级服务注册信息
     * @return                              添加结果，success-0, failed-非0
     * @param lowRegServInfo                下级服务注册信息
     */         
    static MY_COMMON::MyStatus_t            AddSipRegLowServInfo(const MY_COMMON::MySipLowRegServInfo_dt& lowRegServInfo);

    /** 
     * @brief                               删除下级服务注册信息
     * @return                              删除结果，success-0, failed-非0
     * @param lowRegServInfo                下级服务注册信息
     */             
    static MY_COMMON::MyStatus_t            DelSipRegLowServInfo(const std::string& lowRegServId);

    /** 
     * @brief                               下级服务注册信息是否存在
     * @return                              是否存在，success-0, failed-非0
     * @param lowRegServInfo                下级服务注册信息
     */             
    static MY_COMMON::MyStatus_t            HasSipRegLowServInfo(const std::string& lowRegServId);

    /** 
     * @brief                               获取下级服务注册最后注册时间
     * @return                              获取结果，success-0, failed-非0
     * @param lowRegServId                  下级服务注册id
     * @param time                          下级服务注册最后注册时间
     */ 
    static MY_COMMON::MyStatus_t            GetSipRegLowServLastRegTime(const std::string& lowRegServId, std::string& time);

    /** 
     * @brief                               获取下级服务注册时长
     * @return                              获取结果，success-0, failed-非0
     * @param lowRegServId                  下级服务注册id
     * @param expired                       下级服务注册时长
     */ 
    static MY_COMMON::MyStatus_t            GetSipRegLowServExpired(const std::string& lowRegServId, uint32_t& expired);

    /**         
     * @brief                               获取下级服务注册配置map
     * @return                              获取结果，success-0, failed-非0
     * @param lowRegCfgMap                  下级服务注册配置map
     */ 
    static MY_COMMON::MyStatus_t            GetSipRegLowServCfgMap(MY_COMMON::MySipRegLowServCfgMap& lowRegCfgMap);

    /**     
     * @brief                               获取下级服务注册认证配置
     * @return                              获取结果，success-0, failed-非0
     * @param name                          认证名称
     * @param realm                         认证域名
     * @param authCfg                       认证配置
     */     
    static MY_COMMON::MyStatus_t            GetSipRegLowAuthCfg(const std::string& name, const std::string& realm, MY_COMMON::MySipServAuthCfg_dt& authCfg);

    /**
     * @brief                               更新下级服务注册最后注册时间
     * @return                              更新结果，success-0, failed-非0
     * @param lowRegServId                  下级服务注册id
     * @param time                          下级服务注册最后注册时间
     */ 
    static MY_COMMON::MyStatus_t            UpdateSipRegLowServLastRegTime(const std::string& lowRegServId, const std::string& time);

    /** 
     * @brief                               更新下级服务注册时长
     * @return                              更新结果，success-0, failed-非0
     * @param lowRegServId                  下级服务注册id
     * @param expired                       下级服务注册时长
     */ 
    static MY_COMMON::MyStatus_t            UpdateSipRegLowServExpired(const std::string& lowRegServId, uint32_t expired);

    /** 
     * @brief                               更新下级服务地址
     * @return                              更新结果，success-0, failed-非0
     * @param lowRegServId                  下级服务注册id
     * @param expired                       下级服务地址
     */ 
    static MY_COMMON::MyStatus_t            UpdateSipRegLowServIpAddr(const std::string& lowRegServId, const MY_COMMON::MySipServRegAddrCfg_dt& lowRegServAddr);
};

}; // namespace MY_MANAGER