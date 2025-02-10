#include <map>
#include <fstream>
#include <cstdint>
#include <cstdbool>
#include <json/json.h>
#include "utils/myJsonHelper.h"
using namespace MY_COMMON;

namespace MY_UTILS {

MyStatus_t MyJsonHelper::ParseSipServAddrJsonFile(const std::string& filePath, MySipServAddrMap& cfgMap)
{
    if (filePath.empty()) {
        return MyStatus_t::FAILED;
    }

    // 读取 json 文件
    std::ifstream ifs(filePath.c_str());
    Json::Value rootNode;
    ifs >> rootNode;

    for (unsigned int i = 1; i < rootNode.size(); ++i) {
        // 解析json根节点
        std::string sRootNodeName = std::string("sipServer-") + std::to_string(i);
        if (!(rootNode.isMember(sRootNodeName.c_str()))) {
            break;
        }

        const Json::Value& sipServNode = rootNode[sRootNodeName.c_str()];
        // 解析本级服务id
        if (!(sipServNode.isMember("id"))) {
            continue;
        }

        MySipServAddrCfg_dt cfg;
        cfg.id      = sipServNode["id"].asString();
        cfg.ipAddr  = sipServNode["ipAddr"].asString();
        cfg.port    = static_cast<uint16_t>(std::stoi(sipServNode["port"].asString()));
        cfg.regPort = static_cast<uint16_t>(std::stoi(sipServNode["regPort"].asString()));
        cfg.name    = sipServNode["name"].asString();
        cfg.domain  = sipServNode["domain"].asString();

        if (!cfg.id.empty() && (cfgMap.end() == cfgMap.find(cfg.id))) {
            cfgMap.insert(std::make_pair(cfg.id, std::move(cfg)));
        }
    }

    ifs.close();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyJsonHelper::ParseSipServRegJsonFile(const std::string& filePath, MySipRegServCfgMap& cfgMap)
{
    if (filePath.empty()) {
        return MyStatus_t::FAILED;
    }

    // 读取 json 文件
    std::ifstream ifs(filePath.c_str());
    Json::Value rootNode;
    ifs >> rootNode;

    for (unsigned int i = 1; i < rootNode.size(); ++i) {
        // 解析json根节点
        std::string sRootNodeName = std::string("registerServer-") + std::to_string(i);
        if (!(rootNode.isMember(sRootNodeName.c_str()))) {
            break;
        }

        const Json::Value& regServNode = rootNode[sRootNodeName.c_str()];
        // 解析本级服务id
        if (!(regServNode.isMember("id"))) {
            continue;
        }

        MySipRegServCfg_dt cfg;
        cfg.localServId = regServNode["id"].asString();

        // 解析上级注册服务
        if ((regServNode.isMember("upperServer"))) {
            const Json::Value& upServNodes = regServNode["upperServer"];
            for (const auto& curServNode : upServNodes) {
                MySipServAddrCfg_dt upServAddrCfg;
                upServAddrCfg.id = curServNode["id"].asString();

                if (cfg.upRegSipServMap.end() != cfg.upRegSipServMap.find(upServAddrCfg.id)) {
                    continue;
                }

                upServAddrCfg.ipAddr = curServNode["ipAddr"].asString();
                upServAddrCfg.port   = static_cast<uint16_t>(std::stoi(curServNode["port"].asString()));
                upServAddrCfg.name   = curServNode["name"].asString();
                upServAddrCfg.domain = curServNode["domain"].asString();

                if (upServAddrCfg.id.empty()   || upServAddrCfg.ipAddr.empty() || (0 == upServAddrCfg.port) || 
                    upServAddrCfg.name.empty() || upServAddrCfg.domain.empty()) {
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
                upRegServCfg.proto              = proto;

                if (!upRegServCfg.upSipServAddrCfg.id.empty() && 
                    (cfg.upRegSipServMap.end() == cfg.upRegSipServMap.find(upRegServCfg.upSipServAddrCfg.id))) {
                    cfg.upRegSipServMap.insert(std::make_pair(upRegServCfg.upSipServAddrCfg.id, std::move(upRegServCfg)));
                }
            }
        }

        // 解析下级注册服务
        if ((regServNode.isMember("lowerServer"))) {
            const Json::Value& lowServNodes = regServNode["lowerServer"];
            for (const auto& curServNode : lowServNodes) {
                MySipServAddrCfg_dt lowServAddrCfg;
                lowServAddrCfg.id = curServNode["id"].asString();
            
                if (cfg.lowRegSipServMap.end() != cfg.lowRegSipServMap.find(lowServAddrCfg.id)) {
                    continue;
                }
            
                lowServAddrCfg.ipAddr = curServNode["ipAddr"].asString();
                lowServAddrCfg.port   = static_cast<uint16_t>(std::stoi(curServNode["port"].asString()));
                lowServAddrCfg.name   = curServNode["name"].asString();
                lowServAddrCfg.domain = curServNode["domain"].asString();
            
                if (lowServAddrCfg.id.empty()   || lowServAddrCfg.ipAddr.empty() || (0 == lowServAddrCfg.port) || 
                    lowServAddrCfg.name.empty() || lowServAddrCfg.domain.empty()) {
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
                regLowServInfo.proto              = proto;

                if (!regLowServInfo.lowSipServAddrCfg.id.empty() && 
                    (cfg.lowRegSipServMap.end() == cfg.lowRegSipServMap.find(regLowServInfo.lowSipServAddrCfg.id))) {
                    cfg.lowRegSipServMap.insert(std::make_pair(regLowServInfo.lowSipServAddrCfg.id, std::move(regLowServInfo)));
                }
            }
        }
        
        if (!cfg.localServId.empty() && (cfgMap.end() == cfgMap.find(cfg.localServId))) {
            cfgMap.insert(std::make_pair(cfg.localServId, std::move(cfg)));
        }
    }

    ifs.close();
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_UTILS