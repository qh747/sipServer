#include <fstream>
#include <cstdint>
#include <cstdbool>
#include <json/json.h>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "utils/myJsonHelper.h"
using MY_COMMON::MyStatus_t;
using MY_COMMON::MyTpProto_t;
using MY_COMMON::MySipServAuthCfg_dt;
using MY_COMMON::MySipServAddrCfg_dt;
using MY_COMMON::MySipRegUpServCfg_dt;
using MY_COMMON::MySipRegLowServCfg_dt;

namespace MY_UTILS {

MyStatus_t MyJsonHelper::ParseSipServRegJsonFile(const std::string& filePath, SipRegUpServJsonMap& regUpServMap, SipRegLowServJsonMap& regLowServMap)
{
    if (filePath.empty()) {
        LOG(ERROR) << "Json file parse failed. filePath is empty.";
        return MyStatus_t::FAILED;
    }

    // 读取 json 文件
    std::ifstream ifs(filePath.c_str());
    Json::Value rootNode;
    ifs >> rootNode;

    // 解析json根节点
    if (!(rootNode.isMember("registerServer"))) {
        LOG(ERROR) << "Json file parse failed. root node is not registerServer.";
        return MyStatus_t::FAILED;
    }
    const Json::Value& regServNode = rootNode["registerServer"];

    // 解析上级注册服务
    if (!(regServNode.isMember("upperServer"))) {
        LOG(ERROR) << "Json file parse failed. upperServer node is not exist.";
        return MyStatus_t::FAILED;
    }

    const Json::Value& upServNodes = regServNode["upperServer"];
    for (const auto& curServNode : upServNodes) {
        MySipServAddrCfg_dt upServAddrCfg;
        upServAddrCfg.id = curServNode["id"].asString();

        if (regUpServMap.end() != regUpServMap.find(upServAddrCfg.id)) {
            LOG(WARNING) << "Json file parse failed. conflict upper server id: " << upServAddrCfg.id << ".";
            continue;
        }

        upServAddrCfg.ipAddr = curServNode["ipAddr"].asString();
        upServAddrCfg.port   = static_cast<uint16_t>(std::stoi(curServNode["port"].asString()));
        upServAddrCfg.name   = curServNode["name"].asString();
        upServAddrCfg.domain = curServNode["domain"].asString();

        if (upServAddrCfg.id.empty()   || upServAddrCfg.ipAddr.empty() || (0 == upServAddrCfg.port) || 
            upServAddrCfg.name.empty() || upServAddrCfg.domain.empty()) {
            LOG(WARNING) << "Json file parse failed. invalid upper server info. id: " << upServAddrCfg.id << " ,name: " << upServAddrCfg.name 
                         << ", ipAddr: " << upServAddrCfg.ipAddr << ", port: " << upServAddrCfg.port << ", domain: " << upServAddrCfg.domain << ".";
            continue;
        }

        double expired = static_cast<double>(std::stoi(curServNode["expired"].asString()));
        if (expired <= 0) {
            LOG(WARNING) << "Json file parse failed. invalid upper server expired. id: " << upServAddrCfg.id 
                         << ", ipAddr: " << upServAddrCfg.ipAddr << ", port: " << upServAddrCfg.port
                         << " ,expired: " << expired << ".";
            continue;
        } 

        std::string sProto = curServNode["proto"].asString();
        MyTpProto_t proto;
        if      ("udp" == sProto)   { proto = MyTpProto_t::UDP; }
        else if ("tcp" == sProto)   { proto = MyTpProto_t::TCP; }
        else                        { proto = MyTpProto_t::UDP; }
        
        MySipServAuthCfg_dt upServAuthCfg;
        if ("true" == curServNode["auth"].asString()) {
            upServAuthCfg.enableAuth  = true;
            upServAuthCfg.authName    = curServNode["authName"].asString();
            upServAuthCfg.authPwd     = curServNode["authPwd"].asString();
            upServAuthCfg.authRealm   = curServNode["authRealm"].asString();
        }
        else {
            upServAuthCfg.enableAuth  = false;
            upServAuthCfg.authName    = "";
            upServAuthCfg.authPwd     = "";
            upServAuthCfg.authRealm   = "";
        }

        MySipRegUpServCfg_dt upRegServCfg;
        upRegServCfg.upSipServAddrCfg   = std::move(upServAddrCfg);
        upRegServCfg.upSipServAuthCfg   = std::move(upServAuthCfg);
        upRegServCfg.expired            = expired;
        upRegServCfg.proto              = proto;
        regUpServMap.insert(std::make_pair(upRegServCfg.upSipServAddrCfg.id, std::move(upRegServCfg)));
    }

    // 解析下级注册服务
    if (!(regServNode.isMember("lowerServer"))) {
        LOG(ERROR) << "json file parse failed. lowerServer node is not exist.";
        return MyStatus_t::FAILED;
    }

    const Json::Value& lowServNodes = regServNode["lowerServer"];
    for (const auto& curServNode : lowServNodes) {
        MySipServAddrCfg_dt lowServAddrCfg;
        lowServAddrCfg.id = curServNode["id"].asString();

        if (regLowServMap.end() != regLowServMap.find(lowServAddrCfg.id)) {
            LOG(WARNING) << "Json file parse failed. conflict lower server id: " << lowServAddrCfg.id << ".";
            continue;
        }

        lowServAddrCfg.ipAddr = curServNode["ipAddr"].asString();
        lowServAddrCfg.port   = static_cast<uint16_t>(std::stoi(curServNode["port"].asString()));
        lowServAddrCfg.name   = curServNode["name"].asString();
        lowServAddrCfg.domain = curServNode["domain"].asString();

        if (lowServAddrCfg.id.empty()   || lowServAddrCfg.ipAddr.empty() || (0 == lowServAddrCfg.port) || 
            lowServAddrCfg.name.empty() || lowServAddrCfg.domain.empty()) {
            LOG(WARNING) << "Json file parse failed. invalid lower server info. id: " << lowServAddrCfg.id << " ,name: " << lowServAddrCfg.name 
                         << ", ipAddr: " << lowServAddrCfg.ipAddr << ", port: " << lowServAddrCfg.port << ", domain: " << lowServAddrCfg.domain << ".";
            continue;
        }

        double expired = static_cast<double>(std::stoi(curServNode["expired"].asString()));
        if (expired <= 0) {
            LOG(WARNING) << "Json file parse failed. invalid lower server expired. id: " << lowServAddrCfg.id 
                         << ", ipAddr: " << lowServAddrCfg.ipAddr << ", port: " << lowServAddrCfg.port
                         << " ,expired: " << expired << ".";
            continue;
        }

        std::string sProto = curServNode["proto"].asString();
        MyTpProto_t proto;
        if      ("udp" == sProto)   { proto = MyTpProto_t::UDP; }
        else if ("tcp" == sProto)   { proto = MyTpProto_t::TCP; }
        else                        { proto = MyTpProto_t::UDP; }
        
        MySipServAuthCfg_dt lowServAuthCfg;
        if ("true" == curServNode["auth"].asString()) {
            lowServAuthCfg.enableAuth  = true;
            lowServAuthCfg.authName    = curServNode["authName"].asString();
            lowServAuthCfg.authPwd     = curServNode["authPwd"].asString();
            lowServAuthCfg.authRealm   = curServNode["authRealm"].asString();
        }
        else {
            lowServAuthCfg.enableAuth  = false;
            lowServAuthCfg.authName    = "";
            lowServAuthCfg.authPwd     = "";
            lowServAuthCfg.authRealm   = "";
        }

        MySipRegLowServCfg_dt regLowServInfo;
        regLowServInfo.lowSipServAddrCfg  = std::move(lowServAddrCfg);
        regLowServInfo.lowSipServAuthCfg  = std::move(lowServAuthCfg);
        regLowServInfo.expired            = expired;
        regLowServInfo.proto              = proto;
        regLowServMap.insert(std::make_pair(regLowServInfo.lowSipServAddrCfg.id, std::move(regLowServInfo)));
    }

    ifs.close();
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_UTILS