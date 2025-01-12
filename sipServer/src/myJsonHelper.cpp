#include <fstream>
#include <iostream>
#include <cstdbool>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "utils/myJsonHelper.h"

namespace MY_UTILS {

MyStatus_t MyJsonHelper::ParseServRegJsonFile(const std::string& filePath, RegUpServJsonMap& regUpServMap, RegLowServJsonMap& regLowServMap)
{
    if (filePath.empty()) {
        LOG(ERROR) << "json file parse failed. filePath is empty.";
        return MyStatus_t::FAILED;
    }

    // 读取 json 文件
    std::ifstream ifs(filePath.c_str());
    Json::Value rootNode;
    ifs >> rootNode;

    // 解析json根节点
    if (!(rootNode.isMember("registerServer"))) {
        LOG(ERROR) << "json file parse failed. root node is not registerServer.";
        return MyStatus_t::FAILED;
    }
    const Json::Value& regServNode = rootNode["registerServer"];

    // 解析上级注册服务
    if (!(regServNode.isMember("upperServer"))) {
        LOG(ERROR) << "json file parse failed. upperServer node is not exist.";
        return MyStatus_t::FAILED;
    }
    const Json::Value& upServNodes = regServNode["upperServer"];
    for (const auto& curServNode : upServNodes) {
        std::string id     = curServNode["id"].asString();
        std::string ipAddr = curServNode["ipAddr"].asString();
        uint16_t    port   = curServNode["port"].asUInt();
        std::string name   = curServNode["name"].asString();
        std::string domain = curServNode["domain"].asString();
        double      expired = curServNode["expired"].asDouble();    

        if (id.empty() || ipAddr.empty() || (0 == port) || name.empty() || domain.empty() || (0 == expired)) {
            LOG(WARNING) << "json file parse failed. invalid upper server info. id: " << id << " ,name: " << name << ", ipAddr: " << ipAddr 
                         << ", port: " << port << ", domain: " << domain << ", expired: " << expired << ".";
            continue;
        }
        else if (regUpServMap.end() != regUpServMap.find(id)) {
            LOG(WARNING) << "json file parse failed. conflict upper server id: " << id << ".";
            continue;
        }

        MySipRegUpServInfo_dt regUpServInfo;
        regUpServInfo.id       = id;
        regUpServInfo.ipAddr   = ipAddr;
        regUpServInfo.port     = port;
        regUpServInfo.name     = name;
        regUpServInfo.domain   = domain;
        regUpServInfo.duration = expired;
        regUpServMap.insert(std::make_pair(id, std::move(regUpServInfo)));
    }

    // 解析下级注册服务
    if (!(regServNode.isMember("lowerServer"))) {
        LOG(ERROR) << "json file parse failed. lowerServer node is not exist.";
        return MyStatus_t::FAILED;
    }
    const Json::Value& lowServNodes = regServNode["lowerServer"];
    for (const auto& curServNode : lowServNodes) {
        std::string id     = curServNode["id"].asString();
        std::string ipAddr = curServNode["ipAddr"].asString();
        uint16_t    port   = curServNode["port"].asUInt();
        std::string name   = curServNode["name"].asString();
        std::string domain = curServNode["domain"].asString();
        double      expired = curServNode["expired"].asDouble();   

        if (id.empty() || ipAddr.empty() || (0 == port) || name.empty() || domain.empty() || (0 == expired)) {
            LOG(WARNING) << "json file parse failed. invalid lower server info. id: " << id << " ,name: " << name << ", ipAddr: " << ipAddr 
                         << ", port: " << port << ", domain: " << domain << ", expired: " << expired << ".";
            continue;
        }
        else if (regLowServMap.end() != regLowServMap.find(id)) {
            LOG(WARNING) << "json file parse failed. conflict lower server id: " << id << ".";
            continue;
        }

        MySipRegLowServInfo_dt regLowServInfo;
        regLowServInfo.id       = id;
        regLowServInfo.ipAddr   = ipAddr;
        regLowServInfo.port     = port;
        regLowServInfo.name     = name;
        regLowServInfo.domain   = domain;
        regLowServInfo.duration = expired;
        regLowServMap.insert(std::make_pair(id, std::move(regLowServInfo)));
    }

    ifs.close();
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_UTILS