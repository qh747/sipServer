#include <boost/thread/shared_mutex.hpp>
#include "manager/mySipCatalogManage.h"
using namespace MY_COMMON;

namespace MY_MANAGER
{

/**
 * sip服务设备目录管理对象类
 */
class MySipCatalogManage::MySipCatalogManageObject
{
public:
    MyStatus_t add(const std::string& servId, const MySipCatalogInfo_dt& servCatalogInfo) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (m_sipServCatalogInfoMap.end() != m_sipServCatalogInfoMap.find(servId)) {
            return MyStatus_t::FAILED;
        }

        m_sipServCatalogInfoMap.insert(std::make_pair(servId, servCatalogInfo));
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t upt(const std::string& servId, const MySipCatalogInfo_dt& servCatalogInfo) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServCatalogInfoMap.find(servId);
        if (m_sipServCatalogInfoMap.end() == iter) {
            m_sipServCatalogInfoMap.insert(std::make_pair(servId, servCatalogInfo));
            return MyStatus_t::SUCCESS;
        }

        iter->second.sn = servCatalogInfo.sn;
        iter->second.sipPlatMap = servCatalogInfo.sipPlatMap;
        
        {
            auto& oldSipSubPlatMap = iter->second.sipSubPlatMap;
            const auto& newSipSubPlatMap = servCatalogInfo.sipSubPlatMap;
            for (const auto newSipSubPlatPair : newSipSubPlatMap) {
                auto oldSipSubPlatIter = oldSipSubPlatMap.find(newSipSubPlatPair.first);
                if (oldSipSubPlatMap.end() == oldSipSubPlatIter) {
                    oldSipSubPlatMap.insert(std::make_pair(newSipSubPlatPair.first, newSipSubPlatPair.second));
                    continue;
                }
                if (oldSipSubPlatIter->second != newSipSubPlatPair.second) {
                    oldSipSubPlatIter->second = newSipSubPlatPair.second;
                }
            }
        }
        
        {
            auto& oldSipSubVirtualPlatMap = iter->second.sipSubVirtualPlatMap;
            const auto& newSipSubVirtualPlatMap = servCatalogInfo.sipSubVirtualPlatMap;
            for (const auto newSipSubVirtualPlatPair : newSipSubVirtualPlatMap) {
                auto oldSipSubVirtualPlatIter = oldSipSubVirtualPlatMap.find(newSipSubVirtualPlatPair.first);
                if (oldSipSubVirtualPlatMap.end() == oldSipSubVirtualPlatIter) {
                    oldSipSubVirtualPlatMap.insert(std::make_pair(newSipSubVirtualPlatPair.first, newSipSubVirtualPlatPair.second));
                    continue;
                }
                if (oldSipSubVirtualPlatIter->second != newSipSubVirtualPlatPair.second) {
                    oldSipSubVirtualPlatIter->second = newSipSubVirtualPlatPair.second;
                }
            }
        }

