#include <boost/thread/shared_mutex.hpp>
#include "manager/mySipServRegManage.h"
using namespace MY_COMMON;

namespace MY_MANAGER
{

/**
 * sip服务注册管理对象类
 */
class MySipServRegManage::MySipServRegManageObject
{
public:
    MyStatus_t addUpRegInfo(const std::string& servId, const MySipUpRegServInfo_dt& upRegServInfo) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(servId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            MySipUpRegServInfoSmtPtr upRegServInfoPtr = std::make_shared<MySipUpRegServInfo_dt>(upRegServInfo);

            MySipServRegManage::MySipUpRegServInfoSubMap subMap;
            subMap.insert(std::make_pair(upRegServInfoPtr->sipRegUpServCfg.upSipServRegAddrCfg.id, upRegServInfoPtr));

            m_sipUpRegServInfoMap.insert(std::make_pair(servId, subMap));
        }
        else {
            auto& subMap = iter->second;
            auto subIter = subMap.find(upRegServInfo.sipRegUpServCfg.upSipServRegAddrCfg.id);
            if (subMap.end() == subIter) {
                MySipUpRegServInfoSmtPtr upRegServInfoPtr = std::make_shared<MySipUpRegServInfo_dt>(upRegServInfo);
                subMap.insert(std::make_pair(upRegServInfoPtr->sipRegUpServCfg.upSipServRegAddrCfg.id, upRegServInfoPtr));
            }
            else {
                return MyStatus_t::FAILED;
            }
        }
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t delUpRegInfo(const std::string& servId, const std::string& upRegServId) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(servId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        
        auto subIter = iter->second.find(upRegServId);
        if (iter->second.end() == subIter) {
            return MyStatus_t::FAILED;
        }

        iter->second.erase(subIter);
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t delAllUpRegInfo(const std::string& servId) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(servId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        
        m_sipUpRegServInfoMap.erase(iter);
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t hasUpRegInfo(const std::string& servId, const std::string& upRegServId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(servId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        
        auto subIter = iter->second.find(upRegServId);
        if (iter->second.end() == subIter) {
            return MyStatus_t::FAILED;
        }
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getUpRegInfoMap(const std::string& servId, MySipServRegManage::MySipUpRegServInfoSubMap& upRegServInfoMap) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(servId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        upRegServInfoMap = iter->second;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getAllUpRegInfoMap(MySipServRegManage::MySipUpRegServInfoMap& upRegServInfoMap) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        upRegServInfoMap = m_sipUpRegServInfoMap;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getUpRegProto(const std::string& servId, const std::string& upRegServId, MyTpProto_t& proto) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(servId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        
        auto subIter = iter->second.find(upRegServId);
        if (iter->second.end() == subIter) {
            return MyStatus_t::FAILED;
        }

        proto = subIter->second->sipRegUpServCfg.proto;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getUpRegLastRegTime(const std::string& servId, const std::string& upRegServId, std::string& time) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(servId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        
        auto subIter = iter->second.find(upRegServId);
        if (iter->second.end() == subIter) {
            return MyStatus_t::FAILED;
        }

        time = subIter->second->sipRegUpServLastRegTime;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getUpRegExpired(const std::string& servId, const std::string& upRegServId, uint32_t& expired) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(servId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        
        auto subIter = iter->second.find(upRegServId);
        if (iter->second.end() == subIter) {
            return MyStatus_t::FAILED;
        }

        expired = subIter->second->sipRegUpServExpired;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getUpRegKeepAliveIdx(const std::string& servId, const std::string& upRegServId, uint32_t& idx) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(servId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        
        auto subIter = iter->second.find(upRegServId);
        if (iter->second.end() == subIter) {
            return MyStatus_t::FAILED;
        }

        idx = subIter->second->sipRegUpServKeepAliveIdx;
        return MyStatus_t::SUCCESS;
    }

    void uptUpRegLastRegTime(const std::string& servId, const std::string& upRegServId, const std::string& time) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(servId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return;
        }
        
        auto subIter = iter->second.find(upRegServId);
        if (iter->second.end() == subIter) {
            return;
        }
        subIter->second->sipRegUpServLastRegTime = time;
    }

    void uptUpRegExpired(const std::string& servId, const std::string& upRegServId, uint32_t expired) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(servId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return;
        }
        
        auto subIter = iter->second.find(upRegServId);
        if (iter->second.end() == subIter) {
            return;
        }
        subIter->second->sipRegUpServExpired = expired;
    }

    void uptUpRegKeepAliveIdx(const std::string& servId, const std::string& upRegServId, uint32_t keepAliveIdx) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(servId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return;
        }
        
        auto subIter = iter->second.find(upRegServId);
        if (iter->second.end() == subIter) {
            return;
        }
        subIter->second->sipRegUpServKeepAliveIdx = keepAliveIdx;
    }

public:
    MyStatus_t addLowRegInfo(const std::string& servId, const MySipLowRegServInfo_dt& lowRegServInfo) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(servId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            MySipLowRegServInfoSmtPtr lowRegServInfoPtr = std::make_shared<MySipLowRegServInfo_dt>(lowRegServInfo);

            MySipServRegManage::MySipLowRegServInfoSubMap subMap;
            subMap.insert(std::make_pair(lowRegServInfo.sipRegLowServCfg.lowSipServRegAddrCfg.id, lowRegServInfoPtr));

            m_sipLowRegServInfoMap.insert(std::make_pair(servId, subMap));
        }
        else {
            auto& subMap = iter->second;
            auto subIter = subMap.find(lowRegServInfo.sipRegLowServCfg.lowSipServRegAddrCfg.id);
            if (subMap.end() == subIter) {
                MySipLowRegServInfoSmtPtr lowRegServInfoPtr = std::make_shared<MySipLowRegServInfo_dt>(lowRegServInfo);
                subMap.insert(std::make_pair(lowRegServInfo.sipRegLowServCfg.lowSipServRegAddrCfg.id, lowRegServInfoPtr));
            }
            else {
                return MyStatus_t::FAILED;
            }
        }
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t delLowRegInfo(const std::string& servId, const std::string& lowRegServId) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(servId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        
        auto subIter = iter->second.find(lowRegServId);
        if (iter->second.end() == subIter) {
            return MyStatus_t::FAILED;
        }

        iter->second.erase(subIter);
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t delAllLowRegInfo(const std::string& servId) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(servId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        
        m_sipLowRegServInfoMap.erase(iter);
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t hasLowRegInfo(const std::string& servId, const std::string& lowRegServId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(servId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        
        auto subIter = iter->second.find(lowRegServId);
        if (iter->second.end() == subIter) {
            return MyStatus_t::FAILED;
        }
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getLowRegLastRegTime(const std::string& servId, const std::string& lowRegServId, std::string& time) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(servId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        
        auto subIter = iter->second.find(lowRegServId);
        if (iter->second.end() == subIter) {
            return MyStatus_t::FAILED;
        }

        time = subIter->second->sipRegLowServLastRegTime;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getLowRegExpired(const std::string& servId, const std::string& lowRegServId, uint32_t& expired) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(servId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        
        auto subIter = iter->second.find(lowRegServId);
        if (iter->second.end() == subIter) {
            return MyStatus_t::FAILED;
        }

        expired = subIter->second->sipRegLowServExpired;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getLowRegInfoMap(const std::string& servId, MySipServRegManage::MySipLowRegServInfoSubMap& lowRegServInfoMap) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(servId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        lowRegServInfoMap = iter->second;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getAllLowRegInfoMap(MySipServRegManage::MySipLowRegServInfoMap& lowRegServInfoMap) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        lowRegServInfoMap = m_sipLowRegServInfoMap;
        return MyStatus_t::SUCCESS;
    }

    void uptLowRegLastRegTime(const std::string& servId, const std::string& lowRegServId, const std::string& time) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(servId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return;
        }
        
        auto subIter = iter->second.find(lowRegServId);
        if (iter->second.end() == subIter) {
            return;
        }
        subIter->second->sipRegLowServLastRegTime = time;
    }

    void uptLowRegExpired(const std::string& servId, const std::string& lowRegServId, uint32_t expired) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(servId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return;
        }
        
        auto subIter = iter->second.find(lowRegServId);
        if (iter->second.end() == subIter) {
            return;
        }
        subIter->second->sipRegLowServExpired = expired;
    }

    void uptLowRegAddr(const std::string& servId, const std::string& lowRegServId, const MySipServRegAddrCfg_dt& lowRegServAddr) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(servId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return;
        }
        
        auto subIter = iter->second.find(lowRegServId);
        if (iter->second.end() == subIter) {
            return;
        }
        subIter->second->sipRegLowServCfg.lowSipServRegAddrCfg = lowRegServAddr;
    }

private:
    //                                          读写互斥量
    boost::shared_mutex                         m_rwMutex;

    //                                          上级sip注册服务信息
    MySipServRegManage::MySipUpRegServInfoMap   m_sipUpRegServInfoMap;

    //                                          下级sip注册服务信息
    MySipServRegManage::MySipLowRegServInfoMap  m_sipLowRegServInfoMap;
};

static MySipServRegManage::MySipServRegManageObject ManageObject;

MyStatus_t MySipServRegManage::GetSipLocalServId(const std::string& regServId, bool isUpRegServ, std::string& servId)
{
    if (isUpRegServ) {
        MySipServRegManage::MySipUpRegServInfoMap upRegServMap;
        if (MyStatus_t::SUCCESS != ManageObject.getAllUpRegInfoMap(upRegServMap)) {
            return MyStatus_t::FAILED;
        }

        for (const auto& pair : upRegServMap) {
            if (pair.second.end() != pair.second.find(regServId)) {
                servId = pair.first;
                return MyStatus_t::SUCCESS;
            }
        }
    }
    else {
        MySipServRegManage::MySipLowRegServInfoMap lowRegServMap;
        if (MyStatus_t::SUCCESS != ManageObject.getAllLowRegInfoMap(lowRegServMap)) {
            return MyStatus_t::FAILED;
        }

        for (const auto& pair : lowRegServMap) {
            if (pair.second.end() != pair.second.find(regServId)) {
                servId = pair.first;
                return MyStatus_t::SUCCESS;
            }
        }
    }
    return MyStatus_t::FAILED;
}

MyStatus_t MySipServRegManage::AddSipRegUpServInfo(const std::string& servId, const MySipUpRegServInfo_dt& upRegServInfo)
{
    return ManageObject.addUpRegInfo(servId, upRegServInfo);
}

MyStatus_t MySipServRegManage::DelSipRegUpServInfo(const std::string& servId, const std::string& upRegServId)
{
    return ManageObject.delUpRegInfo(servId, upRegServId);
}

MyStatus_t MySipServRegManage::DelAllSipRegUpServInfo(const std::string& servId)
{
    return ManageObject.delAllUpRegInfo(servId);
}

MyStatus_t MySipServRegManage::HasSipRegUpServInfo(const std::string& servId, const std::string& upRegServId)
{
    return ManageObject.hasUpRegInfo(servId, upRegServId);
}

MyStatus_t MySipServRegManage::GetSipRegUpServCfgMap(const std::string& servId, MySipRegUpServCfgMap& upRegCfgMap)
{
    MySipServRegManage::MySipUpRegServInfoSubMap upRegInfoMap;
    if (MyStatus_t::SUCCESS != ManageObject.getUpRegInfoMap(servId, upRegInfoMap)) {
        return MyStatus_t::FAILED;
    }

    if (upRegInfoMap.empty()) {
        return MyStatus_t::FAILED;
    }
    else {
        for (const auto& pair : upRegInfoMap) {
            upRegCfgMap.insert(std::make_pair(pair.first, pair.second->sipRegUpServCfg));
        }
        return MyStatus_t::SUCCESS;
    }
}

MyStatus_t MySipServRegManage::GetSipRegUpServCfg(const std::string& servId, const std::string& upRegServId, MySipRegUpServCfg_dt& upRegCfg)
{
    MySipRegUpServCfgMap upRegCfgMap;
    if(MyStatus_t::SUCCESS != MySipServRegManage::GetSipRegUpServCfgMap(servId, upRegCfgMap)) {
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

MyStatus_t MySipServRegManage::GetSipRegUpServProto(const std::string& servId, const std::string& upRegServId, MY_COMMON::MyTpProto_t& proto)
{
    return ManageObject.getUpRegProto(servId, upRegServId, proto);
}

MyStatus_t MySipServRegManage::GetSipRegUpServLastRegTime(const std::string& servId, const std::string& upRegServId, std::string& time)
{
    return ManageObject.getUpRegLastRegTime(servId, upRegServId, time);
}

MyStatus_t MySipServRegManage::GetSipRegUpServExpired(const std::string& servId, const std::string& upRegServId, uint32_t& expired)
{
    return ManageObject.getUpRegExpired(servId, upRegServId, expired);
}

MyStatus_t MySipServRegManage::GetSipRegUpServKeepAliveIdx(const std::string& servId, const std::string& upRegServId, uint32_t& idx)
{
    return ManageObject.getUpRegKeepAliveIdx(servId, upRegServId, idx);
}

void MySipServRegManage::UpdateSipRegUpServLastRegTime(const std::string& servId, const std::string& upRegServId, const std::string& time)
{
    ManageObject.uptUpRegLastRegTime(servId, upRegServId, time);
}

void MySipServRegManage::UpdateSipRegUpServExpired(const std::string& servId, const std::string& upRegServId, uint32_t expired)
{
    ManageObject.uptUpRegExpired(servId, upRegServId, expired);
}

void MySipServRegManage::UpdateSipRegUpServKeepAliveIdx(const std::string& servId, const std::string& upRegServId, uint32_t idx)
{
    ManageObject.uptUpRegKeepAliveIdx(servId, upRegServId, idx);
}

MyStatus_t MySipServRegManage::AddSipRegLowServInfo(const std::string& servId, const MySipLowRegServInfo_dt& lowRegServInfo)
{
    return ManageObject.addLowRegInfo(servId, lowRegServInfo);
}

MyStatus_t MySipServRegManage::DelSipRegLowServInfo(const std::string& servId, const std::string& lowRegServId)
{
    return ManageObject.delLowRegInfo(servId, lowRegServId);
}

MyStatus_t MySipServRegManage::DelAllSipRegLowServInfo(const std::string& servId)
{
    return ManageObject.delAllLowRegInfo(servId);
}

MyStatus_t MySipServRegManage::HasSipRegLowServInfo(const std::string& servId, const std::string& lowRegServId)
{
    return ManageObject.hasLowRegInfo(servId, lowRegServId);
}

MyStatus_t MySipServRegManage::GetSipRegLowServLastRegTime(const std::string& servId, const std::string& lowRegServId, std::string& time)
{
    return ManageObject.getLowRegLastRegTime(servId, lowRegServId, time);
}

MyStatus_t MySipServRegManage::GetSipRegLowServExpired(const std::string& servId, const std::string& lowRegServId, uint32_t& expired)
{
    return ManageObject.getLowRegExpired(servId, lowRegServId, expired);
}

MyStatus_t MySipServRegManage::GetSipRegLowServCfgMap(const std::string& servId, MySipRegLowServCfgMap& lowRegCfgMap)
{
    MySipServRegManage::MySipLowRegServInfoSubMap lowRegInfoMap;
    if (MyStatus_t::SUCCESS != ManageObject.getLowRegInfoMap(servId, lowRegInfoMap)) {
        return MyStatus_t::FAILED;
    }

    if (lowRegInfoMap.empty()) {
        return MyStatus_t::FAILED;
    }
    else {
        for (const auto& pair : lowRegInfoMap) {
            lowRegCfgMap.insert(std::make_pair(pair.first, pair.second->sipRegLowServCfg));
        }
        return MyStatus_t::SUCCESS;
    }
}

MyStatus_t MySipServRegManage::GetSipRegLowAuthCfg(const std::string& name, const std::string& realm, MySipServAuthCfg_dt& authCfg)
{
    MySipLowRegServInfoMap lowRegServInfoMap;
    if (MyStatus_t::SUCCESS != ManageObject.getAllLowRegInfoMap(lowRegServInfoMap)) {
        return MyStatus_t::FAILED;
    }

    if (lowRegServInfoMap.empty()) {
        return MyStatus_t::FAILED;
    }

    for (const auto& pair : lowRegServInfoMap) {
        for (const auto& subPair : pair.second) {
            if (subPair.second->sipRegLowServCfg.lowSipServRegAuthCfg.authName  == name &&
                subPair.second->sipRegLowServCfg.lowSipServRegAuthCfg.authRealm == realm) {
                authCfg = subPair.second->sipRegLowServCfg.lowSipServRegAuthCfg;
                return MyStatus_t::SUCCESS;
            }
        }
    }
    return MyStatus_t::FAILED;
}

void MySipServRegManage::UpdateSipRegLowServLastRegTime(const std::string& servId, const std::string& lowRegServId, const std::string& time)
{
    return ManageObject.uptLowRegLastRegTime(servId, lowRegServId, time);
}

void MySipServRegManage::UpdateSipRegLowServExpired(const std::string& servId, const std::string& lowRegServId, uint32_t expired)
{
    return ManageObject.uptLowRegExpired(servId, lowRegServId, expired);
}

void MySipServRegManage::UpdateSipRegLowServIpAddr(const std::string& servId, const std::string& lowRegServId, const MySipServRegAddrCfg_dt& lowRegServAddr)
{
    return ManageObject.uptLowRegAddr(servId, lowRegServId, lowRegServAddr);
}

}; // namespace MY_MANAGER