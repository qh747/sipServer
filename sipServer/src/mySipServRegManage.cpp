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
            subMap.insert(std::make_pair(upRegServInfoPtr->sipRegUpServCfg.upSipServAddrCfg.id, upRegServInfoPtr));

            m_sipUpRegServInfoMap.insert(std::make_pair(servId, subMap));
        }
        else {
            auto& subMap = iter->second;
            auto subIter = subMap.find(upRegServInfo.sipRegUpServCfg.upSipServAddrCfg.id);
            if (subMap.end() == subIter) {
                MySipUpRegServInfoSmtPtr upRegServInfoPtr = std::make_shared<MySipUpRegServInfo_dt>(upRegServInfo);
                subMap.insert(std::make_pair(upRegServInfoPtr->sipRegUpServCfg.upSipServAddrCfg.id, upRegServInfoPtr));
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

    MySipServRegManage::MySipUpRegServInfoSubMap getUpRegInfoMap(const std::string& servId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(servId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return MySipServRegManage::MySipUpRegServInfoSubMap();
        }
        return iter->second;
    }

    MySipServRegManage::MySipUpRegServInfoMap getAllUpRegInfoMap() {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        return m_sipUpRegServInfoMap;
    }

    std::string getUpRegLastRegTime(const std::string& servId, const std::string& upRegServId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(servId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return "";
        }
        
        auto subIter = iter->second.find(upRegServId);
        if (iter->second.end() == subIter) {
            return "";
        }
        return subIter->second->sipRegUpServLastRegTime;
    }

    uint32_t getUpRegExpired(const std::string& servId, const std::string& upRegServId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(servId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return 0;
        }
        
        auto subIter = iter->second.find(upRegServId);
        if (iter->second.end() == subIter) {
            return 0;
        }
        return subIter->second->sipRegUpServExpired;
    }

    uint32_t getUpRegKeepAliveIdx(const std::string& servId, const std::string& upRegServId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpRegServInfoMap.find(servId);
        if (m_sipUpRegServInfoMap.end() == iter) {
            return 0;
        }
        
        auto subIter = iter->second.find(upRegServId);
        if (iter->second.end() == subIter) {
            return 0;
        }
        return subIter->second->sipRegUpServKeepAliveIdx;
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
            subMap.insert(std::make_pair(lowRegServInfo.sipRegLowServCfg.lowSipServAddrCfg.id, lowRegServInfoPtr));

            m_sipLowRegServInfoMap.insert(std::make_pair(servId, subMap));
        }
        else {
            auto& subMap = iter->second;
            auto subIter = subMap.find(lowRegServInfo.sipRegLowServCfg.lowSipServAddrCfg.id);
            if (subMap.end() == subIter) {
                MySipLowRegServInfoSmtPtr lowRegServInfoPtr = std::make_shared<MySipLowRegServInfo_dt>(lowRegServInfo);
                subMap.insert(std::make_pair(lowRegServInfo.sipRegLowServCfg.lowSipServAddrCfg.id, lowRegServInfoPtr));
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

    std::string getLowRegLastRegTime(const std::string& servId, const std::string& lowRegServId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(servId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return "";
        }
        
        auto subIter = iter->second.find(lowRegServId);
        if (iter->second.end() == subIter) {
            return "";
        }
        return subIter->second->sipRegLowServLastRegTime;
    }

    uint32_t getLowRegExpired(const std::string& servId, const std::string& lowRegServId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(servId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return 0;
        }
        
        auto subIter = iter->second.find(lowRegServId);
        if (iter->second.end() == subIter) {
            return 0;
        }
        return subIter->second->sipRegLowServExpired;
    }

    MySipServRegManage::MySipLowRegServInfoSubMap getLowRegInfoMap(const std::string& servId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(servId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return MySipServRegManage::MySipLowRegServInfoSubMap();
        }
        return iter->second;
    }

    MySipServRegManage::MySipLowRegServInfoMap getAllLowRegInfoMap() {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        return m_sipLowRegServInfoMap;
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

    void uptLowRegAddr(const std::string& servId, const std::string& lowRegServId, const MySipServAddrCfg_dt& lowRegServAddr) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipLowRegServInfoMap.find(servId);
        if (m_sipLowRegServInfoMap.end() == iter) {
            return;
        }
        
        auto subIter = iter->second.find(lowRegServId);
        if (iter->second.end() == subIter) {
            return;
        }
        subIter->second->sipRegLowServCfg.lowSipServAddrCfg = lowRegServAddr;
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

std::string MySipServRegManage::GetSipLocalServId(const std::string& regServId, bool isUpRegServ)
{
    if (isUpRegServ) {
        auto upRegServMap = ManageObject.getAllUpRegInfoMap();
        for (const auto& pair : upRegServMap) {
            if (pair.second.end() != pair.second.find(regServId)) {
                return pair.first;
            }
        }
    }
    else {
        auto lowRegServMap = ManageObject.getAllLowRegInfoMap();
        for (const auto& pair : lowRegServMap) {
            if (pair.second.end() != pair.second.find(regServId)) {
                return pair.first;
            }
        }
    }
    return "";
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

MySipRegUpServCfgMap MySipServRegManage::GetSipRegUpServCfgMap(const std::string& servId)
{
    auto upRegInfoMap = ManageObject.getUpRegInfoMap(servId);
    if (upRegInfoMap.empty()) {
        return MySipRegUpServCfgMap();
    }
    else {
        MySipRegUpServCfgMap upRegCfgMap;
        for (const auto& pair : upRegInfoMap) {
            upRegCfgMap.insert(std::make_pair(pair.first, pair.second->sipRegUpServCfg));
        }
        return upRegCfgMap;
    }
}

std::string MySipServRegManage::GetSipRegUpServLastRegTime(const std::string& servId, const std::string& upRegServId)
{
    return ManageObject.getUpRegLastRegTime(servId, upRegServId);
}

uint32_t MySipServRegManage::GetSipRegUpServExpired(const std::string& servId, const std::string& upRegServId)
{
    return ManageObject.getUpRegExpired(servId, upRegServId);
}

uint32_t MySipServRegManage::GetSipRegUpServKeepAliveIdx(const std::string& servId, const std::string& upRegServId)
{
    return ManageObject.getUpRegKeepAliveIdx(servId, upRegServId);
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

std::string MySipServRegManage::GetSipRegLowServLastRegTime(const std::string& servId, const std::string& lowRegServId)
{
    return ManageObject.getLowRegLastRegTime(servId, lowRegServId);
}

uint32_t MySipServRegManage::GetSipRegLowServExpired(const std::string& servId, const std::string& lowRegServId)
{
    return ManageObject.getLowRegExpired(servId, lowRegServId);
}

MySipRegLowServCfgMap MySipServRegManage::GetSipRegLowServCfgMap(const std::string& servId)
{
    auto lowRegInfoMap = ManageObject.getLowRegInfoMap(servId);
    if (lowRegInfoMap.empty()) {
        return MySipRegLowServCfgMap();
    }
    else {
        MySipRegLowServCfgMap lowRegCfgMap;
        for (const auto& pair : lowRegInfoMap) {
            lowRegCfgMap.insert(std::make_pair(pair.first, pair.second->sipRegLowServCfg));
        }
        return lowRegCfgMap;
    }
}

MySipServAuthCfg_dt MySipServRegManage::GetSipRegLowAuthCfg(const std::string& name, const std::string& realm)
{
    MySipLowRegServInfoMap lowRegServInfoMap = ManageObject.getAllLowRegInfoMap();
    for (const auto& pair : lowRegServInfoMap) {
        const auto& cfgMap = pair.second;
        for (const auto& subPair : cfgMap) {
            if (subPair.second->sipRegLowServCfg.lowSipServAuthCfg.authName == name &&
                subPair.second->sipRegLowServCfg.lowSipServAuthCfg.authRealm == realm) {
                return subPair.second->sipRegLowServCfg.lowSipServAuthCfg;
            }
        }
    }
    return MySipServAuthCfg_dt();
}

void MySipServRegManage::UpdateSipRegLowServLastRegTime(const std::string& servId, const std::string& lowRegServId, const std::string& time)
{
    return ManageObject.uptLowRegLastRegTime(servId, lowRegServId, time);
}

void MySipServRegManage::UpdateSipRegLowServExpired(const std::string& servId, const std::string& lowRegServId, uint32_t expired)
{
    return ManageObject.uptLowRegExpired(servId, lowRegServId, expired);
}

void MySipServRegManage::UpdateSipRegLowServIpAddr(const std::string& servId, const std::string& lowRegServId, const MySipServAddrCfg_dt& lowRegServAddr)
{
    return ManageObject.uptLowRegAddr(servId, lowRegServId, lowRegServAddr);
}

}; // namespace MY_MANAGER