        {
            auto& oldSipDeviceMap = iter->second.sipDeviceMap;
            const auto& newSipDeviceMap = servCatalogInfo.sipDeviceMap;
            for (const auto newSipDevicePair : newSipDeviceMap) {
                auto oldSipDeviceIter = oldSipDeviceMap.find(newSipDevicePair.first);
                if (oldSipDeviceMap.end() == oldSipDeviceIter) {
                    oldSipDeviceMap.insert(std::make_pair(newSipDevicePair.first, newSipDevicePair.second));
                    continue;
                }
                if (oldSipDeviceIter->second != newSipDevicePair.second) {
                    oldSipDeviceIter->second = newSipDevicePair.second;
                }
            }
        }
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t del(const std::string& servId) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServCatalogInfoMap.find(servId);
        if (m_sipServCatalogInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        m_sipServCatalogInfoMap.erase(iter);
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t has(const std::string& servId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        return (m_sipServCatalogInfoMap.end() != m_sipServCatalogInfoMap.find(servId) ? MyStatus_t::SUCCESS : MyStatus_t::FAILED);
    }

public:
    MyStatus_t getPlatCfgMap(const std::string& servId, MySipCatalogPlatCfgMap& platCfgMap) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServCatalogInfoMap.find(servId);
        if (m_sipServCatalogInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        platCfgMap = iter->second.sipPlatMap;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getSubPlatCfgMap(const std::string& servId, MySipCatalogSubPlatCfgMap& subPlatCfgMap) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServCatalogInfoMap.find(servId);
        if (m_sipServCatalogInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        subPlatCfgMap = iter->second.sipSubPlatMap;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getSubVirtualPlatCfgMap(const std::string& servId, MySipCatalogSubVirtualPlatCfgMap& subVirtualPlatCfgMap) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServCatalogInfoMap.find(servId);
        if (m_sipServCatalogInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        subVirtualPlatCfgMap = iter->second.sipSubVirtualPlatMap;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getDeviceCfgMap(const std::string& servId, MySipCatalogDeviceCfgMap& deviceCfgMap) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServCatalogInfoMap.find(servId);
        if (m_sipServCatalogInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        deviceCfgMap = iter->second.sipDeviceMap;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getServSN(const std::string& servId, std::string& sn) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);
        
        auto iter = m_sipServCatalogInfoMap.find(servId);
        if (m_sipServCatalogInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        sn = iter->second.sn;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t uptServSN(const std::string& servId, const std::string& sn) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);
        
        auto iter = m_sipServCatalogInfoMap.find(servId);
        if (m_sipServCatalogInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        iter->second.sn = sn;
        return MyStatus_t::SUCCESS;
    }

private:
    //                       读写互斥量
    boost::shared_mutex      m_rwMutex;

    //                       sip服务设备目录信息
    MySipServCatalogInfoMap  m_sipServCatalogInfoMap;
};

/**
 * sip上/下级服务设备目录管理对象类
 */
class MySipCatalogManage::MySipCatalogInfoManageObject
{
public:
    MyStatus_t add(const MySipServRegAddrCfg_dt& servAddrCfg, bool isUpServ) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto mapPtr = &m_sipUpServInfoMap;
        if (!isUpServ) {
            mapPtr = &m_sipLowServInfoMap;
        }

        if (mapPtr->end() != mapPtr->find(servAddrCfg.id)) {
            return MyStatus_t::FAILED;
        }
        
        mapPtr->insert(std::make_pair(servAddrCfg.id, servAddrCfg));
        return MyStatus_t::FAILED;
    }

    MyStatus_t del(const std::string& servId, bool isUpServ) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto mapPtr = &m_sipUpServInfoMap;
        if (!isUpServ) {
            mapPtr = &m_sipLowServInfoMap;
        }

        auto iter = mapPtr->find(servId);
        if (mapPtr->end() == iter) {
            return MyStatus_t::FAILED;
        }

        mapPtr->erase(iter);
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t has(const std::string& servId, bool isUpServ) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto mapPtr = &m_sipUpServInfoMap;
        if (!isUpServ) {
            mapPtr = &m_sipLowServInfoMap;
        }
        return (mapPtr->end() != mapPtr->find(servId) ? MyStatus_t::SUCCESS : MyStatus_t::FAILED);
    }

    MyStatus_t get(MySipRegServAddrMap& servAddrCfgMap, bool isUpServ) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        servAddrCfgMap = (isUpServ ? m_sipUpServInfoMap : m_sipLowServInfoMap);
        return MyStatus_t::SUCCESS;
    }

private:
    //                                  读写互斥量
    boost::shared_mutex                 m_rwMutex;

    //                                  sip上级服务信息
    MySipRegServAddrMap                 m_sipUpServInfoMap;

    //                                  sip下级服务信息
    MySipRegServAddrMap                 m_sipLowServInfoMap;
};

static MySipCatalogManage::MySipCatalogManageObject         CatalogManageObject;
static MySipCatalogManage::MySipCatalogInfoManageObject     CatalogSipServgManageObject;

MyStatus_t MySipCatalogManage::AddSipCatalogInfo(const std::string& servId, const MySipCatalogInfo_dt& servCatalogInfo)
{
    return CatalogManageObject.add(servId, servCatalogInfo);
}

MyStatus_t MySipCatalogManage::UptSipCatalogInfo(const std::string& servId, const MY_COMMON::MySipCatalogInfo_dt& servCatalogInfo)
{
    return CatalogManageObject.upt(servId, servCatalogInfo);
}

MyStatus_t MySipCatalogManage::DelSipCatalogInfo(const std::string& servId)
{
    return CatalogManageObject.del(servId);
}

MyStatus_t MySipCatalogManage::HasSipCatalogInfo(const std::string& servId)
{
    return CatalogManageObject.has(servId);
}

MyStatus_t MySipCatalogManage::GetSipCatalogPlatCfgMap(const std::string& servId, MySipCatalogPlatCfgMap& platCfgMap)
{
    return CatalogManageObject.getPlatCfgMap(servId, platCfgMap);
}

MyStatus_t MySipCatalogManage::GetSipCatalogSubPlatCfgMap(const std::string& servId, MySipCatalogSubPlatCfgMap& subPlatCfgMap)
{
    return CatalogManageObject.getSubPlatCfgMap(servId, subPlatCfgMap);
}

MyStatus_t MySipCatalogManage::GetSipCatalogSubVirtualPlatCfgMap(const std::string& servId, MySipCatalogSubVirtualPlatCfgMap& subVirtualPlatCfgMap)
{
    return CatalogManageObject.getSubVirtualPlatCfgMap(servId, subVirtualPlatCfgMap);
}

MyStatus_t MySipCatalogManage::GetSipCatalogDeviceCfgMap(const std::string& servId, MySipCatalogDeviceCfgMap& deviceCfgMap)
{
    return CatalogManageObject.getDeviceCfgMap(servId, deviceCfgMap);
}

MyStatus_t MySipCatalogManage::GetSipCatalogSN(const std::string& servId, std::string& sn)
{
    return CatalogManageObject.getServSN(servId, sn);
}

MyStatus_t MySipCatalogManage::UpdateSipCatalogSN(const std::string& servId, const std::string& sn)
{
    return CatalogManageObject.uptServSN(servId, sn);
}

MyStatus_t MySipCatalogManage::AddSipQueryInfo(const MY_COMMON::MySipServRegAddrCfg_dt& servAddr)
{
    return CatalogSipServgManageObject.add(servAddr, true);
}
          
MyStatus_t MySipCatalogManage::DelSipQueryInfo(const std::string& upServId)
{
    return CatalogSipServgManageObject.del(upServId, true);
}
          
MyStatus_t MySipCatalogManage::HasSipQueryInfo(const std::string& upServId)
{
    return CatalogSipServgManageObject.has(upServId, true);
}
          
MyStatus_t MySipCatalogManage::GetSipQueryInfo(MySipRegServAddrMap& servAddrMap)
{
    return CatalogSipServgManageObject.get(servAddrMap, true);
}

MyStatus_t MySipCatalogManage::AddSipRespInfo(const MY_COMMON::MySipServRegAddrCfg_dt& servAddr)
{
    return CatalogSipServgManageObject.add(servAddr, false);
}
          
MyStatus_t MySipCatalogManage::DelSipRespInfo(const std::string& lowServId)
{
    return CatalogSipServgManageObject.del(lowServId, false);
}
          
MyStatus_t MySipCatalogManage::HasSipRespInfo(const std::string& lowServId)
{
    return CatalogSipServgManageObject.has(lowServId, false);
}
          
MyStatus_t MySipCatalogManage::GetSipRespInfoMap(MySipRegServAddrMap& servAddrMap)
{
    return CatalogSipServgManageObject.get(servAddrMap, false);
}

}; // namespace MY_MANAGER