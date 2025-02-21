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
    itemOwnerElem->SetText("Owner");
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

MyStatus_t MyXmlHelper::ParseSipCatalogPlatCfgMsgBody(const std::string& xmlStr, MySipCatalogPlatCfgMsgBody_dt& catalogPlatCfgMsgBody)
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
        catalogPlatCfgMsgBody.cmdType = cmdTypeElement->GetText();
    }

    tinyxml2::XMLElement* deviceIDElement = root->FirstChildElement("DeviceID");
    if (nullptr != deviceIDElement) {
        catalogPlatCfgMsgBody.deviceId = deviceIDElement->GetText();
    }

    tinyxml2::XMLElement* snElement = root->FirstChildElement("SN");
    if (nullptr != snElement) {
        catalogPlatCfgMsgBody.sn = snElement->GetText();
    }

    tinyxml2::XMLElement* sumNumElement = root->FirstChildElement("SumNum");
    if (nullptr != sumNumElement) {
        catalogPlatCfgMsgBody.sumNum = std::stoul(sumNumElement->GetText());
    }

    tinyxml2::XMLElement* deviceListElement = root->FirstChildElement("DeviceList");
    if (nullptr != deviceListElement) {
        catalogPlatCfgMsgBody.deviceList = std::stoul(deviceListElement->GetText());
    }

    std::size_t deviceCount = std::stoul(deviceListElement->Attribute("Num"));
    if (0 == deviceCount) {
        return MyStatus_t::FAILED;
    }

    // 获取子元素item
    tinyxml2::XMLElement* itemElement = root->FirstChildElement("Item");
    while (nullptr != itemElement) {
        // 配置解析
        MySipCatalogPlatCfg_dt platCfg;

        tinyxml2::XMLElement* itemDeviceIdElement = itemElement->FirstChildElement("DeviceID");
        if (nullptr != itemDeviceIdElement) {
            platCfg.deviceID = itemDeviceIdElement->GetText();
        }

        tinyxml2::XMLElement* itemNameElement = itemElement->FirstChildElement("Name");
        if (nullptr != itemNameElement) {
            platCfg.name = itemNameElement->GetText();
        }

        tinyxml2::XMLElement* itemManufacturerElement = itemElement->FirstChildElement("Manufacturer");
        if (nullptr != itemManufacturerElement) {
            platCfg.manufacturer = itemManufacturerElement->GetText();
        }

        tinyxml2::XMLElement* itemModelElement = itemElement->FirstChildElement("Model");
        if (nullptr != itemModelElement) {
            platCfg.model = itemModelElement->GetText();
        }

        tinyxml2::XMLElement* itemParentalElement = itemElement->FirstChildElement("Parental");
        if (nullptr != itemParentalElement) {
            platCfg.parental = std::stoi(itemParentalElement->GetText());
        }

        if (0 != platCfg.parental) {
            tinyxml2::XMLElement* itemParentIdElement = itemElement->FirstChildElement("ParentID");
            if (nullptr != itemParentIdElement) {
                platCfg.parentID = itemParentIdElement->GetText();
            }
        }

        tinyxml2::XMLElement* itemSafetyWayElement = itemElement->FirstChildElement("SafetyWay");
        if (nullptr != itemSafetyWayElement) {
            platCfg.safetyWay = std::stoi(itemSafetyWayElement->GetText());
        }

        tinyxml2::XMLElement* itemRegisterWayElement = itemElement->FirstChildElement("RegisterWay");
        if (nullptr != itemRegisterWayElement) {
            platCfg.registerWay = std::stoi(itemRegisterWayElement->GetText());
        }

        tinyxml2::XMLElement* itemSecrecyElement = itemElement->FirstChildElement("Secrecy");
        if (nullptr != itemSecrecyElement) {
            platCfg.secrecy = std::stoi(itemSecrecyElement->GetText());
        }

        tinyxml2::XMLElement* itemStatusElement = itemElement->FirstChildElement("Status");
        if (nullptr != itemStatusElement) {
            platCfg.status = itemStatusElement->GetText();
        }

        // 添加配置
        if (catalogPlatCfgMsgBody.platCfgMap.end() == catalogPlatCfgMsgBody.platCfgMap.find(platCfg.deviceID)) {
            catalogPlatCfgMsgBody.platCfgMap.insert(std::make_pair(platCfg.deviceID, platCfg));
        }

        // 移动到下一个 <Item>
        itemElement = itemElement->NextSiblingElement("Item");
    }
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
    itemOwnerElem->SetText("Owner");
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

