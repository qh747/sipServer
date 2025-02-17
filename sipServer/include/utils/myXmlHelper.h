#pragma once
#include <string>
#include <tinyxml2.h>
#include "common/myDataDef.h"
#include "common/myConfigDef.h"
#include "common/mySipMsgDef.h"
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

/**
 * xml帮助类
 */
class MyXmlHelper : public MyNonConstructableAndNonCopyable
{
public:
    /**
     * @brief                           获取sip消息内容类型
     * @return                          sip消息内容类型
     * @param xmlStr                    xml字符串
     */             
    static MY_COMMON::MySipMsgBody_t    GetSipMsgBodyType(const std::string& xmlStr);

public:             
    /**             
     * @note                            消息格式：
     *                                  <?xml version="1.0" encoding="UTF-8"?>                           
     *                                  <Notify>
     *                                    <CmdType>Keepalive</CmdType>
     *                                    <SN>1234</SN>
     *                                    <DeviceID>12345678909876543210</DeviceID>
     *                                    <Status>OK</Status>
     *                                  </Notify>
     *                  
     * @brief                           sip keepalive消息生成
     * @return                          sip keepalive消息体
     * @param idx                       消息索引编号
     * @param id                        本级服务id
     */                 
    static std::string                  GenerateSipKeepAliveMsgBody(const std::string& idx, const std::string& localServId);

    /**                 
     * @brief                           sip keepalive消息解析
     * @return                          解析结果
     * @param xmlStr                    字符串格式消息内容
     * @param keepAliveMsgBody          sip keepalive消息体
     */                 
    static MY_COMMON::MyStatus_t        ParseSipKeepAliveMsgBody(const std::string&                   xmlStr, 
                                                                 MY_COMMON::MySipKeepAliveMsgBody_dt& keepAliveMsgBody);

public:                 
    /**                 
     * @note                            消息格式：
     *                                  <?xml version="1.0" encoding="UTF-8"?>                           
     *                                  <Query>
     *                                    <CmdType>Catalog</CmdType>
     *                                    <SN>1234</SN>
     *                                    <DeviceID>12345678909876543210</DeviceID>
     *                                  </Query>
     *                  
     * @brief                           sip catalog请求消息生成
     * @return                          sip catalog请求消息体
     * @param id                        下级服务id
     */                 
    static std::string                  GenerateSipCatalogReqMsgBody(const std::string& id);

    /**             
     * @brief                           sip catalog请求消息解析
     * @return                          解析结果
     * @param xmlStr                    字符串格式消息内容
     * @param catalogReqMsgBody         sip catalog请求消息内容
     */                 
    static MY_COMMON::MyStatus_t        ParseSipCatalogReqMsgBody(const std::string&                    xmlStr, 
                                                                  MY_COMMON::MySipCatalogReqMsgBody_dt& catalogReqMsgBody);

public:                 
    /**                 
     * @note                            消息格式：
     *                                  <?xml version="1.0" encoding="gb2312"?>
     *                                  <Response>
     *                                    <CmdType>Catalog</CmdType>
     *                                    <SN>49013560</SN>
     *                                    <DeviceID>34020000001110000001</DeviceID>
     *                                    <SumNum>8</SumNum>
     *                                    <DeviceList Num="1">
     *                                      <Item>
     *                                        <DeviceID>34020000001320000001</DeviceID>
     *                                        <Name>sip platform</Name>
     *                                        <Manufacturer>Manufacturer</Manufacturer>
     *                                        <Model>Camera</Model>
     *                                        <Owner>Owner</Owner>
     *                                        <CivilCode>340200</CivilCode>
     *                                        <Address>192.168.30.200</Address>
     *                                        <Parental>0</Parental>
     *                                        <SafetyWay>0</SafetyWay>
     *                                        <RegisterWay>1</RegisterWay>
     *                                        <Secrecy>0</Secrecy>
     *                                        <Status>ON</Status>
     *                                      </Item>
     *                                    </DeviceList>
     *                                  </Response>
     *                  
     * @brief                           sip catalog平台配置推送消息生成
     * @return                          sip catalog平台配置推送消息体
     * @param platCfg                   sip设备目录平台配置
     * @param sn                        消息索引编号
     * @param sumNum                    sip设备目录平台配置数量
     */                 
    static std::string                  GenerateSipCatalogPlatCfgMsgBody(const MY_COMMON::MySipCatalogPlatCfg_dt& platCfg, 
                                                                         const std::string&                       sn,
                                                                         std::size_t                              sumNum);

