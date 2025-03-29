#include <map>
#include <fstream>
#include <json/json.h>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "utils/myJsonHelper.h"
using namespace MY_COMMON;

namespace MY_UTILS {

MyStatus_t MyJsonHelper::ParseSipRegJsonFile(const std::string& filePath, MySipRegServCfg_dt& regCfg)
{
    if (filePath.empty()) {
        return MyStatus_t::FAILED;
    }

    // 读取 json 文件
    std::ifstream ifs(filePath.c_str());
    if (!ifs.is_open()) {
        LOG(ERROR) << "open json file failed: " << filePath;
        return MyStatus_t::FAILED;
    }

    Json::Value rootNode;
    ifs >> rootNode;

    // 解析上级注册服务根节点
    if (rootNode.isMember("upperServer")) {
        const Json::Value& regUpServNode = rootNode["upperServer"];
        for (const auto& curServNode : regUpServNode) {
            MySipRegUpServCfg_dt upRegServCfg;

            upRegServCfg.upSipServRegAddrCfg.id = curServNode["id"].asString();
            if (upRegServCfg.upSipServRegAddrCfg.id.empty() || (regCfg.upRegSipServMap.end() != regCfg.upRegSipServMap.find(upRegServCfg.upSipServRegAddrCfg.id))) {
                continue;
            }

            upRegServCfg.upSipServRegAddrCfg.ipAddr  = curServNode["ipAddr"].asString();
            upRegServCfg.upSipServRegAddrCfg.port    = static_cast<uint16_t>(std::stoi(curServNode["port"].asString()));
            upRegServCfg.upSipServRegAddrCfg.name    = curServNode["name"].asString();
            upRegServCfg.upSipServRegAddrCfg.domain  = curServNode["domain"].asString();

            if (upRegServCfg.upSipServRegAddrCfg.id.empty()   || upRegServCfg.upSipServRegAddrCfg.ipAddr.empty() || (0 == upRegServCfg.upSipServRegAddrCfg.port) || 
                upRegServCfg.upSipServRegAddrCfg.name.empty() || upRegServCfg.upSipServRegAddrCfg.domain.empty()) {
                continue;
            }

            if ("true" == curServNode["auth"].asString()) {
                upRegServCfg.upSipServRegAuthCfg.enableAuth  = true;
                upRegServCfg.upSipServRegAuthCfg.authName    = curServNode["authName"].asString();
                upRegServCfg.upSipServRegAuthCfg.authPwd     = curServNode["authPwd"].asString();
                upRegServCfg.upSipServRegAuthCfg.authRealm   = curServNode["authRealm"].asString();
            }
            else {
                upRegServCfg.upSipServRegAuthCfg.enableAuth  = false;
                upRegServCfg.upSipServRegAuthCfg.authName    = "";
                upRegServCfg.upSipServRegAuthCfg.authPwd     = "";
                upRegServCfg.upSipServRegAuthCfg.authRealm   = "";
            }

            std::string sProto = curServNode["proto"].asString();
            if ("tcp" == sProto) { upRegServCfg.proto = MyTpProto_t::TCP; }
            else                 { upRegServCfg.proto = MyTpProto_t::UDP; }

            regCfg.upRegSipServMap.insert(std::make_pair(upRegServCfg.upSipServRegAddrCfg.id, std::move(upRegServCfg)));
        }
    }

    // 解析下级注册服务
    if (rootNode.isMember("lowerServer")) {
        const Json::Value& regLowServNode = rootNode["lowerServer"];
        for (const auto& curServNode : regLowServNode) {
            MySipRegLowServCfg_dt regLowServCfg;

            regLowServCfg.lowSipServRegAddrCfg.id = curServNode["id"].asString();
            if (regLowServCfg.lowSipServRegAddrCfg.id.empty() || (regCfg.lowRegSipServMap.end() != regCfg.lowRegSipServMap.find(regLowServCfg.lowSipServRegAddrCfg.id))) {
                continue;
            }
        
            regLowServCfg.lowSipServRegAddrCfg.ipAddr  = curServNode["ipAddr"].asString();
            regLowServCfg.lowSipServRegAddrCfg.port    = static_cast<uint16_t>(std::stoi(curServNode["port"].asString()));
            regLowServCfg.lowSipServRegAddrCfg.name    = curServNode["name"].asString();
            regLowServCfg.lowSipServRegAddrCfg.domain  = curServNode["domain"].asString();
        
            if (regLowServCfg.lowSipServRegAddrCfg.id.empty()   || regLowServCfg.lowSipServRegAddrCfg.ipAddr.empty() || (0 == regLowServCfg.lowSipServRegAddrCfg.port) || 
                regLowServCfg.lowSipServRegAddrCfg.name.empty() || regLowServCfg.lowSipServRegAddrCfg.domain.empty()) {
                continue;
            }

            if ("true" == curServNode["auth"].asString()) {
                regLowServCfg.lowSipServRegAuthCfg.enableAuth  = true;
                regLowServCfg.lowSipServRegAuthCfg.authName    = curServNode["authName"].asString();
                regLowServCfg.lowSipServRegAuthCfg.authPwd     = curServNode["authPwd"].asString();
                regLowServCfg.lowSipServRegAuthCfg.authRealm   = curServNode["authRealm"].asString();
            }
            else {
                regLowServCfg.lowSipServRegAuthCfg.enableAuth  = false;
                regLowServCfg.lowSipServRegAuthCfg.authName    = "";
                regLowServCfg.lowSipServRegAuthCfg.authPwd     = "";
                regLowServCfg.lowSipServRegAuthCfg.authRealm   = "";
            }
        
            std::string sProto = curServNode["proto"].asString();
            if ("tcp" == sProto)   { regLowServCfg.proto = MyTpProto_t::TCP; }
            else                   { regLowServCfg.proto = MyTpProto_t::UDP; }

            regCfg.lowRegSipServMap.insert(std::make_pair(regLowServCfg.lowSipServRegAddrCfg.id, std::move(regLowServCfg)));
        }
    }

    ifs.close();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyJsonHelper::ParseSipCatalogJsonFile(const std::string& filePath, MySipCatalogCfg_dt& catalogCfg)
{
    if (filePath.empty()) {
        return MyStatus_t::FAILED;
    }

    // 读取 json 文件
    std::ifstream ifs(filePath.c_str());
    if (!ifs.is_open()) {
        LOG(ERROR) << "open json file failed: " << filePath;
        return MyStatus_t::FAILED;
    }

    Json::Value rootNode;
    ifs >> rootNode;

    // 解析平台配置节点
    if (rootNode.isMember("platInfo")) {
        const Json::Value& platNode             = rootNode["platInfo"];
        catalogCfg.catalogPlatCfg.name          = platNode["name"].asString();
        catalogCfg.catalogPlatCfg.status        = platNode["status"].asString();
        catalogCfg.catalogPlatCfg.parental      = platNode["parental"].asString();
        catalogCfg.catalogPlatCfg.manufacturer  = platNode["manufacturer"].asString();
        catalogCfg.catalogPlatCfg.model         = platNode["model"].asString();
        catalogCfg.catalogPlatCfg.block         = platNode["block"].asString();
        catalogCfg.catalogPlatCfg.safetyWay     = platNode["safetyWay"].asString();
        catalogCfg.catalogPlatCfg.registerWay   = platNode["registerWay"].asString();
        catalogCfg.catalogPlatCfg.secrecy       = platNode["secrecy"].asString();
        catalogCfg.catalogPlatCfg.deviceID      = platNode["deviceID"].asString();
        catalogCfg.catalogPlatCfg.parentID      = platNode["parentID"].asString();
        catalogCfg.catalogPlatCfg.platformID    = platNode["platformID"].asString();
        catalogCfg.catalogPlatCfg.platformIp    = platNode["platformIp"].asString();
        catalogCfg.catalogPlatCfg.platformPort  = platNode["platformPort"].asString();
        catalogCfg.catalogPlatCfg.owner         = platNode["owner"].asString();
        catalogCfg.catalogPlatCfg.deviceIp      = platNode["deviceIp"].asString();
        catalogCfg.catalogPlatCfg.devicePort    = platNode["devicePort"].asString();
        catalogCfg.catalogPlatCfg.deviceChannel = platNode["deviceChannel"].asString();
        catalogCfg.catalogPlatCfg.deviceStream  = platNode["deviceStream"].asString();
        catalogCfg.catalogPlatCfg.longitude     = platNode["longitude"].asString();
        catalogCfg.catalogPlatCfg.latitude      = platNode["latitude"].asString();
    }
    else {
        return MyStatus_t::FAILED;
    }

    // 解析子平台配置节点
    if (rootNode.isMember("subPlatInfo")) {
        const Json::Value& subPlatNode = rootNode["subPlatInfo"];

        for (unsigned int j = 1; j <= subPlatNode.size(); ++j) {
            std::string sSubPlatNodeName = std::string("subPlat-") + std::to_string(j);
            if (!(subPlatNode.isMember(sSubPlatNodeName.c_str()))) {
                continue;
            }
        
            const Json::Value& curSubPlatNode = subPlatNode[sSubPlatNodeName.c_str()];

            MySipCatalogSubPlatCfg_dt cfg;
            cfg.name          = curSubPlatNode["name"].asString();
            cfg.status        = curSubPlatNode["status"].asString();
            cfg.parental      = curSubPlatNode["parental"].asString();
            cfg.manufacturer  = curSubPlatNode["manufacturer"].asString();
            cfg.model         = curSubPlatNode["model"].asString();
            cfg.block         = curSubPlatNode["block"].asString();
            cfg.safetyWay     = curSubPlatNode["safetyWay"].asString();
            cfg.registerWay   = curSubPlatNode["registerWay"].asString();
            cfg.secrecy       = curSubPlatNode["secrecy"].asString();
            cfg.deviceID      = curSubPlatNode["deviceID"].asString();
            cfg.parentID      = curSubPlatNode["parentID"].asString();
            cfg.platformID    = curSubPlatNode["platformID"].asString();
            cfg.platformIp    = curSubPlatNode["platformIp"].asString();
            cfg.platformPort  = curSubPlatNode["platformPort"].asString();
            cfg.owner         = curSubPlatNode["owner"].asString();
            cfg.deviceIp      = curSubPlatNode["deviceIp"].asString();
            cfg.devicePort    = curSubPlatNode["devicePort"].asString();
            cfg.deviceChannel = curSubPlatNode["deviceChannel"].asString();
            cfg.deviceStream  = curSubPlatNode["deviceStream"].asString();
            cfg.longitude     = curSubPlatNode["longitude"].asString();
            cfg.latitude      = curSubPlatNode["latitude"].asString();

            if (catalogCfg.catalogSubPlatCfgMap.end() != catalogCfg.catalogSubPlatCfgMap.find(cfg.deviceID)) {
                return MyStatus_t::FAILED;
            }

            const auto& platCfg = catalogCfg.catalogPlatCfg;
            const auto& subPlatCfgMap = catalogCfg.catalogSubPlatCfgMap;

            if (platCfg.deviceID == cfg.parentID && platCfg.deviceID == cfg.platformID) {
                catalogCfg.catalogSubPlatCfgMap.insert(std::make_pair(cfg.deviceID, cfg));
            }
            else {
                return MyStatus_t::FAILED;
            }
        }
    }

    // 解析虚拟子平台配置节点
    if (rootNode.isMember("subVirtualPlatInfo")) {
        const Json::Value& subVirtualPlatNode = rootNode["subVirtualPlatInfo"];

        for (unsigned int j = 1; j <= subVirtualPlatNode.size(); ++j) {
            std::string subVirtualPlatNodeName = std::string("subVirtualPlat-") + std::to_string(j);
            if (!(subVirtualPlatNode.isMember(subVirtualPlatNodeName.c_str()))) {
                continue;
            }
        
            const Json::Value& curSubVirtualPlatNode = subVirtualPlatNode[subVirtualPlatNodeName.c_str()];

            MySipCatalogVirtualSubPlatCfg_dt cfg;
            cfg.name          = curSubVirtualPlatNode["name"].asString();
            cfg.status        = curSubVirtualPlatNode["status"].asString();
            cfg.parental      = curSubVirtualPlatNode["parental"].asString();
            cfg.manufacturer  = curSubVirtualPlatNode["manufacturer"].asString();
            cfg.model         = curSubVirtualPlatNode["model"].asString();
            cfg.block         = curSubVirtualPlatNode["block"].asString();
            cfg.safetyWay     = curSubVirtualPlatNode["safetyWay"].asString();
            cfg.registerWay   = curSubVirtualPlatNode["registerWay"].asString();
            cfg.secrecy       = curSubVirtualPlatNode["secrecy"].asString();
            cfg.deviceID      = curSubVirtualPlatNode["deviceID"].asString();
            cfg.parentID      = curSubVirtualPlatNode["parentID"].asString();
            cfg.platformID    = curSubVirtualPlatNode["platformID"].asString();
            cfg.platformIp    = curSubVirtualPlatNode["platformIp"].asString();
            cfg.platformPort  = curSubVirtualPlatNode["platformPort"].asString();
            cfg.owner         = curSubVirtualPlatNode["owner"].asString();
            cfg.deviceIp      = curSubVirtualPlatNode["deviceIp"].asString();
            cfg.devicePort    = curSubVirtualPlatNode["devicePort"].asString();
            cfg.deviceChannel = curSubVirtualPlatNode["deviceChannel"].asString();
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

            if (catalogCfg.catalogVirtualSubPlatCfgMap.end() != catalogCfg.catalogVirtualSubPlatCfgMap.find(cfg.deviceID)) {
                return MyStatus_t::FAILED;
            }

            const auto& platCfg = catalogCfg.catalogPlatCfg;
            const auto& subPlatCfgMap = catalogCfg.catalogSubPlatCfgMap;

            if ((platCfg.deviceID == cfg.parentID || subPlatCfgMap.end() != subPlatCfgMap.find(cfg.parentID)) && (platCfg.deviceID == cfg.platformID)) {
                catalogCfg.catalogVirtualSubPlatCfgMap.insert(std::make_pair(cfg.deviceID, cfg));
            }
            else {
                return MyStatus_t::FAILED;
            }
        }
    }

    // 解析设备配置节点
    if (rootNode.isMember("deviceInfo")) {
        const Json::Value& deviceNode = rootNode["deviceInfo"];
    
        for (unsigned int j = 1; j <= deviceNode.size(); ++j) {
            std::string deviceNodeName = std::string("device-") + std::to_string(j);
            if (!(deviceNode.isMember(deviceNodeName.c_str()))) {
                continue;
            }
        
            const Json::Value& curDeviceNode = deviceNode[deviceNodeName.c_str()];

            MySipCatalogDeviceCfg_dt cfg;
            cfg.name          = curDeviceNode["name"].asString();
            cfg.status        = curDeviceNode["status"].asString();
            cfg.parental      = curDeviceNode["parental"].asString();
            cfg.manufacturer  = curDeviceNode["manufacturer"].asString();
            cfg.model         = curDeviceNode["model"].asString();
            cfg.block         = curDeviceNode["block"].asString();
            cfg.safetyWay     = curDeviceNode["safetyWay"].asString();
            cfg.registerWay   = curDeviceNode["registerWay"].asString();
            cfg.secrecy       = curDeviceNode["secrecy"].asString();
            cfg.deviceID      = curDeviceNode["deviceID"].asString();
            cfg.parentID      = curDeviceNode["parentID"].asString();
            cfg.platformID    = curDeviceNode["platformID"].asString();
            cfg.owner         = curDeviceNode["owner"].asString();
            cfg.deviceIp      = curDeviceNode["deviceIp"].asString();
            cfg.devicePort    = curDeviceNode["devicePort"].asString();
            cfg.deviceChannel = curDeviceNode["deviceChannel"].asString();
            cfg.deviceStream  = curDeviceNode["deviceStream"].asString();
            cfg.longitude     = curDeviceNode["longitude"].asString();
            cfg.latitude      = curDeviceNode["latitude"].asString();

            if (catalogCfg.catalogDeviceCfgMap.end() != catalogCfg.catalogDeviceCfgMap.find(cfg.deviceID)) {
                return MyStatus_t::FAILED;
            }

            const auto& platCfg = catalogCfg.catalogPlatCfg;
            const auto& subPlatCfgMap = catalogCfg.catalogSubPlatCfgMap;
            const auto& virtualSubPlatCfgMap = catalogCfg.catalogVirtualSubPlatCfgMap;

            if ((platCfg.deviceID == cfg.parentID || 
                subPlatCfgMap.end() != subPlatCfgMap.find(cfg.parentID) || 
                virtualSubPlatCfgMap.end() != virtualSubPlatCfgMap.find(cfg.parentID)) &&
                (platCfg.deviceID == cfg.platformID)) {
                catalogCfg.catalogDeviceCfgMap.insert(std::make_pair(cfg.deviceID, cfg));
            }
            else {
                return MyStatus_t::FAILED;
            }
        }
    }
    
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyJsonHelper::ParseHttpReqMediaInfo(const std::string& buf, MyHttpReqMediaInfo_dt& reqInfo)
{
    Json::Value rootNode;
    Json::CharReaderBuilder reader;
    std::string errs;

    // 解析字符串
    std::istringstream bufStream(buf);
    if (!Json::parseFromStream(reader, bufStream, &rootNode, &errs)) {
        return MyStatus_t::FAILED;
    }

    // JSON数据校验
    if (!rootNode.isMember("deviceId") || !rootNode.isMember("playType") || !rootNode.isMember("protoType")) {
        return MyStatus_t::FAILED;
    }

    std::string playType = rootNode["playType"].asString();
    if ("play" != playType && "playback" != playType) {
        return MyStatus_t::FAILED;
    }

    std::string protoType = rootNode["protoType"].asString();
    if ("tcp" != protoType && "udp" != protoType) {
        return MyStatus_t::FAILED;
    }

    // 解析JSON数据
    reqInfo.deviceId  = rootNode["deviceId"].asString();
    reqInfo.playType  = ("play" == playType ? MyMedaPlayWay_t::PLAY : MyMedaPlayWay_t::PLAYBACK);
    reqInfo.protoType = ("udp" == protoType ? MyTpProto_t::UDP : MyTpProto_t::TCP);
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyJsonHelper::GenerateHttpErrMsgBody(const std::string& errInfo, std::string& msgBody)
{
    Json::Value root;
    root["errInfo"] = errInfo;

    Json::StreamWriterBuilder builder;
    msgBody = Json::writeString(builder, root);
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyJsonHelper::GenerateDeviceListInfo(const std::map<std::string, MySipCatalogInfo_dt>& deviceInfoMap, std::string& deviceInfoStr)
{
    Json::Value root;

    // json数据添加
    for (const auto& deviceInfoPair : deviceInfoMap) {
        for (const auto& platPair : deviceInfoPair.second.sipPlatMap) {
            Json::Value platNode;

            // 平台信息添加
            platNode["name"]       = platPair.second.name;
            platNode["deviceID"]   = platPair.second.deviceID;
            platNode["deviceIp"]   = platPair.second.deviceIp;
            platNode["devicePort"] = platPair.second.devicePort; 

            // 平台下挂载子平台信息添加
            if (!deviceInfoPair.second.sipSubPlatMap.empty()) {
                Json::Value subPlatArray;

                for (const auto& subPlatPair : deviceInfoPair.second.sipSubPlatMap) {
                    if (subPlatPair.second.parentID != platPair.first) {
                        continue;
                    }

                    Json::Value subPlatNode;
                    subPlatNode["name"]       = subPlatPair.second.name;
                    subPlatNode["deviceID"]   = subPlatPair.second.deviceID;
                    subPlatNode["deviceIp"]   = subPlatPair.second.deviceIp;
                    subPlatNode["devicePort"] = subPlatPair.second.devicePort; 

                    // 子平台下挂载虚拟子平台信息添加
                    if (!deviceInfoPair.second.sipSubVirtualPlatMap.empty()) {
                        Json::Value subVirtualPlatArray;

                        for (const auto& subVirtualPlatPair : deviceInfoPair.second.sipSubVirtualPlatMap) {
                            if (subVirtualPlatPair.second.parentID != subPlatPair.first) {
                                continue;
                            }

                            Json::Value subVirtualPlatNode;
                            subVirtualPlatNode["name"]       = subVirtualPlatPair.second.name;
                            subVirtualPlatNode["deviceID"]   = subVirtualPlatPair.second.deviceID;
                            subVirtualPlatNode["deviceIp"]   = subVirtualPlatPair.second.deviceIp;
                            subVirtualPlatNode["devicePort"] = subVirtualPlatPair.second.devicePort; 

                            // 虚拟子平台下挂载设备信息添加
                            if (!deviceInfoPair.second.sipDeviceMap.empty()) {
                                Json::Value deviceArray;

                                for (const auto& devicePair : deviceInfoPair.second.sipDeviceMap) {
                                    if (devicePair.second.parentID != subVirtualPlatPair.first) {
                                        continue;
                                    }
                                
                                    Json::Value deviceNode;
                                    deviceNode["name"]       = devicePair.second.name;
                                    deviceNode["deviceID"]   = devicePair.second.deviceID;
                                    deviceNode["deviceIp"]   = devicePair.second.deviceIp;
                                    deviceNode["devicePort"] = devicePair.second.devicePort; 
                                    deviceArray["subDevice_" + devicePair.first] = deviceNode;
                                }
                                
                                if (!deviceArray.empty()) {
                                    subVirtualPlatNode["subDevice"] = deviceArray;
                                }
                            }

                            if (!subVirtualPlatNode.empty()) {
                                subVirtualPlatArray["subVirtualPlatform" + subVirtualPlatPair.first] = subVirtualPlatNode;
                            }
                        }

                        if (!subVirtualPlatArray.empty()) {
                            subPlatNode["subVirtualPlatform"] = subVirtualPlatArray;
                        }
                    }

                    // 子平台下挂载设备信息添加
                    if (!deviceInfoPair.second.sipDeviceMap.empty()) {
                        Json::Value deviceArray;

                        for (const auto& devicePair : deviceInfoPair.second.sipDeviceMap) {
                            if (devicePair.second.parentID != subPlatPair.first) {
                                continue;
                            }
                            Json::Value deviceNode;
                            deviceNode["name"]       = devicePair.second.name;
                            deviceNode["deviceID"]   = devicePair.second.deviceID;
                            deviceNode["deviceIp"]   = devicePair.second.deviceIp;
                            deviceNode["devicePort"] = devicePair.second.devicePort; 
                            deviceArray["device" + devicePair.first] = deviceNode;
                        }

                        if (!deviceArray.empty()) {
                            subPlatNode["subDevice"] = deviceArray;
                        }
                    }

                    if (!subPlatNode.empty()) {
                        subPlatArray["subPlatform_" + subPlatPair.first] = subPlatNode;
                    }
                }

                if (!subPlatArray.empty()) {
                    platNode["subPlatform"] = subPlatArray;
                }
            }
            
            // 平台下挂载虚拟子平台信息添加
            if (!deviceInfoPair.second.sipSubVirtualPlatMap.empty()) {
                Json::Value subVirtualPlatArray;                
                for (const auto& subVirtualPlatPair : deviceInfoPair.second.sipSubVirtualPlatMap) {
                    if (subVirtualPlatPair.second.parentID != platPair.first) {
                        continue;
                    }

                    Json::Value subVirtualPlatNode;
                    subVirtualPlatNode["name"]       = subVirtualPlatPair.second.name;
                    subVirtualPlatNode["deviceID"]   = subVirtualPlatPair.second.deviceID;
                    subVirtualPlatNode["deviceIp"]   = subVirtualPlatPair.second.deviceIp;
                    subVirtualPlatNode["devicePort"] = subVirtualPlatPair.second.devicePort; 

                    // 虚拟子平台下挂载设备信息添加
                    if (!deviceInfoPair.second.sipDeviceMap.empty()) {
                        Json::Value deviceArray;
                        for (const auto& devicePair : deviceInfoPair.second.sipDeviceMap) {
                            if (devicePair.second.parentID != subVirtualPlatPair.first) {
                                continue;
                            }

                            Json::Value deviceNode;
                            deviceNode["name"]       = devicePair.second.name;
                            deviceNode["deviceID"]   = devicePair.second.deviceID;
                            deviceNode["deviceIp"]   = devicePair.second.deviceIp;
                            deviceNode["devicePort"] = devicePair.second.devicePort; 
                            deviceArray["subDevice_" + devicePair.first] = deviceNode;
                        }

                        if (!deviceArray.empty()) {
                            subVirtualPlatNode["subDevice"] = deviceArray;
                        } 
                    }

                    if (!subVirtualPlatNode.empty()) {
                        subVirtualPlatArray["subVirtualPlatform_" + subVirtualPlatPair.first] = subVirtualPlatNode;
                    }
                }

                if (!subVirtualPlatArray.empty()) {
                    platNode["subVirtualPlatform"] = subVirtualPlatArray;
                }
            }
            
            // 平台下挂载设备信息添加
            if (!deviceInfoPair.second.sipDeviceMap.empty()) {
                Json::Value deviceArray;

                for (const auto& devicePair : deviceInfoPair.second.sipDeviceMap) {
                    if (devicePair.second.parentID != platPair.first) {
                        continue;
                    }
    
                    Json::Value deviceNode;
                    deviceNode["name"]       = devicePair.second.name;
                    deviceNode["deviceID"]   = devicePair.second.deviceID;
                    deviceNode["deviceIp"]   = devicePair.second.deviceIp;
                    deviceNode["devicePort"] = devicePair.second.devicePort; 
                    deviceArray["subDevice_" + devicePair.first] = deviceNode;
                }
                
                if (!deviceArray.empty()) {
                    platNode["subDevice"] = deviceArray;
                }
            }
            
            if (!platNode.empty()) {
                root["platform_" + platPair.first] = platNode;
            }
        }
    }
    
    // 生成JSON字符串
    if (!root.empty()) {
        Json::StreamWriterBuilder builder;
        deviceInfoStr = Json::writeString(builder, root);

        return MyStatus_t::SUCCESS;
    }
    else {
        return MyStatus_t::FAILED;
    }
}

MyStatus_t MyJsonHelper::GenerateDeviceInfo(const MySipCatalogPlatCfg_dt& deviceInfo, std::string& deviceInfoStr)
{
    Json::Value root;

    // json数据添加
    Json::Value deviceNode;
    deviceNode["name"]          = deviceInfo.name;        
    deviceNode["status"]        = deviceInfo.status;
    deviceNode["parental"]      = deviceInfo.parental;         
    deviceNode["manufacturer"]  = deviceInfo.manufacturer;
    deviceNode["model"]         = deviceInfo.model;
    deviceNode["safetyWay"]     = deviceInfo.safetyWay;
    deviceNode["registerWay"]   = deviceInfo.registerWay;
    deviceNode["secrecy"]       = deviceInfo.secrecy;
    deviceNode["deviceID"]      = deviceInfo.deviceID;
    deviceNode["parentID"]      = deviceInfo.parentID;
    deviceNode["platformID"]    = deviceInfo.platformID;
    deviceNode["owner"]         = deviceInfo.owner;
    deviceNode["deviceIp"]      = deviceInfo.deviceIp;
    deviceNode["devicePort"]    = deviceInfo.devicePort;
    root["platformInfo"]           = deviceNode;

    // 生成JSON字符串
    Json::StreamWriterBuilder builder;
    deviceInfoStr = Json::writeString(builder, root);

    return MyStatus_t::SUCCESS;
}

MyStatus_t MyJsonHelper::GenerateDeviceInfo(const MySipCatalogSubPlatCfg_dt& deviceInfo, std::string& deviceInfoStr)
{
    Json::Value root;

    // json数据添加
    Json::Value deviceNode;
    deviceNode["name"]          = deviceInfo.name;        
    deviceNode["status"]        = deviceInfo.status;
    deviceNode["parental"]      = deviceInfo.parental;         
    deviceNode["manufacturer"]  = deviceInfo.manufacturer;
    deviceNode["model"]         = deviceInfo.model;
    deviceNode["safetyWay"]     = deviceInfo.safetyWay;
    deviceNode["registerWay"]   = deviceInfo.registerWay;
    deviceNode["secrecy"]       = deviceInfo.secrecy;
    deviceNode["deviceID"]      = deviceInfo.deviceID;
    deviceNode["parentID"]      = deviceInfo.parentID;
    deviceNode["platformID"]    = deviceInfo.platformID;
    deviceNode["owner"]         = deviceInfo.owner;
    deviceNode["deviceIp"]      = deviceInfo.deviceIp;
    deviceNode["devicePort"]    = deviceInfo.devicePort;
    root["subPlatformInfo"]        = deviceNode;

    // 生成JSON字符串
    Json::StreamWriterBuilder builder;
    deviceInfoStr = Json::writeString(builder, root);

    return MyStatus_t::SUCCESS;
}

MyStatus_t MyJsonHelper::GenerateDeviceInfo(const MySipCatalogVirtualSubPlatCfg_dt& deviceInfo, std::string& deviceInfoStr)
{
    Json::Value root;

    // json数据添加
    Json::Value deviceNode;
    deviceNode["name"]              = deviceInfo.name;        
    deviceNode["status"]            = deviceInfo.status;
    deviceNode["parental"]          = deviceInfo.parental;         
    deviceNode["manufacturer"]      = deviceInfo.manufacturer;
    deviceNode["model"]             = deviceInfo.model;
    deviceNode["safetyWay"]         = deviceInfo.safetyWay;
    deviceNode["registerWay"]       = deviceInfo.registerWay;
    deviceNode["secrecy"]           = deviceInfo.secrecy;
    deviceNode["deviceID"]          = deviceInfo.deviceID;
    deviceNode["parentID"]          = deviceInfo.parentID;
    deviceNode["platformID"]        = deviceInfo.platformID;
    deviceNode["owner"]             = deviceInfo.owner;
    deviceNode["deviceIp"]          = deviceInfo.deviceIp;
    deviceNode["devicePort"]        = deviceInfo.devicePort;
    root["subVirtualPlatformInfo"]     = deviceNode;

    // 生成JSON字符串
    Json::StreamWriterBuilder builder;
    deviceInfoStr = Json::writeString(builder, root);

    return MyStatus_t::SUCCESS;
}

MyStatus_t MyJsonHelper::GenerateDeviceInfo(const MySipCatalogDeviceCfg_dt& deviceInfo, std::string& deviceInfoStr)
{
    Json::Value root;

    // json数据添加
    Json::Value deviceNode;
    deviceNode["name"]          = deviceInfo.name;        
    deviceNode["status"]        = deviceInfo.status;
    deviceNode["parental"]      = deviceInfo.parental;         
    deviceNode["manufacturer"]  = deviceInfo.manufacturer;
    deviceNode["model"]         = deviceInfo.model;
    deviceNode["safetyWay"]     = deviceInfo.safetyWay;
    deviceNode["registerWay"]   = deviceInfo.registerWay;
    deviceNode["secrecy"]       = deviceInfo.secrecy;
    deviceNode["deviceID"]      = deviceInfo.deviceID;
    deviceNode["parentID"]      = deviceInfo.parentID;
    deviceNode["platformID"]    = deviceInfo.platformID;
    deviceNode["owner"]         = deviceInfo.owner;
    deviceNode["deviceIp"]      = deviceInfo.deviceIp;
    deviceNode["devicePort"]    = deviceInfo.devicePort;
    root["deviceInfo"]          = deviceNode;

    // 生成JSON字符串
    Json::StreamWriterBuilder builder;
    deviceInfoStr = Json::writeString(builder, root);

    return MyStatus_t::SUCCESS;
}

}; // namespace MY_UTILS