MyStatus_t MyXmlHelper::ParseSipCatalogSubPlatCfgMsgBody(const std::string& xmlStr, MySipCatalogSubPlatCfgMsgBody_dt& catalogSubPlatCfgMsgBody)
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
        catalogSubPlatCfgMsgBody.cmdType = cmdTypeElement->GetText();
    }

    tinyxml2::XMLElement* deviceIDElement = root->FirstChildElement("DeviceID");
    if (nullptr != deviceIDElement) {
        catalogSubPlatCfgMsgBody.deviceId = deviceIDElement->GetText();
    }

    tinyxml2::XMLElement* snElement = root->FirstChildElement("SN");
    if (nullptr != snElement) {
        catalogSubPlatCfgMsgBody.sn = snElement->GetText();
    }

    tinyxml2::XMLElement* sumNumElement = root->FirstChildElement("SumNum");
    if (nullptr != sumNumElement) {
        catalogSubPlatCfgMsgBody.sumNum = std::stoul(sumNumElement->GetText());
    }

    tinyxml2::XMLElement* deviceListElement = root->FirstChildElement("DeviceList");
    if (nullptr != deviceListElement) {
        catalogSubPlatCfgMsgBody.deviceList = std::stoul(deviceListElement->GetText());
    }

    std::size_t deviceCount = std::stoul(deviceListElement->Attribute("Num"));
    if (0 == deviceCount) {
        return MyStatus_t::FAILED;
    }

    // 获取子元素item
    tinyxml2::XMLElement* itemElement = root->FirstChildElement("Item");
    while (nullptr != itemElement) {
        // 配置解析
        MySipCatalogSubPlatCfg_dt subPlatCfg;

        tinyxml2::XMLElement* itemDeviceIdElement = itemElement->FirstChildElement("DeviceID");
        if (nullptr != itemDeviceIdElement) {
            subPlatCfg.deviceID = itemDeviceIdElement->GetText();
        }

        tinyxml2::XMLElement* itemNameElement = itemElement->FirstChildElement("Name");
        if (nullptr != itemNameElement) {
            subPlatCfg.name = itemNameElement->GetText();
        }

        tinyxml2::XMLElement* itemManufacturerElement = itemElement->FirstChildElement("Manufacturer");
        if (nullptr != itemManufacturerElement) {
            subPlatCfg.manufacturer = itemManufacturerElement->GetText();
        }

        tinyxml2::XMLElement* itemModelElement = itemElement->FirstChildElement("Model");
        if (nullptr != itemModelElement) {
            subPlatCfg.model = itemModelElement->GetText();
        }

        tinyxml2::XMLElement* itemParentalElement = itemElement->FirstChildElement("Parental");
        if (nullptr != itemParentalElement) {
            subPlatCfg.parental = std::stoi(itemParentalElement->GetText());
        }

        tinyxml2::XMLElement* itemParentIdElement = itemElement->FirstChildElement("ParentID");
        if (nullptr != itemParentIdElement) {
            subPlatCfg.parentID = itemParentIdElement->GetText();
        }

        tinyxml2::XMLElement* itemSafetyWayElement = itemElement->FirstChildElement("SafetyWay");
        if (nullptr != itemSafetyWayElement) {
            subPlatCfg.safetyWay = std::stoi(itemSafetyWayElement->GetText());
        }

        tinyxml2::XMLElement* itemRegisterWayElement = itemElement->FirstChildElement("RegisterWay");
        if (nullptr != itemRegisterWayElement) {
            subPlatCfg.registerWay = std::stoi(itemRegisterWayElement->GetText());
        }

        tinyxml2::XMLElement* itemSecrecyElement = itemElement->FirstChildElement("Secrecy");
        if (nullptr != itemSecrecyElement) {
            subPlatCfg.secrecy = std::stoi(itemSecrecyElement->GetText());
        }

        tinyxml2::XMLElement* itemStatusElement = itemElement->FirstChildElement("Status");
        if (nullptr != itemStatusElement) {
            subPlatCfg.status = itemStatusElement->GetText();
        }

        // 添加配置
        if (catalogSubPlatCfgMsgBody.subPlatCfgMap.end() == catalogSubPlatCfgMsgBody.subPlatCfgMap.find(subPlatCfg.deviceID)) {
            catalogSubPlatCfgMsgBody.subPlatCfgMap.insert(std::make_pair(subPlatCfg.deviceID, subPlatCfg));
        }

        // 移动到下一个 <Item>
        itemElement = itemElement->NextSiblingElement("Item");
    }
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
    itemOwnerElem->SetText("Owner");
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