    /**             
     * @brief                           sip catalog平台配置消息解析
     * @return                          解析结果
     * @param xmlStr                    字符串格式消息内容
     * @param catalogPlatCfgMsgBody     sip catalog平台配置消息内容
     */                 
    static MY_COMMON::MyStatus_t        ParseSipCatalogPlatCfgMsgBody(const std::string&                        xmlStr, 
                                                                      MY_COMMON::MySipCatalogPlatCfgMsgBody_dt& catalogPlatCfgMsgBody);

public:                 
    /**                 
     * @note                            消息格式：
     *                                  <?xml version="1.0" encoding="gb2312"?>
     *                                  <Response>
     *                                    <CmdType>Catalog</CmdType>
     *                                    <SN>49013560</SN>
     *                                    <DeviceID>34020000001110000001</DeviceID>
     *                                    <SumNum>8</SumNum>
     *                                    <DeviceList Num="1">
     *                                      <Item>
     *                                        <DeviceID>34020000001320000001</DeviceID>
     *                                        <Name>sip platform</Name>
     *                                        <Manufacturer>Manufacturer</Manufacturer>
     *                                        <Model>Camera</Model>
     *                                        <Owner>Owner</Owner>
     *                                        <CivilCode>340200</CivilCode>
     *                                        <Address>192.168.30.200</Address>
     *                                        <Parental>0</Parental>
     *                                        <SafetyWay>0</SafetyWay>
     *                                        <RegisterWay>1</RegisterWay>
     *                                        <Secrecy>0</Secrecy>
     *                                        <Status>ON</Status>
     *                                      </Item>
     *                                    </DeviceList>
     *                                  </Response>
     *                  
     * @brief                           sip catalog子平台配置推送消息生成
     * @return                          sip catalog子平台配置推送消息体
     * @param subPlatCfg                sip设备目录子平台配置
     * @param sn                        消息索引编号
     * @param sumNum                    sip设备目录子平台配置数量
     */                 
    static std::string                  GenerateSipCatalogSubPlatCfgMsgBody(const MY_COMMON::MySipCatalogSubPlatCfg_dt& subPlatCfg, 
                                                                            const std::string&                          sn,
                                                                            std::size_t                                 sumNum);

    /**             
     * @brief                           sip catalog子平台配置消息解析
     * @return                          解析结果
     * @param xmlStr                    字符串格式消息内容
     * @param catalogSubPlatCfgMsgBody  sip catalog子平台配置消息内容
     */                 
    static MY_COMMON::MyStatus_t        ParseSipCatalogSubPlatCfgMsgBody(const std::string&                           xmlStr, 
                                                                         MY_COMMON::MySipCatalogSubPlatCfgMsgBody_dt& catalogSubPlatCfgMsgBody);

public:                 
    /**                 
     * @note                            消息格式：
     *                                  <?xml version="1.0" encoding="gb2312"?>
     *                                  <Response>
     *                                    <CmdType>Catalog</CmdType>
     *                                    <SN>49013560</SN>
     *                                    <DeviceID>34020000001110000001</DeviceID>
     *                                    <SumNum>8</SumNum>
     *                                    <DeviceList Num="1">
     *                                      <Item>
     *                                        <DeviceID>34020000001320000001</DeviceID>
     *                                        <Name>sip platform</Name>
     *                                        <Manufacturer>Manufacturer</Manufacturer>
     *                                        <Model>Camera</Model>
     *                                        <Owner>Owner</Owner>
     *                                        <CivilCode>340200</CivilCode>
     *                                        <Address>192.168.30.200</Address>
     *                                        <Parental>0</Parental>
     *                                        <SafetyWay>0</SafetyWay>
     *                                        <RegisterWay>1</RegisterWay>
     *                                        <Secrecy>0</Secrecy>
     *                                        <Status>ON</Status>
     *                                      </Item>
     *                                    </DeviceList>
     *                                  </Response>
     *                  
     * @brief                           sip catalog虚拟子平台配置推送消息生成
     * @return                          sip catalog虚拟子平台配置推送消息体
     * @param subVirtualPlatCfg         sip设备目录虚拟子平台配置
     * @param sn                        消息索引编号
     * @param sumNum                    sip设备目录虚拟子平台配置数量
     */                 
    static std::string                  GenerateSipCatalogSubVirtualPlatCfgMsgBody(const MY_COMMON::MySipCatalogVirtualSubPlatCfg_dt& subVirtualPlatCfg, 
                                                                                   const std::string&                                 sn,
                                                                                   std::size_t                                        sumNum);

