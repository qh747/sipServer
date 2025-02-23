#include "utils/myStrHelper.h"
#include "utils/myRandomHelper.h"
#include "utils/myXmlHelper.h"
using namespace tinyxml2;
using namespace MY_COMMON;

namespace MY_UTILS {

MyStatus_t MyXmlHelper::GetSipMsgBodyType(const std::string& xmlStr, MySipMsgBody_t& msgBodyType)
{
    tinyxml2::XMLDocument doc;

    // 解析XML数据
    if (tinyxml2::XML_SUCCESS != doc.Parse(xmlStr.c_str())) {
        return MyStatus_t::FAILED;
    }

    // 获取根元素
    tinyxml2::XMLElement* root = doc.RootElement();
    if (nullptr == root) {
        return MyStatus_t::FAILED;
    }

    // 获取子元素
    tinyxml2::XMLElement* cmdTypeElement = root->FirstChildElement("CmdType");
    if (nullptr == cmdTypeElement) {
        return MyStatus_t::FAILED;
    }

    std::string cmdType;
    MyStrHelper::TrimEmptyStr(cmdTypeElement->GetText(), cmdType);
    MyStrHelper::ConvertToLowStr(cmdType, cmdType);
    
    if ("keepalive" == cmdType) {
        msgBodyType = MySipMsgBody_t::SIP_MSG_KEEPALIVE;
        return MyStatus_t::SUCCESS;
    }
    else if ("catalog" == cmdType) {
        std::string rootName;
        MyStrHelper::ConvertToLowStr(root->Name(), rootName);

        if ("query" == rootName) {
            msgBodyType = MySipMsgBody_t::SIP_MSG_CATALOG_QUERY;
            return MyStatus_t::SUCCESS;
        }
        else if ("response" == rootName) {
            msgBodyType = MySipMsgBody_t::SIP_MSG_CATALOG_RESPONSE;
            return MyStatus_t::SUCCESS;
        }
    }
    return MyStatus_t::FAILED;
}

MyStatus_t MyXmlHelper::GetSipCatalogRespMsgDeviceId(const std::string& xmlStr, std::string& deviceId)
{
    tinyxml2::XMLDocument doc;

    // 解析XML数据
    if (tinyxml2::XML_SUCCESS != doc.Parse(xmlStr.c_str())) {
        return MyStatus_t::FAILED;
    }

    // 获取根元素
    tinyxml2::XMLElement* root = doc.RootElement();
    if (nullptr == root) {
        return MyStatus_t::FAILED;
    }

    // 获取子元素
    tinyxml2::XMLElement* deviceIdElement = root->FirstChildElement("DeviceID");
    if (nullptr == deviceIdElement) {
        return MyStatus_t::FAILED;
    }

    deviceId = deviceIdElement->GetText();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyXmlHelper::GenerateSipKeepAliveMsgBody(const std::string& idx, const std::string& localServId, std::string& msgBody)
{
    // 创建一个 XML 文档对象
    tinyxml2::XMLDocument doc;

    // 添加 XML 声明（<?xml version="1.0" encoding="UTF-8"?>）
    doc.InsertFirstChild(doc.NewDeclaration());

    // 创建根节点 <Notify>
    tinyxml2::XMLElement* root = doc.NewElement("Notify");
    doc.InsertEndChild(root);

    // 创建子节点 <CmdType> 并设置内容
    tinyxml2::XMLElement* cmdType = doc.NewElement("CmdType");
    cmdType->SetText("Keepalive");
    root->InsertEndChild(cmdType);

    // 创建子节点 <SN> 并设置内容
    tinyxml2::XMLElement* sn = doc.NewElement("SN");
    sn->SetText(idx.c_str());
    root->InsertEndChild(sn);

    // 创建子节点 <DeviceID> 并设置内容
    tinyxml2::XMLElement* deviceID = doc.NewElement("DeviceID");
    deviceID->SetText(localServId.c_str());
    root->InsertEndChild(deviceID);

    // 创建子节点 <Status> 并设置内容
    tinyxml2::XMLElement* status = doc.NewElement("Status");
    status->SetText("OK");
    root->InsertEndChild(status);

    // 将 XML 文档转换为字符串
    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);
    msgBody = printer.CStr();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyXmlHelper::ParseSipKeepAliveMsgBody(const std::string& xmlStr, MySipKeepAliveMsgBody_dt& keepAliveMsgBody)
{
    tinyxml2::XMLDocument doc;

    // 解析XML数据
    if (tinyxml2::XML_SUCCESS != doc.Parse(xmlStr.c_str())) {
        return MyStatus_t::FAILED;
    }

    // 获取根元素
    tinyxml2::XMLElement* root = doc.RootElement();
    if (nullptr == root) {
        return MyStatus_t::FAILED;
    }

    // 获取子元素
    tinyxml2::XMLElement* cmdTypeElement = root->FirstChildElement("CmdType");
    if (nullptr != cmdTypeElement) {
        keepAliveMsgBody.cmdType = cmdTypeElement->GetText();
    }

    tinyxml2::XMLElement* deviceIDElement = root->FirstChildElement("DeviceID");
    if (nullptr != deviceIDElement) {
        keepAliveMsgBody.deviceId = deviceIDElement->GetText();
    }

    tinyxml2::XMLElement* snElement = root->FirstChildElement("SN");
    if (nullptr != snElement) {
        keepAliveMsgBody.sn = snElement->GetText();
    }

    tinyxml2::XMLElement* statusElement = root->FirstChildElement("Status");
    if (nullptr != statusElement) {
        std::string statusStr;
        MyStrHelper::ConvertToUpStr(statusElement->GetText(), statusStr);

        keepAliveMsgBody.status = ("OK" == statusStr ? true : false);
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyXmlHelper::GenerateSipCatalogQueryReqMsgBody(const std::string& id, std::string& msgBody)
{
    // 创建一个 XML 文档对象
    tinyxml2::XMLDocument doc;

    // 添加 XML 声明（<?xml version="1.0" encoding="UTF-8"?>）
    doc.InsertFirstChild(doc.NewDeclaration());

    // 创建根节点 <Notify>
    tinyxml2::XMLElement* root = doc.NewElement("Query");
    doc.InsertEndChild(root);

    // 创建子节点 <CmdType> 并设置内容
    tinyxml2::XMLElement* cmdType = doc.NewElement("CmdType");
    cmdType->SetText("Catalog");
    root->InsertEndChild(cmdType);

    // 创建子节点 <SN> 并设置内容
    tinyxml2::XMLElement* sn = doc.NewElement("SN");

    std::string snStr;
    MyRandomHelper::Get4BytesLenRandomStr(snStr);
    sn->SetText(snStr.c_str());
    root->InsertEndChild(sn);

    // 创建子节点 <DeviceID> 并设置内容
    tinyxml2::XMLElement* deviceID = doc.NewElement("DeviceID");
    deviceID->SetText(id.c_str());
    root->InsertEndChild(deviceID);

    // 将 XML 文档转换为字符串
    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);
    msgBody = printer.CStr();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyXmlHelper::ParseSipCatalogQueryReqMsgBody(const std::string& xmlStr, MySipCatalogReqMsgBody_dt& catalogReqMsgBody)
{
    tinyxml2::XMLDocument doc;

    // 解析XML数据
    if (tinyxml2::XML_SUCCESS != doc.Parse(xmlStr.c_str())) {
        return MyStatus_t::FAILED;
    }

    // 获取根元素
    tinyxml2::XMLElement* root = doc.RootElement();
    if (nullptr == root) {
        return MyStatus_t::FAILED;
    }

    // 获取子元素
    tinyxml2::XMLElement* cmdTypeElement = root->FirstChildElement("CmdType");
    if (nullptr != cmdTypeElement) {
        catalogReqMsgBody.cmdType = cmdTypeElement->GetText();
    }

    tinyxml2::XMLElement* deviceIDElement = root->FirstChildElement("DeviceID");
    if (nullptr != deviceIDElement) {
        catalogReqMsgBody.deviceId = deviceIDElement->GetText();
    }

    tinyxml2::XMLElement* snElement = root->FirstChildElement("SN");
    if (nullptr != snElement) {
        catalogReqMsgBody.sn = snElement->GetText();
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyXmlHelper::GenerateSipCatalogPlatCfgMsgBody(const MY_COMMON::MySipCatalogPlatCfg_dt& platCfg, const std::string& sn, 
                                                         std::size_t sumNum, std::string& msgBody)
{
    // 创建一个 XML 文档对象
    tinyxml2::XMLDocument doc;

    // 添加 XML 声明（<?xml version="1.0" encoding="UTF-8"?>）
    doc.InsertFirstChild(doc.NewDeclaration());

    // 创建根节点Response
    tinyxml2::XMLElement* rootElem = doc.NewElement("Response");
    doc.InsertEndChild(rootElem);

    // 创建子节点CmdType
    tinyxml2::XMLElement* cmdTypeElem = doc.NewElement("CmdType");
    cmdTypeElem->SetText("Catalog");
    rootElem->InsertEndChild(cmdTypeElem);

    // 创建子节点SN
    tinyxml2::XMLElement* snElem = doc.NewElement("SN");
    snElem->SetText(sn.c_str());
    rootElem->InsertEndChild(snElem);

    // 创建子节点DeviceID
    tinyxml2::XMLElement* deviceIdElem = doc.NewElement("DeviceID");
    deviceIdElem->SetText(platCfg.platformID.c_str());
    rootElem->InsertEndChild(deviceIdElem);

    // 创建子节点SumNum
    tinyxml2::XMLElement* sumNumElem = doc.NewElement("SumNum");
    sumNumElem->SetText(std::to_string(sumNum).c_str());
    rootElem->InsertEndChild(sumNumElem);

    // 创建子节点DeviceList
    tinyxml2::XMLElement* deviceListElem = doc.NewElement("DeviceList");
    deviceListElem->SetAttribute("Num", "1");
    rootElem->InsertEndChild(deviceListElem);

    // 创建子节点DeviceList的子节点item
    tinyxml2::XMLElement* itemElem = doc.NewElement("Item");
    deviceListElem->InsertEndChild(itemElem);

    // 创建子节点item的子节点DeviceID
    tinyxml2::XMLElement* itemDeviceIdElem = doc.NewElement("DeviceID");
    itemDeviceIdElem->SetText(platCfg.deviceID.c_str());
    itemElem->InsertEndChild(itemDeviceIdElem);

    // 创建子节点item的子节点Name
    tinyxml2::XMLElement* itemNameElem = doc.NewElement("Name");
    itemNameElem->SetText(platCfg.name.c_str());
    itemElem->InsertEndChild(itemNameElem);

    // 创建子节点item的子节点Manufacturer
    tinyxml2::XMLElement* itemManufacturerElem = doc.NewElement("Manufacturer");
    itemManufacturerElem->SetText((platCfg.manufacturer.empty() ? "unknown" : platCfg.manufacturer.c_str()));
    itemElem->InsertEndChild(itemManufacturerElem);

    // 创建子节点item的子节点Model
    tinyxml2::XMLElement* itemModelElem = doc.NewElement("Model");
    itemModelElem->SetText((platCfg.model.empty() ? "unknown" : platCfg.model.c_str()));
    itemElem->InsertEndChild(itemModelElem);

    // 创建子节点item的子节点Owner
    tinyxml2::XMLElement* itemOwnerElem = doc.NewElement("Owner");
    itemOwnerElem->SetText(platCfg.owner.c_str());
    itemElem->InsertEndChild(itemOwnerElem);

    // 创建子节点item的子节点CivilCode
    tinyxml2::XMLElement* itemCivilCodeElem = doc.NewElement("CivilCode");
    itemCivilCodeElem->SetText(platCfg.deviceID.substr(0, 6).c_str());
    itemElem->InsertEndChild(itemCivilCodeElem);

    // 创建子节点item的子节点Address
    tinyxml2::XMLElement* itemAddressElem = doc.NewElement("Address");
    itemAddressElem->SetText(platCfg.deviceIp.c_str());
    itemElem->InsertEndChild(itemAddressElem);

    // 创建子节点item的子节点Parental
    tinyxml2::XMLElement* itemParentalElem = doc.NewElement("Parental");
    itemParentalElem->SetText(std::to_string(platCfg.parental).c_str());
    itemElem->InsertEndChild(itemParentalElem);

    // 创建子节点item的子节点ParentID
    if (0 != platCfg.parental) {     
        tinyxml2::XMLElement* itemParentIdElem = doc.NewElement("ParentID");
        itemParentIdElem->SetText(platCfg.parentID.c_str());
        itemElem->InsertEndChild(itemParentIdElem);
    }

    // 创建子节点item的子节点PlatformID
    tinyxml2::XMLElement* itemPlatformIdElem = doc.NewElement("PlatformID");
    itemPlatformIdElem->SetText(platCfg.platformID.c_str());
    itemElem->InsertEndChild(itemPlatformIdElem);

    // 创建子节点item的子节点SafetyWay
    tinyxml2::XMLElement* itemSafetyWayElem = doc.NewElement("SafetyWay");
    itemSafetyWayElem->SetText(std::to_string(platCfg.safetyWay).c_str());
    itemElem->InsertEndChild(itemSafetyWayElem);

    // 创建子节点item的子节点RegisterWay
    tinyxml2::XMLElement* itemRegisterWayElem = doc.NewElement("RegisterWay");
    itemRegisterWayElem->SetText(std::to_string(platCfg.registerWay).c_str());
    itemElem->InsertEndChild(itemRegisterWayElem);

    // 创建子节点item的子节点Secrecy
    tinyxml2::XMLElement* itemSecrecyElem = doc.NewElement("Secrecy");
    itemSecrecyElem->SetText(std::to_string(platCfg.secrecy).c_str());
    itemElem->InsertEndChild(itemSecrecyElem);

    // 创建子节点item的子节点Status
    tinyxml2::XMLElement* itemStatusElem = doc.NewElement("Status");
    itemStatusElem->SetText(platCfg.status.c_str());
    itemElem->InsertEndChild(itemStatusElem);

    // 将 XML 文档转换为字符串
    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);
    msgBody = printer.CStr();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyXmlHelper::GenerateSipCatalogSubPlatCfgMsgBody(const MySipCatalogSubPlatCfg_dt& subPlatCfg, const std::string& sn, 
                                                            std::size_t sumNum, std::string& msgBody)
{
    // 创建一个 XML 文档对象
    tinyxml2::XMLDocument doc;

    // 添加 XML 声明（<?xml version="1.0" encoding="UTF-8"?>）
    doc.InsertFirstChild(doc.NewDeclaration());

    // 创建根节点Response
    tinyxml2::XMLElement* rootElem = doc.NewElement("Response");
    doc.InsertEndChild(rootElem);

    // 创建子节点CmdType
    tinyxml2::XMLElement* cmdTypeElem = doc.NewElement("CmdType");
    cmdTypeElem->SetText("Catalog");
    rootElem->InsertEndChild(cmdTypeElem);

    // 创建子节点SN
    tinyxml2::XMLElement* snElem = doc.NewElement("SN");
    snElem->SetText(sn.c_str());
    rootElem->InsertEndChild(snElem);

    // 创建子节点DeviceID
    tinyxml2::XMLElement* deviceIdElem = doc.NewElement("DeviceID");
    deviceIdElem->SetText(subPlatCfg.platformID.c_str());
    rootElem->InsertEndChild(deviceIdElem);

    // 创建子节点SumNum
    tinyxml2::XMLElement* sumNumElem = doc.NewElement("SumNum");
    sumNumElem->SetText(std::to_string(sumNum).c_str());
    rootElem->InsertEndChild(sumNumElem);

    // 创建子节点DeviceList
    tinyxml2::XMLElement* deviceListElem = doc.NewElement("DeviceList");
    deviceListElem->SetAttribute("Num", "1");
    rootElem->InsertEndChild(deviceListElem);

    // 创建子节点DeviceList的子节点item
    tinyxml2::XMLElement* itemElem = doc.NewElement("Item");
    deviceListElem->InsertEndChild(itemElem);

    // 创建子节点item的子节点DeviceID
    tinyxml2::XMLElement* itemDeviceIdElem = doc.NewElement("DeviceID");
    itemDeviceIdElem->SetText(subPlatCfg.deviceID.c_str());
    itemElem->InsertEndChild(itemDeviceIdElem);

    // 创建子节点item的子节点Name
    tinyxml2::XMLElement* itemNameElem = doc.NewElement("Name");
    itemNameElem->SetText(subPlatCfg.name.c_str());
    itemElem->InsertEndChild(itemNameElem);

    // 创建子节点item的子节点Manufacturer
    tinyxml2::XMLElement* itemManufacturerElem = doc.NewElement("Manufacturer");
    itemManufacturerElem->SetText((subPlatCfg.manufacturer.empty() ? "unknown" : subPlatCfg.manufacturer.c_str()));
    itemElem->InsertEndChild(itemManufacturerElem);

    // 创建子节点item的子节点Model
    tinyxml2::XMLElement* itemModelElem = doc.NewElement("Model");
    itemModelElem->SetText((subPlatCfg.model.empty() ? "unknown" : subPlatCfg.model.c_str()));
    itemElem->InsertEndChild(itemModelElem);

    // 创建子节点item的子节点Owner
    tinyxml2::XMLElement* itemOwnerElem = doc.NewElement("Owner");
    itemOwnerElem->SetText(subPlatCfg.owner.c_str());
    itemElem->InsertEndChild(itemOwnerElem);

    // 创建子节点item的子节点CivilCode
    tinyxml2::XMLElement* itemCivilCodeElem = doc.NewElement("CivilCode");
    itemCivilCodeElem->SetText(subPlatCfg.deviceID.substr(0, 6).c_str());
    itemElem->InsertEndChild(itemCivilCodeElem);

    // 创建子节点item的子节点Address
    tinyxml2::XMLElement* itemAddressElem = doc.NewElement("Address");
    itemAddressElem->SetText(subPlatCfg.deviceIp.c_str());
    itemElem->InsertEndChild(itemAddressElem);

    // 创建子节点item的子节点Parental
    tinyxml2::XMLElement* itemParentalElem = doc.NewElement("Parental");
    itemParentalElem->SetText(std::to_string(subPlatCfg.parental).c_str());
    itemElem->InsertEndChild(itemParentalElem);

    // 创建子节点item的子节点ParentID
    if (0 != subPlatCfg.parental) {     
        tinyxml2::XMLElement* itemParentIdElem = doc.NewElement("ParentID");
        itemParentIdElem->SetText(subPlatCfg.parentID.c_str());
        itemElem->InsertEndChild(itemParentIdElem);
    }

    // 创建子节点item的子节点PlatformID
    tinyxml2::XMLElement* itemPlatformIdElem = doc.NewElement("PlatformID");
    itemPlatformIdElem->SetText(subPlatCfg.platformID.c_str());
    itemElem->InsertEndChild(itemPlatformIdElem);

    // 创建子节点item的子节点SafetyWay
    tinyxml2::XMLElement* itemSafetyWayElem = doc.NewElement("SafetyWay");
    itemSafetyWayElem->SetText(std::to_string(subPlatCfg.safetyWay).c_str());
    itemElem->InsertEndChild(itemSafetyWayElem);

    // 创建子节点item的子节点RegisterWay
    tinyxml2::XMLElement* itemRegisterWayElem = doc.NewElement("RegisterWay");
    itemRegisterWayElem->SetText(std::to_string(subPlatCfg.registerWay).c_str());
    itemElem->InsertEndChild(itemRegisterWayElem);

    // 创建子节点item的子节点Secrecy
    tinyxml2::XMLElement* itemSecrecyElem = doc.NewElement("Secrecy");
    itemSecrecyElem->SetText(std::to_string(subPlatCfg.secrecy).c_str());
    itemElem->InsertEndChild(itemSecrecyElem);

    // 创建子节点item的子节点Status
    tinyxml2::XMLElement* itemStatusElem = doc.NewElement("Status");
    itemStatusElem->SetText(subPlatCfg.status.c_str());
    itemElem->InsertEndChild(itemStatusElem);

    // 将 XML 文档转换为字符串
    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);
    msgBody = printer.CStr();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyXmlHelper::GenerateSipCatalogSubVirtualPlatCfgMsgBody(const MySipCatalogVirtualSubPlatCfg_dt& subVirtualPlatCfg, const std::string& sn, 
                                                                   std::size_t sumNum, std::string& msgBody)
{
    // 创建一个 XML 文档对象
    tinyxml2::XMLDocument doc;

    // 添加 XML 声明（<?xml version="1.0" encoding="UTF-8"?>）
    doc.InsertFirstChild(doc.NewDeclaration());

    // 创建根节点Response
    tinyxml2::XMLElement* rootElem = doc.NewElement("Response");
    doc.InsertEndChild(rootElem);

    // 创建子节点CmdType
    tinyxml2::XMLElement* cmdTypeElem = doc.NewElement("CmdType");
    cmdTypeElem->SetText("Catalog");
    rootElem->InsertEndChild(cmdTypeElem);

    // 创建子节点SN
    tinyxml2::XMLElement* snElem = doc.NewElement("SN");
    snElem->SetText(sn.c_str());
    rootElem->InsertEndChild(snElem);

    // 创建子节点DeviceID
    tinyxml2::XMLElement* deviceIdElem = doc.NewElement("DeviceID");
    deviceIdElem->SetText(subVirtualPlatCfg.platformID.c_str());
    rootElem->InsertEndChild(deviceIdElem);

    // 创建子节点SumNum
    tinyxml2::XMLElement* sumNumElem = doc.NewElement("SumNum");
    sumNumElem->SetText(std::to_string(sumNum).c_str());
    rootElem->InsertEndChild(sumNumElem);

    // 创建子节点DeviceList
    tinyxml2::XMLElement* deviceListElem = doc.NewElement("DeviceList");
    deviceListElem->SetAttribute("Num", "1");
    rootElem->InsertEndChild(deviceListElem);

    // 创建子节点DeviceList的子节点item
    tinyxml2::XMLElement* itemElem = doc.NewElement("Item");
    deviceListElem->InsertEndChild(itemElem);

    // 创建子节点item的子节点DeviceID
    tinyxml2::XMLElement* itemDeviceIdElem = doc.NewElement("DeviceID");
    itemDeviceIdElem->SetText(subVirtualPlatCfg.deviceID.c_str());
    itemElem->InsertEndChild(itemDeviceIdElem);

    // 创建子节点item的子节点Name
    tinyxml2::XMLElement* itemNameElem = doc.NewElement("Name");
    itemNameElem->SetText(subVirtualPlatCfg.name.c_str());
    itemElem->InsertEndChild(itemNameElem);

    // 创建子节点item的子节点Manufacturer
    tinyxml2::XMLElement* itemManufacturerElem = doc.NewElement("Manufacturer");
    itemManufacturerElem->SetText((subVirtualPlatCfg.manufacturer.empty() ? "unknown" : subVirtualPlatCfg.manufacturer.c_str()));
    itemElem->InsertEndChild(itemManufacturerElem);

    // 创建子节点item的子节点Model
    tinyxml2::XMLElement* itemModelElem = doc.NewElement("Model");
    itemModelElem->SetText((subVirtualPlatCfg.model.empty() ? "unknown" : subVirtualPlatCfg.model.c_str()));
    itemElem->InsertEndChild(itemModelElem);

    // 创建子节点item的子节点Owner
    tinyxml2::XMLElement* itemOwnerElem = doc.NewElement("Owner");
    itemOwnerElem->SetText(subVirtualPlatCfg.owner.c_str());
    itemElem->InsertEndChild(itemOwnerElem);

    // 创建子节点item的子节点CivilCode
    tinyxml2::XMLElement* itemCivilCodeElem = doc.NewElement("CivilCode");
    itemCivilCodeElem->SetText(subVirtualPlatCfg.deviceID.substr(0, 6).c_str());
    itemElem->InsertEndChild(itemCivilCodeElem);

    // 创建子节点item的子节点Address
    tinyxml2::XMLElement* itemAddressElem = doc.NewElement("Address");
    itemAddressElem->SetText(subVirtualPlatCfg.deviceIp.c_str());
    itemElem->InsertEndChild(itemAddressElem);

    // 创建子节点item的子节点Parental
    tinyxml2::XMLElement* itemParentalElem = doc.NewElement("Parental");
    itemParentalElem->SetText(std::to_string(subVirtualPlatCfg.parental).c_str());
    itemElem->InsertEndChild(itemParentalElem);

    // 创建子节点item的子节点ParentID
    if (0 != subVirtualPlatCfg.parental) {     
        tinyxml2::XMLElement* itemParentIdElem = doc.NewElement("ParentID");
        itemParentIdElem->SetText(subVirtualPlatCfg.parentID.c_str());
        itemElem->InsertEndChild(itemParentIdElem);
    }

    // 创建子节点item的子节点PlatformID
    tinyxml2::XMLElement* itemPlatformIdElem = doc.NewElement("PlatformID");
    itemPlatformIdElem->SetText(subVirtualPlatCfg.platformID.c_str());
    itemElem->InsertEndChild(itemPlatformIdElem);

    // 创建子节点item的子节点SafetyWay
    tinyxml2::XMLElement* itemSafetyWayElem = doc.NewElement("SafetyWay");
    itemSafetyWayElem->SetText(std::to_string(subVirtualPlatCfg.safetyWay).c_str());
    itemElem->InsertEndChild(itemSafetyWayElem);

    // 创建子节点item的子节点RegisterWay
    tinyxml2::XMLElement* itemRegisterWayElem = doc.NewElement("RegisterWay");
    itemRegisterWayElem->SetText(std::to_string(subVirtualPlatCfg.registerWay).c_str());
    itemElem->InsertEndChild(itemRegisterWayElem);

    // 创建子节点item的子节点Secrecy
    tinyxml2::XMLElement* itemSecrecyElem = doc.NewElement("Secrecy");
    itemSecrecyElem->SetText(std::to_string(subVirtualPlatCfg.secrecy).c_str());
    itemElem->InsertEndChild(itemSecrecyElem);

    // 创建子节点item的子节点Status
    tinyxml2::XMLElement* itemStatusElem = doc.NewElement("Status");
    itemStatusElem->SetText(subVirtualPlatCfg.status.c_str());
    itemElem->InsertEndChild(itemStatusElem);

    // 将 XML 文档转换为字符串
    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);
    msgBody = printer.CStr();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyXmlHelper::GenerateSipCatalogDeviceCfgMsgBody(const MySipCatalogDeviceCfg_dt& deviceCfg, const std::string& sn, 
                                                           std::size_t sumNum, std::string& msgBody)
{
    // 创建一个 XML 文档对象
    tinyxml2::XMLDocument doc;

    // 添加 XML 声明（<?xml version="1.0" encoding="UTF-8"?>）
    doc.InsertFirstChild(doc.NewDeclaration());

    // 创建根节点Response
    tinyxml2::XMLElement* rootElem = doc.NewElement("Response");
    doc.InsertEndChild(rootElem);

    // 创建子节点CmdType
    tinyxml2::XMLElement* cmdTypeElem = doc.NewElement("CmdType");
    cmdTypeElem->SetText("Catalog");
    rootElem->InsertEndChild(cmdTypeElem);

    // 创建子节点SN
    tinyxml2::XMLElement* snElem = doc.NewElement("SN");
    snElem->SetText(sn.c_str());
    rootElem->InsertEndChild(snElem);

    // 创建子节点DeviceID
    tinyxml2::XMLElement* deviceIdElem = doc.NewElement("DeviceID");
    deviceIdElem->SetText(deviceCfg.platformID.c_str());
    rootElem->InsertEndChild(deviceIdElem);

    // 创建子节点SumNum
    tinyxml2::XMLElement* sumNumElem = doc.NewElement("SumNum");
    sumNumElem->SetText(std::to_string(sumNum).c_str());
    rootElem->InsertEndChild(sumNumElem);

    // 创建子节点DeviceList
    tinyxml2::XMLElement* deviceListElem = doc.NewElement("DeviceList");
    deviceListElem->SetAttribute("Num", "1");
    rootElem->InsertEndChild(deviceListElem);

    // 创建子节点DeviceList的子节点item
    tinyxml2::XMLElement* itemElem = doc.NewElement("Item");
    deviceListElem->InsertEndChild(itemElem);

    // 创建子节点item的子节点DeviceID
    tinyxml2::XMLElement* itemDeviceIdElem = doc.NewElement("DeviceID");
    itemDeviceIdElem->SetText(deviceCfg.deviceID.c_str());
    itemElem->InsertEndChild(itemDeviceIdElem);

    // 创建子节点item的子节点Name
    tinyxml2::XMLElement* itemNameElem = doc.NewElement("Name");
    itemNameElem->SetText(deviceCfg.name.c_str());
    itemElem->InsertEndChild(itemNameElem);

    // 创建子节点item的子节点Manufacturer
    tinyxml2::XMLElement* itemManufacturerElem = doc.NewElement("Manufacturer");
    itemManufacturerElem->SetText((deviceCfg.manufacturer.empty() ? "unknown" : deviceCfg.manufacturer.c_str()));
    itemElem->InsertEndChild(itemManufacturerElem);

    // 创建子节点item的子节点Model
    tinyxml2::XMLElement* itemModelElem = doc.NewElement("Model");
    itemModelElem->SetText((deviceCfg.model.empty() ? "unknown" : deviceCfg.model.c_str()));
    itemElem->InsertEndChild(itemModelElem);

    // 创建子节点item的子节点Owner
    tinyxml2::XMLElement* itemOwnerElem = doc.NewElement("Owner");
    itemOwnerElem->SetText(deviceCfg.owner.c_str());
    itemElem->InsertEndChild(itemOwnerElem);

    // 创建子节点item的子节点CivilCode
    tinyxml2::XMLElement* itemCivilCodeElem = doc.NewElement("CivilCode");
    itemCivilCodeElem->SetText(deviceCfg.deviceID.substr(0, 6).c_str());
    itemElem->InsertEndChild(itemCivilCodeElem);

    // 创建子节点item的子节点Address
    tinyxml2::XMLElement* itemAddressElem = doc.NewElement("Address");
    itemAddressElem->SetText(deviceCfg.deviceIp.c_str());
    itemElem->InsertEndChild(itemAddressElem);

    // 创建子节点item的子节点Parental
    tinyxml2::XMLElement* itemParentalElem = doc.NewElement("Parental");
    itemParentalElem->SetText(std::to_string(deviceCfg.parental).c_str());
    itemElem->InsertEndChild(itemParentalElem);

    // 创建子节点item的子节点ParentID
    if (0 != deviceCfg.parental) {     
        tinyxml2::XMLElement* itemParentIdElem = doc.NewElement("ParentID");
        itemParentIdElem->SetText(deviceCfg.parentID.c_str());
        itemElem->InsertEndChild(itemParentIdElem);
    }

    // 创建子节点item的子节点PlatformID
    tinyxml2::XMLElement* itemPlatformIdElem = doc.NewElement("PlatformID");
    itemPlatformIdElem->SetText(deviceCfg.platformID.c_str());
    itemElem->InsertEndChild(itemPlatformIdElem);

    // 创建子节点item的子节点SafetyWay
    tinyxml2::XMLElement* itemSafetyWayElem = doc.NewElement("SafetyWay");
    itemSafetyWayElem->SetText(std::to_string(deviceCfg.safetyWay).c_str());
    itemElem->InsertEndChild(itemSafetyWayElem);

    // 创建子节点item的子节点RegisterWay
    tinyxml2::XMLElement* itemRegisterWayElem = doc.NewElement("RegisterWay");
    itemRegisterWayElem->SetText(std::to_string(deviceCfg.registerWay).c_str());
    itemElem->InsertEndChild(itemRegisterWayElem);

    // 创建子节点item的子节点Secrecy
    tinyxml2::XMLElement* itemSecrecyElem = doc.NewElement("Secrecy");
    itemSecrecyElem->SetText(std::to_string(deviceCfg.secrecy).c_str());
    itemElem->InsertEndChild(itemSecrecyElem);

    // 创建子节点item的子节点Status
    tinyxml2::XMLElement* itemStatusElem = doc.NewElement("Status");
    itemStatusElem->SetText(deviceCfg.status.c_str());
    itemElem->InsertEndChild(itemStatusElem);

    // 将 XML 文档转换为字符串
    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);
    msgBody = printer.CStr();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyXmlHelper::ParseSipCatalogRespMsgBody(const std::string& xmlStr, MySipCatalogRespMsgBody_dt& catalogRespMsgBody)
{
    tinyxml2::XMLDocument doc;

    // 解析XML数据
    if (tinyxml2::XML_SUCCESS != doc.Parse(xmlStr.c_str())) {
        return MyStatus_t::FAILED;
    }

    // 获取根元素
    tinyxml2::XMLElement* root = doc.RootElement();
    if (nullptr == root) {
        return MyStatus_t::FAILED;
    }

    // 获取子元素
    tinyxml2::XMLElement* cmdTypeElement = root->FirstChildElement("CmdType");
    if (nullptr != cmdTypeElement) {
        catalogRespMsgBody.cmdType = cmdTypeElement->GetText();
    }

    tinyxml2::XMLElement* deviceIDElement = root->FirstChildElement("DeviceID");
    if (nullptr != deviceIDElement) {
        catalogRespMsgBody.deviceId = deviceIDElement->GetText();
    }

    tinyxml2::XMLElement* snElement = root->FirstChildElement("SN");
    if (nullptr != snElement) {
        catalogRespMsgBody.sn = snElement->GetText();
    }

    tinyxml2::XMLElement* sumNumElement = root->FirstChildElement("SumNum");
    if (nullptr != sumNumElement) {
        catalogRespMsgBody.sumNum = std::stoul(sumNumElement->GetText());
    }

    tinyxml2::XMLElement* deviceListElement = root->FirstChildElement("DeviceList");
    if (nullptr != deviceListElement) {
        catalogRespMsgBody.deviceList = std::stoul(deviceListElement->Attribute("Num"));
    }

    // 获取子元素item
    tinyxml2::XMLElement* itemElement = root->FirstChildElement("Item");
    while (nullptr != itemElement) {
        std::string deviceID;
        tinyxml2::XMLElement* itemDeviceIdElement = itemElement->FirstChildElement("DeviceID");
        if (nullptr != itemDeviceIdElement) {
            deviceID = itemDeviceIdElement->GetText();
        }

        std::string name;
        tinyxml2::XMLElement* itemNameElement = itemElement->FirstChildElement("Name");
        if (nullptr != itemNameElement) {
            name = itemNameElement->GetText();
        }

        std::string manufacturer;
        tinyxml2::XMLElement* itemManufacturerElement = itemElement->FirstChildElement("Manufacturer");
        if (nullptr != itemManufacturerElement) {
            manufacturer = itemManufacturerElement->GetText();
        }

        std::string model;
        tinyxml2::XMLElement* itemModelElement = itemElement->FirstChildElement("Model");
        if (nullptr != itemModelElement) {
            model = itemModelElement->GetText();
        }

        std::string owner;
        tinyxml2::XMLElement* itemOwnerElement = itemElement->FirstChildElement("Owner");
        if (nullptr != itemOwnerElement) {
            owner = itemOwnerElement->GetText();
        }

        std::string address;
        tinyxml2::XMLElement* itemAddressElement = itemElement->FirstChildElement("Address");
        if (nullptr != itemAddressElement) {
            address = itemAddressElement->GetText();
        }

        unsigned int parental = 0;
        tinyxml2::XMLElement* itemParentalElement = itemElement->FirstChildElement("Parental");
        if (nullptr != itemParentalElement) {
            parental = std::stoi(itemParentalElement->GetText());
        }

        std::string parentID;
        tinyxml2::XMLElement* itemParentIdElement = itemElement->FirstChildElement("ParentID");
        if (nullptr != itemParentIdElement) {
            parentID = itemParentIdElement->GetText();
        }

        std::string platformID;
        tinyxml2::XMLElement* itemPlatformIDElement = itemElement->FirstChildElement("PlatformID");
        if (nullptr != itemPlatformIDElement) {
            platformID = itemPlatformIDElement->GetText();
        }

        unsigned int safetyWay = 0;
        tinyxml2::XMLElement* itemSafetyWayElement = itemElement->FirstChildElement("SafetyWay");
        if (nullptr != itemSafetyWayElement) {
            safetyWay = std::stoi(itemSafetyWayElement->GetText());
        }

        unsigned int registerWay = 0;
        tinyxml2::XMLElement* itemRegisterWayElement = itemElement->FirstChildElement("RegisterWay");
        if (nullptr != itemRegisterWayElement) {
            registerWay = std::stoi(itemRegisterWayElement->GetText());
        }

        unsigned int secrecy = 0;
        tinyxml2::XMLElement* itemSecrecyElement = itemElement->FirstChildElement("Secrecy");
        if (nullptr != itemSecrecyElement) {
            secrecy = std::stoi(itemSecrecyElement->GetText());
        }

        std::string status;
        tinyxml2::XMLElement* itemStatusElement = itemElement->FirstChildElement("Status");
        if (nullptr != itemStatusElement) {
            status = itemStatusElement->GetText();
        }

        std::string deviceType = deviceID.substr(10, 3);
        if ("200" == deviceType) {
            MySipCatalogPlatCfg_dt platCfg;
            platCfg.deviceID     = deviceID;
            platCfg.manufacturer = manufacturer;
            platCfg.model        = model;
            platCfg.owner        = owner;
            platCfg.name         = name;
            platCfg.deviceIp     = address;
            platCfg.parental     = parental;
            platCfg.parentID     = parentID;
            platCfg.platformID   = platformID;
            platCfg.safetyWay    = safetyWay;
            platCfg.registerWay  = registerWay;
            platCfg.secrecy      = secrecy;
            platCfg.status       = status;

            if (catalogRespMsgBody.platCfgMap.end() == catalogRespMsgBody.platCfgMap.find(deviceID)) {
                catalogRespMsgBody.platCfgMap.insert(std::make_pair(deviceID, platCfg));
            }
        }
        else if ("215" == deviceType) {
            MySipCatalogSubPlatCfg_dt subPlatCfg;
            subPlatCfg.deviceID     = deviceID;
            subPlatCfg.manufacturer = manufacturer;
            subPlatCfg.model        = model;
            subPlatCfg.owner        = owner;
            subPlatCfg.name         = name;
            subPlatCfg.deviceIp     = address;
            subPlatCfg.parental     = parental;
            subPlatCfg.parentID     = parentID;
            subPlatCfg.platformID   = platformID;
            subPlatCfg.safetyWay    = safetyWay;
            subPlatCfg.registerWay  = registerWay;
            subPlatCfg.secrecy      = secrecy;
            subPlatCfg.status       = status;

            if (catalogRespMsgBody.subPlatCfgMap.end() == catalogRespMsgBody.subPlatCfgMap.find(deviceID)) {
                catalogRespMsgBody.subPlatCfgMap.insert(std::make_pair(deviceID, subPlatCfg));
            }
        }
        else if ("216" == deviceType) {
            MySipCatalogVirtualSubPlatCfg_dt virtualSubPlatCfg;
            virtualSubPlatCfg.deviceID     = deviceID;
            virtualSubPlatCfg.manufacturer = manufacturer;
            virtualSubPlatCfg.model        = model;
            virtualSubPlatCfg.owner        = owner;
            virtualSubPlatCfg.name         = name;
            virtualSubPlatCfg.deviceIp     = address;
            virtualSubPlatCfg.parental     = parental;
            virtualSubPlatCfg.parentID     = parentID;
            virtualSubPlatCfg.platformID   = platformID;
            virtualSubPlatCfg.safetyWay    = safetyWay;
            virtualSubPlatCfg.registerWay  = registerWay;
            virtualSubPlatCfg.secrecy      = secrecy;
            virtualSubPlatCfg.status       = status;

            if (catalogRespMsgBody.subVirtualPlatCfgMap.end() == catalogRespMsgBody.subVirtualPlatCfgMap.find(deviceID)) {
                catalogRespMsgBody.subVirtualPlatCfgMap.insert(std::make_pair(deviceID, virtualSubPlatCfg));
            }
        }
        else if ("131" == deviceType || "132" == deviceType) {
            MySipCatalogDeviceCfg_dt deviceCfg;
            deviceCfg.deviceID     = deviceID;
            deviceCfg.manufacturer = manufacturer;
            deviceCfg.model        = model;
            deviceCfg.owner        = owner;
            deviceCfg.name         = name;
            deviceCfg.deviceIp     = address;
            deviceCfg.parental     = parental;
            deviceCfg.parentID     = parentID;
            deviceCfg.platformID   = platformID;
            deviceCfg.safetyWay    = safetyWay;
            deviceCfg.registerWay  = registerWay;
            deviceCfg.secrecy      = secrecy;
            deviceCfg.status       = status;

            if (catalogRespMsgBody.deviceCfgMap.end() == catalogRespMsgBody.deviceCfgMap.find(deviceID)) {
                catalogRespMsgBody.deviceCfgMap.insert(std::make_pair(deviceID, deviceCfg));
            }
        }
        
        // 移动到下一个 <Item>
        itemElement = itemElement->NextSiblingElement("Item");
    }
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_UTILS