MyStatus_t MyXmlHelper::ParseSipCatalogSubVirtualPlatCfgMsgBody(const std::string& xmlStr, MySipCatalogSubVirtualPlatCfgMsgBody_dt& catalogSubVirtualPlatCfgMsgBody)
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
        catalogSubVirtualPlatCfgMsgBody.cmdType = cmdTypeElement->GetText();
    }

    tinyxml2::XMLElement* deviceIDElement = root->FirstChildElement("DeviceID");
    if (nullptr != deviceIDElement) {
        catalogSubVirtualPlatCfgMsgBody.deviceId = deviceIDElement->GetText();
    }

    tinyxml2::XMLElement* snElement = root->FirstChildElement("SN");
    if (nullptr != snElement) {
        catalogSubVirtualPlatCfgMsgBody.sn = snElement->GetText();
    }

    tinyxml2::XMLElement* sumNumElement = root->FirstChildElement("SumNum");
    if (nullptr != sumNumElement) {
        catalogSubVirtualPlatCfgMsgBody.sumNum = std::stoul(sumNumElement->GetText());
    }

    tinyxml2::XMLElement* deviceListElement = root->FirstChildElement("DeviceList");
    if (nullptr != deviceListElement) {
        catalogSubVirtualPlatCfgMsgBody.deviceList = std::stoul(deviceListElement->GetText());
    }

    std::size_t deviceCount = std::stoul(deviceListElement->Attribute("Num"));
    if (0 == deviceCount) {
        return MyStatus_t::FAILED;
    }

    // 获取子元素item
    tinyxml2::XMLElement* itemElement = root->FirstChildElement("Item");
    while (nullptr != itemElement) {
        // 配置解析
        MySipCatalogVirtualSubPlatCfg_dt subVirtualPlatCfg;

        tinyxml2::XMLElement* itemDeviceIdElement = itemElement->FirstChildElement("DeviceID");
        if (nullptr != itemDeviceIdElement) {
            subVirtualPlatCfg.deviceID = itemDeviceIdElement->GetText();
        }

        tinyxml2::XMLElement* itemNameElement = itemElement->FirstChildElement("Name");
        if (nullptr != itemNameElement) {
            subVirtualPlatCfg.name = itemNameElement->GetText();
        }

        tinyxml2::XMLElement* itemManufacturerElement = itemElement->FirstChildElement("Manufacturer");
        if (nullptr != itemManufacturerElement) {
            subVirtualPlatCfg.manufacturer = itemManufacturerElement->GetText();
        }

        tinyxml2::XMLElement* itemModelElement = itemElement->FirstChildElement("Model");
        if (nullptr != itemModelElement) {
            subVirtualPlatCfg.model = itemModelElement->GetText();
        }

        tinyxml2::XMLElement* itemParentalElement = itemElement->FirstChildElement("Parental");
        if (nullptr != itemParentalElement) {
            subVirtualPlatCfg.parental = std::stoi(itemParentalElement->GetText());
        }

        tinyxml2::XMLElement* itemParentIdElement = itemElement->FirstChildElement("ParentID");
        if (nullptr != itemParentIdElement) {
            subVirtualPlatCfg.parentID = itemParentIdElement->GetText();
        }

        tinyxml2::XMLElement* itemSafetyWayElement = itemElement->FirstChildElement("SafetyWay");
        if (nullptr != itemSafetyWayElement) {
            subVirtualPlatCfg.safetyWay = std::stoi(itemSafetyWayElement->GetText());
        }

        tinyxml2::XMLElement* itemRegisterWayElement = itemElement->FirstChildElement("RegisterWay");
        if (nullptr != itemRegisterWayElement) {
            subVirtualPlatCfg.registerWay = std::stoi(itemRegisterWayElement->GetText());
        }

        tinyxml2::XMLElement* itemSecrecyElement = itemElement->FirstChildElement("Secrecy");
        if (nullptr != itemSecrecyElement) {
            subVirtualPlatCfg.secrecy = std::stoi(itemSecrecyElement->GetText());
        }

        tinyxml2::XMLElement* itemStatusElement = itemElement->FirstChildElement("Status");
        if (nullptr != itemStatusElement) {
            subVirtualPlatCfg.status = itemStatusElement->GetText();
        }

        // 添加配置
        if (catalogSubVirtualPlatCfgMsgBody.subVirtualPlatCfgMap.end() == catalogSubVirtualPlatCfgMsgBody.subVirtualPlatCfgMap.find(subVirtualPlatCfg.deviceID)) {
            catalogSubVirtualPlatCfgMsgBody.subVirtualPlatCfgMap.insert(std::make_pair(subVirtualPlatCfg.deviceID, subVirtualPlatCfg));
        }

        // 移动到下一个 <Item>
        itemElement = itemElement->NextSiblingElement("Item");
    }
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
    itemOwnerElem->SetText("Owner");
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