    /**             
     * @brief                                  sip catalog虚拟子平台配置消息解析
     * @return                                 解析结果
     * @param xmlStr                           字符串格式消息内容
     * @param catalogSubVirtualPlatCfgMsgBody  sip catalog虚拟子平台配置消息内容
     */                 
    static MY_COMMON::MyStatus_t        ParseSipCatalogSubVirtualPlatCfgMsgBody(const std::string&                                  xmlStr, 
                                                                                MY_COMMON::MySipCatalogSubVirtualPlatCfgMsgBody_dt& catalogSubVirtualPlatCfgMsgBody);

public:                 
    /**                 
     * @note                            消息格式：
     *                                  <?xml version="1.0" encoding="gb2312"?>
     *                                  <Response>
     *                                    <CmdType>Catalog</CmdType>
     *                                    <SN>49013560</SN>
     *                                    <DeviceID>34020000001110000001</DeviceID>
     *                                    <SumNum>8</SumNum>
     *                                    <DeviceList Num="1">
     *                                      <Item>
     *                                        <DeviceID>34020000001320000001</DeviceID>
     *                                        <Name>sip platform</Name>
     *                                        <Manufacturer>Manufacturer</Manufacturer>
     *                                        <Model>Camera</Model>
     *                                        <Owner>Owner</Owner>
     *                                        <CivilCode>340200</CivilCode>
     *                                        <Address>192.168.30.200</Address>
     *                                        <Parental>0</Parental>
     *                                        <SafetyWay>0</SafetyWay>
     *                                        <RegisterWay>1</RegisterWay>
     *                                        <Secrecy>0</Secrecy>
     *                                        <Status>ON</Status>
     *                                      </Item>
     *                                    </DeviceList>
     *                                  </Response>
     *                  
     * @brief                           sip catalog设备配置推送消息生成
     * @return                          sip catalog设备配置推送消息体
     * @param subPlatCfg                sip设备目录设备配置
     * @param sn                        消息索引编号
     * @param sumNum                    sip设备目录设备配置数量
     */                 
    static std::string                  GenerateSipCatalogDeviceCfgMsgBody(const MY_COMMON::MySipCatalogDeviceCfg_dt& deviceCfg, 
                                                                           const std::string&                         sn,
                                                                           std::size_t                                sumNum);

    /**             
     * @brief                           sip catalog设备配置消息解析
     * @return                          解析结果
     * @param xmlStr                    字符串格式消息内容
     * @param catalogDeviceCfgMsgBody   sip catalog设备配置消息内容
     */                 
    static MY_COMMON::MyStatus_t        ParseSipCatalogDeviceCfgMsgBody(const std::string&                          xmlStr, 
                                                                        MY_COMMON::MySipCatalogDeviceCfgMsgBody_dt& catalogDeviceCfgMsgBody);
};

}; // namespace MY_UTILS