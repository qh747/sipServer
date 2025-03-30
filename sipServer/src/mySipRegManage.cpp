#include <boost/thread/shared_mutex.hpp>
#include "manager/mySipRegManage.h"
using namespace MY_COMMON;

namespace MY_MANAGER
{

/**
 * sip服务注册管理对象类
 */
class MySipRegManage::MySipServRegManageObject
{
public:
    MyStatus_t addUpRegInfo(const MySipUpRegServInfo_dt& upRegServInfo) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (m_sipUpRegServInfoMap.end() != m_sipUpRegServInfoMap.find(upRegServInfo.sipRegUpServCfg.upSipServRegAddrCfg.id)) {
            return MyStatus_t::FAILED;
        }

        m_sipUpRegServInfoMap.insert(std::make_pair(upRegServInfo.sipRegUpServCfg.upSipServRegAddrCfg.id, upRegServInfo));
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t delUpRegInfo(const std::string& upRegServId) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(upRegServId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        m_sipUpRegServInfoMap.erase(iter);
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t hasUpRegInfo(const std::string& upRegServId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        return (m_sipUpRegServInfoMap.end() != m_sipUpRegServInfoMap.find(upRegServId) ? MyStatus_t::SUCCESS : MyStatus_t::FAILED);
    }

    MyStatus_t getUpRegInfoMap(MySipRegManage::MySipUpRegServInfoMap& upRegServInfoMap) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        upRegServInfoMap = m_sipUpRegServInfoMap;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getUpRegProto(const std::string& upRegServId, MyTpProto_t& proto) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(upRegServId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        proto = iter->second.sipRegUpServCfg.proto;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getUpRegLastRegTime(const std::string& upRegServId, std::string& time) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        
        auto iter = m_sipUpRegServInfoMap.find(upRegServId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        time = iter->second.sipRegUpServLastRegTime;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getUpRegExpired(const std::string& upRegServId, uint32_t& expired) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(upRegServId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        expired = iter->second.sipRegUpServExpired;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getUpRegKeepAliveIdx(const std::string& upRegServId, uint32_t& idx) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(upRegServId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        idx = iter->second.sipRegUpServKeepAliveIdx;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t uptUpRegLastRegTime(const std::string& upRegServId, const std::string& time) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(upRegServId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        iter->second.sipRegUpServLastRegTime = time;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t uptUpRegExpired(const std::string& upRegServId, uint32_t expired) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(upRegServId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        iter->second.sipRegUpServExpired = expired;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t uptUpRegKeepAliveIdx(const std::string& upRegServId, uint32_t keepAliveIdx) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(upRegServId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        iter->second.sipRegUpServKeepAliveIdx = keepAliveIdx;
        return MyStatus_t::SUCCESS;
    }

public:
    MyStatus_t addLowRegInfo(const MySipLowRegServInfo_dt& lowRegServInfo) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (m_sipLowRegServInfoMap.end() != m_sipLowRegServInfoMap.find(lowRegServInfo.sipRegLowServCfg.lowSipServRegAddrCfg.id)) {
            return MyStatus_t::FAILED;
        }

        m_sipLowRegServInfoMap.insert(std::make_pair(lowRegServInfo.sipRegLowServCfg.lowSipServRegAddrCfg.id, lowRegServInfo));
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t delLowRegInfo(const std::string& lowRegServId) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(lowRegServId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        m_sipLowRegServInfoMap.erase(iter);
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t hasLowRegInfo(const std::string& lowRegServId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        return (m_sipLowRegServInfoMap.end() != m_sipLowRegServInfoMap.find(lowRegServId) ? MyStatus_t::SUCCESS : MyStatus_t::FAILED);
    }

    MyStatus_t getLowRegLastRegTime(const std::string& lowRegServId, std::string& time) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(lowRegServId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        time = iter->second.sipRegLowServLastRegTime;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getLowRegExpired(const std::string& lowRegServId, uint32_t& expired) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(lowRegServId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        expired = iter->second.sipRegLowServExpired;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getLowRegProto(const std::string& lowRegServId, MyTpProto_t& proto) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(lowRegServId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        proto = iter->second.sipRegLowServCfg.proto;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getLowRegInfoMap(MySipRegManage::MySipLowRegServInfoMap& lowRegServInfoMap) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        lowRegServInfoMap = m_sipLowRegServInfoMap;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t uptLowRegLastRegTime(const std::string& lowRegServId, const std::string& time) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(lowRegServId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        iter->second.sipRegLowServLastRegTime = time;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t uptLowRegExpired(const std::string& lowRegServId, uint32_t expired) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(lowRegServId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        iter->second.sipRegLowServExpired = expired;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t uptLowRegAddr(const std::string& lowRegServId, const MySipServRegAddrCfg_dt& lowRegServAddr) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(lowRegServId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        iter->second.sipRegLowServCfg.lowSipServRegAddrCfg = lowRegServAddr;
        return MyStatus_t::SUCCESS;
    }

private:
    //                                              读写互斥量
    boost::shared_mutex                             m_rwMutex;

    //                                              上级sip注册服务信息
    MySipRegManage::MySipUpRegServInfoMap       m_sipUpRegServInfoMap;

    //                                              下级sip注册服务信息
    MySipRegManage::MySipLowRegServInfoMap      m_sipLowRegServInfoMap;
};

static MySipRegManage::MySipServRegManageObject ManageObject;

MyStatus_t MySipRegManage::AddSipRegUpServInfo(const MySipUpRegServInfo_dt& upRegServInfo)
{
    return ManageObject.addUpRegInfo(upRegServInfo);
}

MyStatus_t MySipRegManage::DelSipRegUpServInfo(const std::string& upRegServId)
{
    return ManageObject.delUpRegInfo(upRegServId);
}

MyStatus_t MySipRegManage::HasSipRegUpServInfo(const std::string& upRegServId)
{
    return ManageObject.hasUpRegInfo(upRegServId);
}

MyStatus_t MySipRegManage::GetSipRegUpServCfgMap(MySipRegUpServCfgMap& upRegCfgMap)
{
    MySipRegManage::MySipUpRegServInfoMap upRegInfoMap;
    ManageObject.getUpRegInfoMap(upRegInfoMap);

    if (upRegInfoMap.empty()) {
        return MyStatus_t::FAILED;
    }
    
    for (const auto& pair : upRegInfoMap) {
        upRegCfgMap.insert(std::make_pair(pair.first, pair.second.sipRegUpServCfg));
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegManage::GetSipRegUpServCfg(const std::string& upRegServId, MySipRegUpServCfg_dt& upRegCfg)
{
    MySipRegUpServCfgMap upRegCfgMap;
    if(MyStatus_t::SUCCESS != MySipRegManage::GetSipRegUpServCfgMap( upRegCfgMap)) {
        return MyStatus_t::FAILED;
    }

    if (upRegCfgMap.empty()) {
        return MyStatus_t::FAILED;
    }

    auto iter = upRegCfgMap.find(upRegServId);
    if (upRegCfgMap.end() == iter) {
        return MyStatus_t::FAILED;
    }

    upRegCfg = iter->second;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegManage::GetSipRegUpServProto(const std::string& upRegServId, MY_COMMON::MyTpProto_t& proto)
{
    return ManageObject.getUpRegProto(upRegServId, proto);
}

MyStatus_t MySipRegManage::GetSipRegUpServLastRegTime(const std::string& upRegServId, std::string& time)
{
    return ManageObject.getUpRegLastRegTime(upRegServId, time);
}

MyStatus_t MySipRegManage::GetSipRegUpServExpired(const std::string& upRegServId, uint32_t& expired)
{
    return ManageObject.getUpRegExpired(upRegServId, expired);
}

MyStatus_t MySipRegManage::GetSipRegUpServKeepAliveIdx(const std::string& upRegServId, uint32_t& idx)
{
    return ManageObject.getUpRegKeepAliveIdx(upRegServId, idx);
}

MyStatus_t MySipRegManage::UpdateSipRegUpServLastRegTime(const std::string& upRegServId, const std::string& time)
{
    return ManageObject.uptUpRegLastRegTime(upRegServId, time);
}

MyStatus_t MySipRegManage::UpdateSipRegUpServExpired(const std::string& upRegServId, uint32_t expired)
{
    return ManageObject.uptUpRegExpired(upRegServId, expired);
}

MyStatus_t MySipRegManage::UpdateSipRegUpServKeepAliveIdx(const std::string& upRegServId, uint32_t idx)
{
    return ManageObject.uptUpRegKeepAliveIdx(upRegServId, idx);
}

MyStatus_t MySipRegManage::AddSipRegLowServInfo(const MySipLowRegServInfo_dt& lowRegServInfo)
{
    return ManageObject.addLowRegInfo(lowRegServInfo);
}

MyStatus_t MySipRegManage::DelSipRegLowServInfo(const std::string& lowRegServId)
{
    return ManageObject.delLowRegInfo(lowRegServId);
}

MyStatus_t MySipRegManage::HasSipRegLowServInfo(const std::string& lowRegServId)
{
    return ManageObject.hasLowRegInfo(lowRegServId);
}

MyStatus_t MySipRegManage::GetSipRegLowServLastRegTime(const std::string& lowRegServId, std::string& time)
{
    return ManageObject.getLowRegLastRegTime(lowRegServId, time);
}

MyStatus_t MySipRegManage::GetSipRegLowServExpired(const std::string& lowRegServId, uint32_t& expired)
{
    return ManageObject.getLowRegExpired(lowRegServId, expired);
}

MyStatus_t MySipRegManage::GetSipRegLowServCfgMap(MySipRegLowServCfgMap& lowRegCfgMap)
{
    MySipRegManage::MySipLowRegServInfoMap lowRegInfoMap;
    ManageObject.getLowRegInfoMap(lowRegInfoMap);

    if (lowRegInfoMap.empty()) {
        return MyStatus_t::FAILED;
    }
    
    for (const auto& pair : lowRegInfoMap) {
        lowRegCfgMap.insert(std::make_pair(pair.first, pair.second.sipRegLowServCfg));
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegManage::GetSipRegLowServCfg(const std::string& lowRegServId, MY_COMMON::MySipRegLowServCfg_dt& lowRegServCfg)
{
    MySipRegLowServCfgMap lowRegCfgMap;
    if(MyStatus_t::SUCCESS != MySipRegManage::GetSipRegLowServCfgMap( lowRegCfgMap)) {
        return MyStatus_t::FAILED;
    }

    if (lowRegCfgMap.empty()) {
        return MyStatus_t::FAILED;
    }

    auto iter = lowRegCfgMap.find(lowRegServId);
    if (lowRegCfgMap.end() == iter) {
        return MyStatus_t::FAILED;
    }

    lowRegServCfg = iter->second;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipRegManage::GetSipRegLowAuthCfg(const std::string& name, const std::string& realm, MySipServAuthCfg_dt& authCfg)
{
    MySipLowRegServInfoMap lowRegServInfoMap;
    ManageObject.getLowRegInfoMap(lowRegServInfoMap);

    if (lowRegServInfoMap.empty()) {
        return MyStatus_t::FAILED;
    }

    for (const auto& pair : lowRegServInfoMap) {
        if (pair.second.sipRegLowServCfg.lowSipServRegAuthCfg.authName  == name && pair.second.sipRegLowServCfg.lowSipServRegAuthCfg.authRealm == realm) {
            authCfg = pair.second.sipRegLowServCfg.lowSipServRegAuthCfg;
            return MyStatus_t::SUCCESS;
        }
    }
    return MyStatus_t::FAILED;
}

MyStatus_t MySipRegManage::GetSipRegLowServProto(const std::string& lowRegServId, MyTpProto_t& proto)
{
    return ManageObject.getLowRegProto(lowRegServId, proto);
}

MyStatus_t MySipRegManage::UpdateSipRegLowServLastRegTime(const std::string& lowRegServId, const std::string& time)
{
    return ManageObject.uptLowRegLastRegTime(lowRegServId, time);
}

MyStatus_t MySipRegManage::UpdateSipRegLowServExpired(const std::string& lowRegServId, uint32_t expired)
{
    return ManageObject.uptLowRegExpired(lowRegServId, expired);
}

MyStatus_t MySipRegManage::UpdateSipRegLowServIpAddr(const std::string& lowRegServId, const MySipServRegAddrCfg_dt& lowRegServAddr)
{
    return ManageObject.uptLowRegAddr(lowRegServId, lowRegServAddr);
}

}; // namespace MY_MANAGER