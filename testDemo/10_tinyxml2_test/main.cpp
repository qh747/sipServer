#include <tinyxml2.h>
#include <iostream>
#include <string>

std::string GenerateXmlStr(const std::string idx, const std::string& id)
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
    deviceID->SetText(id.c_str());
    root->InsertEndChild(deviceID);

    // 创建子节点 <Status> 并设置内容
    tinyxml2::XMLElement* status = doc.NewElement("Status");
    status->SetText("OK");
    root->InsertEndChild(status);

    // 创建子节点 <Info> 并设置内容（空内容）
    tinyxml2::XMLElement* info = doc.NewElement("Info");
    info->SetText(" ");
    root->InsertEndChild(info);

    // 将 XML 文档转换为字符串
    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);
    std::string xmlString = printer.CStr();
    return xmlString;
}

int main() {
    // 输出生成的 XML 字符串
    std::cout << "Generated XML:\n" << GenerateXmlStr("123456", "112233") << std::endl;
    return 0;
}