MyStatus_t MyXmlHelper::ParseSipCatalogDeviceCfgMsgBody(const std::string& xmlStr, MySipCatalogDeviceCfgMsgBody_dt& catalogDeviceCfgMsgBody)
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
        catalogDeviceCfgMsgBody.cmdType = cmdTypeElement->GetText();
    }

    tinyxml2::XMLElement* deviceIDElement = root->FirstChildElement("DeviceID");
    if (nullptr != deviceIDElement) {
        catalogDeviceCfgMsgBody.deviceId = deviceIDElement->GetText();
    }

    tinyxml2::XMLElement* snElement = root->FirstChildElement("SN");
    if (nullptr != snElement) {
        catalogDeviceCfgMsgBody.sn = snElement->GetText();
    }

    tinyxml2::XMLElement* sumNumElement = root->FirstChildElement("SumNum");
    if (nullptr != sumNumElement) {
        catalogDeviceCfgMsgBody.sumNum = std::stoul(sumNumElement->GetText());
    }

    tinyxml2::XMLElement* deviceListElement = root->FirstChildElement("DeviceList");
    if (nullptr != deviceListElement) {
        catalogDeviceCfgMsgBody.deviceList = std::stoul(deviceListElement->GetText());
    }

    std::size_t deviceCount = std::stoul(deviceListElement->Attribute("Num"));
    if (0 == deviceCount) {
        return MyStatus_t::FAILED;
    }

    // 获取子元素item
    tinyxml2::XMLElement* itemElement = root->FirstChildElement("Item");
    while (nullptr != itemElement) {
        // 配置解析
        MySipCatalogDeviceCfg_dt deviceCfg;

        tinyxml2::XMLElement* itemDeviceIdElement = itemElement->FirstChildElement("DeviceID");
        if (nullptr != itemDeviceIdElement) {
            deviceCfg.deviceID = itemDeviceIdElement->GetText();
        }

        tinyxml2::XMLElement* itemNameElement = itemElement->FirstChildElement("Name");
        if (nullptr != itemNameElement) {
            deviceCfg.name = itemNameElement->GetText();
        }

        tinyxml2::XMLElement* itemManufacturerElement = itemElement->FirstChildElement("Manufacturer");
        if (nullptr != itemManufacturerElement) {
            deviceCfg.manufacturer = itemManufacturerElement->GetText();
        }

        tinyxml2::XMLElement* itemModelElement = itemElement->FirstChildElement("Model");
        if (nullptr != itemModelElement) {
            deviceCfg.model = itemModelElement->GetText();
        }

        tinyxml2::XMLElement* itemParentalElement = itemElement->FirstChildElement("Parental");
        if (nullptr != itemParentalElement) {
            deviceCfg.parental = std::stoi(itemParentalElement->GetText());
        }

        tinyxml2::XMLElement* itemParentIdElement = itemElement->FirstChildElement("ParentID");
        if (nullptr != itemParentIdElement) {
            deviceCfg.parentID = itemParentIdElement->GetText();
        }

        tinyxml2::XMLElement* itemSafetyWayElement = itemElement->FirstChildElement("SafetyWay");
        if (nullptr != itemSafetyWayElement) {
            deviceCfg.safetyWay = std::stoi(itemSafetyWayElement->GetText());
        }

        tinyxml2::XMLElement* itemRegisterWayElement = itemElement->FirstChildElement("RegisterWay");
        if (nullptr != itemRegisterWayElement) {
            deviceCfg.registerWay = std::stoi(itemRegisterWayElement->GetText());
        }

        tinyxml2::XMLElement* itemSecrecyElement = itemElement->FirstChildElement("Secrecy");
        if (nullptr != itemSecrecyElement) {
            deviceCfg.secrecy = std::stoi(itemSecrecyElement->GetText());
        }

        tinyxml2::XMLElement* itemStatusElement = itemElement->FirstChildElement("Status");
        if (nullptr != itemStatusElement) {
            deviceCfg.status = itemStatusElement->GetText();
        }

        // 添加配置
        if (catalogDeviceCfgMsgBody.deviceCfgMap.end() == catalogDeviceCfgMsgBody.deviceCfgMap.find(deviceCfg.deviceID)) {
            catalogDeviceCfgMsgBody.deviceCfgMap.insert(std::make_pair(deviceCfg.deviceID, deviceCfg));
        }

        // 移动到下一个 <Item>
        itemElement = itemElement->NextSiblingElement("Item");
    }
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_UTILS