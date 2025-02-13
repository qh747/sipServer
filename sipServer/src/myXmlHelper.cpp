#include "utils/myStrHelper.h"
#include "utils/myXmlHelper.h"
using namespace tinyxml2;
using namespace MY_COMMON;

namespace MY_UTILS {

std::string MyXmlHelper::GenerateSipKeepAliveMsgBody(const std::string& idx, const std::string& localServId)
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

    // 创建子节点 <Info> 并设置内容（空内容）
    tinyxml2::XMLElement* info = doc.NewElement("Info");
    info->SetText("");
    root->InsertEndChild(info);

    // 将 XML 文档转换为字符串
    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);
    std::string xmlString = printer.CStr();
    return xmlString;
}

MyStatus_t MyXmlHelper::ParseSipKeepAliveMsgBody(const std::string& xmlStr, MySipKeepAliveMsgBidy_dt& keepAliveMsgBody)
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
        keepAliveMsgBody.status = ("OK" == MyStrHelper::ConvertToUpStr(statusElement->GetText()) ? true : false);
    }
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_UTILS