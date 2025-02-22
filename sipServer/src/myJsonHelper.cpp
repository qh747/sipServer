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

    int size = rootNode.size();
    for (unsigned int i = 1; i <= rootNode.size(); ++i) {
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

    for (unsigned int i = 1; i <= rootNode.size(); ++i) {
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

MyStatus_t MyJsonHelper::ParseSipServCatalogJsonFile(const std::string&                    filePath, 
                                                     MySipServCatalogPlatCfgMap&           platCfgMap,
                                                     MySipServCatalogSubPlatCfgMap&        subPlatCfgMap,
                                                     MySipServCatalogSubVirtualPlatCfgMap& subVirtualPlatCfgMap,
                                                     MySipServCatalogDeviceCfgMap&         deviceCfgMap)
{
    if (filePath.empty()) {
        return MyStatus_t::FAILED;
    }

    // 读取 json 文件
    std::ifstream ifs(filePath.c_str());
    Json::Value rootNode;
    ifs >> rootNode;

    for (unsigned int i = 1; i <= rootNode.size(); ++i) {
        // 解析json根节点
        std::string sRootNodeName = std::string("catalog-") + std::to_string(i);
        if (!(rootNode.isMember(sRootNodeName.c_str()))) {
            break;
        }

        const Json::Value& catalogNode = rootNode[sRootNodeName.c_str()];
        // 解析本级服务id
        if (!(catalogNode.isMember("id"))) {
            continue;
        }
        std::string localSevrId = catalogNode["id"].asString();

        // 解析平台信息
        for (unsigned int j = 1; j <= catalogNode["platInfo"].size(); ++j) {
            std::string sPlatNodeName = std::string("plat-") + std::to_string(j);
            if (!(catalogNode["platInfo"].isMember(sPlatNodeName.c_str()))) {
                continue;
            }
        
            const Json::Value& curPlatNode = catalogNode["platInfo"][sPlatNodeName.c_str()];

            MySipCatalogPlatCfg_dt cfg;
            cfg.name          = curPlatNode["name"].asString();
            cfg.status        = curPlatNode["status"].asString();
            cfg.parental      = curPlatNode["parental"].asInt();
            cfg.manufacturer  = curPlatNode["manufacturer"].asString();
            cfg.model         = curPlatNode["model"].asString();
            cfg.block         = curPlatNode["block"].asString();
            cfg.safetyWay     = curPlatNode["safetyWay"].asInt();
            cfg.registerWay   = curPlatNode["registerWay"].asInt();
            cfg.secrecy       = curPlatNode["secrecy"].asInt();
            cfg.deviceID      = curPlatNode["deviceID"].asString();
            cfg.parentID      = curPlatNode["parentID"].asString();
            cfg.platformID    = curPlatNode["platformID"].asString();
            cfg.platformIp    = curPlatNode["platformIp"].asString();
            cfg.platformPort  = curPlatNode["platformPort"].asInt();
            cfg.owner         = curPlatNode["owner"].asString();
            cfg.deviceIp      = curPlatNode["deviceIp"].asString();
            cfg.devicePort    = curPlatNode["devicePort"].asInt();
            cfg.deviceChannel = curPlatNode["deviceChannel"].asInt();
            cfg.deviceStream  = curPlatNode["deviceStream"].asString();
            cfg.longitude     = curPlatNode["longitude"].asString();
            cfg.latitude      = curPlatNode["latitude"].asString();

            auto iter = platCfgMap.find(localSevrId);
            if (platCfgMap.end() != iter) {
                if (iter->second.end() == iter->second.find(cfg.deviceID)) {
                    iter->second.insert(std::make_pair(cfg.deviceID, cfg));
                }
            }
            else {
                MySipCatalogPlatCfgMap catalogPlatCfgmap;
                catalogPlatCfgmap.insert(std::make_pair(cfg.deviceID, cfg));

                platCfgMap.insert(std::make_pair(localSevrId, catalogPlatCfgmap));
            }
        }

        // 解析子平台信息
        for (unsigned int j = 1; j <= catalogNode["subPlatInfo"].size(); ++j) {
            std::string sSubPlatNodeName = std::string("subPlat-") + std::to_string(j);
            if (!(catalogNode["subPlatInfo"].isMember(sSubPlatNodeName.c_str()))) {
                continue;
            }
        
            const Json::Value& curSubPlatNode = catalogNode["subPlatInfo"][sSubPlatNodeName.c_str()];

            MySipCatalogSubPlatCfg_dt cfg;
            cfg.name          = curSubPlatNode["name"].asString();
            cfg.status        = curSubPlatNode["status"].asString();
            cfg.parental      = curSubPlatNode["parental"].asInt();
            cfg.manufacturer  = curSubPlatNode["manufacturer"].asString();
            cfg.model         = curSubPlatNode["model"].asString();
            cfg.block         = curSubPlatNode["block"].asString();
            cfg.safetyWay     = curSubPlatNode["safetyWay"].asInt();
            cfg.registerWay   = curSubPlatNode["registerWay"].asInt();
            cfg.secrecy       = curSubPlatNode["secrecy"].asInt();
            cfg.deviceID      = curSubPlatNode["deviceID"].asString();
            cfg.parentID      = curSubPlatNode["parentID"].asString();
            cfg.platformID    = curSubPlatNode["platformID"].asString();
            cfg.platformIp    = curSubPlatNode["platformIp"].asString();
            cfg.platformPort  = curSubPlatNode["platformPort"].asInt();
            cfg.owner         = curSubPlatNode["owner"].asString();
            cfg.deviceIp      = curSubPlatNode["deviceIp"].asString();
            cfg.devicePort    = curSubPlatNode["devicePort"].asInt();
            cfg.deviceChannel = curSubPlatNode["deviceChannel"].asInt();
            cfg.deviceStream  = curSubPlatNode["deviceStream"].asString();
            cfg.longitude     = curSubPlatNode["longitude"].asString();
            cfg.latitude      = curSubPlatNode["latitude"].asString();

            auto iter = subPlatCfgMap.find(localSevrId);
            if (subPlatCfgMap.end() != iter) {
                if (iter->second.end() == iter->second.find(cfg.deviceID)) {
                    iter->second.insert(std::make_pair(cfg.deviceID, cfg));
                }
            }
            else {
                MySipCatalogSubPlatCfgMap catalogSubPlatCfgmap;
                catalogSubPlatCfgmap.insert(std::make_pair(cfg.deviceID, cfg));

                subPlatCfgMap.insert(std::make_pair(localSevrId, catalogSubPlatCfgmap));
            }
        }

        // 解析虚拟子平台信息
        for (unsigned int j = 1; j <= catalogNode["subVirtualPlatInfo"].size(); ++j) {
            std::string sSubVirtualPlatNodeName = std::string("subVirtualPlat-") + std::to_string(j);
            if (!(catalogNode["subVirtualPlatInfo"].isMember(sSubVirtualPlatNodeName.c_str()))) {
                continue;
            }
        
            const Json::Value& curSubVirtualPlatNode = catalogNode["subVirtualPlatInfo"][sSubVirtualPlatNodeName.c_str()];

            MySipCatalogVirtualSubPlatCfg_dt cfg;
            cfg.name          = curSubVirtualPlatNode["name"].asString();
            cfg.status        = curSubVirtualPlatNode["status"].asString();
            cfg.parental      = curSubVirtualPlatNode["parental"].asInt();
            cfg.manufacturer  = curSubVirtualPlatNode["manufacturer"].asString();
            cfg.model         = curSubVirtualPlatNode["model"].asString();
            cfg.block         = curSubVirtualPlatNode["block"].asString();
            cfg.safetyWay     = curSubVirtualPlatNode["safetyWay"].asInt();
            cfg.registerWay   = curSubVirtualPlatNode["registerWay"].asInt();
            cfg.secrecy       = curSubVirtualPlatNode["secrecy"].asInt();
            cfg.deviceID      = curSubVirtualPlatNode["deviceID"].asString();
            cfg.parentID      = curSubVirtualPlatNode["parentID"].asString();
            cfg.platformID    = curSubVirtualPlatNode["platformID"].asString();
            cfg.platformIp    = curSubVirtualPlatNode["platformIp"].asString();
            cfg.platformPort  = curSubVirtualPlatNode["platformPort"].asInt();
            cfg.owner         = curSubVirtualPlatNode["owner"].asString();
            cfg.deviceIp      = curSubVirtualPlatNode["deviceIp"].asString();
            cfg.devicePort    = curSubVirtualPlatNode["devicePort"].asInt();
            cfg.deviceChannel = curSubVirtualPlatNode["deviceChannel"].asInt();
            cfg.deviceStream  = curSubVirtualPlatNode["deviceStream"].asString();
            cfg.longitude     = curSubVirtualPlatNode["longitude"].asString();
            cfg.latitude      = curSubVirtualPlatNode["latitude"].asString();

            if (curSubVirtualPlatNode.isMember("auth")) {
                const Json::Value& authArray = curSubVirtualPlatNode["auth"];
                if (authArray.isArray()) {
                    for (const auto& authItem : authArray) {
                        if (authItem.isMember("portNum")) {
                            MySipCatalogVirtualSubPlatAuthCfg_dt authCfg;
                            authCfg.portNum = authItem["portNum"].asString();

                            cfg.authVec.emplace_back(authCfg);
                        }
                    }
                }
            }

            auto iter = subVirtualPlatCfgMap.find(localSevrId);
            if (subVirtualPlatCfgMap.end() != iter) {
                if (iter->second.end() == iter->second.find(cfg.deviceID)) {
                    iter->second.insert(std::make_pair(cfg.deviceID, cfg));
                }
            }
            else {
                MySipCatalogSubVirtualPlatCfgMap catalogSubVirtualPlatCfgmap;
                catalogSubVirtualPlatCfgmap.insert(std::make_pair(cfg.deviceID, cfg));

                subVirtualPlatCfgMap.insert(std::make_pair(localSevrId, catalogSubVirtualPlatCfgmap));
            }
        }

        // 解析设备信息
        for (unsigned int j = 1; j <= catalogNode["deviceInfo"].size(); ++j) {
            std::string sDeviceNodeName = std::string("device-") + std::to_string(j);
            if (!(catalogNode["deviceInfo"].isMember(sDeviceNodeName.c_str()))) {
                continue;
            }
        
            const Json::Value& curDeviceNode = catalogNode["deviceInfo"][sDeviceNodeName.c_str()];

            MySipCatalogDeviceCfg_dt cfg;
            cfg.name          = curDeviceNode["name"].asString();
            cfg.status        = curDeviceNode["status"].asString();
            cfg.parental      = curDeviceNode["parental"].asInt();
            cfg.manufacturer  = curDeviceNode["manufacturer"].asString();
            cfg.model         = curDeviceNode["model"].asString();
            cfg.block         = curDeviceNode["block"].asString();
            cfg.safetyWay     = curDeviceNode["safetyWay"].asInt();
            cfg.registerWay   = curDeviceNode["registerWay"].asInt();
            cfg.secrecy       = curDeviceNode["secrecy"].asInt();
            cfg.deviceID      = curDeviceNode["deviceID"].asString();
            cfg.parentID      = curDeviceNode["parentID"].asString();
            cfg.platformID    = curDeviceNode["platformID"].asString();
            cfg.owner         = curDeviceNode["owner"].asString();
            cfg.deviceIp      = curDeviceNode["deviceIp"].asString();
            cfg.devicePort    = curDeviceNode["devicePort"].asInt();
            cfg.deviceChannel = curDeviceNode["deviceChannel"].asInt();
            cfg.deviceStream  = curDeviceNode["deviceStream"].asString();
            cfg.longitude     = curDeviceNode["longitude"].asString();
            cfg.latitude      = curDeviceNode["latitude"].asString();

            auto iter = deviceCfgMap.find(localSevrId);
            if (deviceCfgMap.end() != iter) {
                if (iter->second.end() == iter->second.find(cfg.deviceID)) {
                    iter->second.insert(std::make_pair(cfg.deviceID, cfg));
                }
            }
            else {
                MySipCatalogDeviceCfgMap catalogDeviceCfgmap;
                catalogDeviceCfgmap.insert(std::make_pair(cfg.deviceID, cfg));

                deviceCfgMap.insert(std::make_pair(localSevrId, catalogDeviceCfgmap));
            }
        }
    }
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_UTILS