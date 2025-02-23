#include <boost/thread/shared_mutex.hpp>
#include "manager/mySipServCatalogManager.h"
using namespace MY_COMMON;

namespace MY_MANAGER
{

/**
 * sip服务设备目录管理对象类
 */
class MySipServCatalogManage::MySipServCatalogManageObject
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

        {
            auto& oldSipPlatMap = iter->second.sipPlatMap;
            const auto& newSipPlatMap = servCatalogInfo.sipPlatMap;
            for (const auto newSipPlatPair : newSipPlatMap) {
                auto oldSipPlatIter = oldSipPlatMap.find(newSipPlatPair.first);
                if (oldSipPlatMap.end() == oldSipPlatIter) {
                    oldSipPlatMap.insert(std::make_pair(newSipPlatPair.first, newSipPlatPair.second));
                    continue;
                }
                if (oldSipPlatIter->second != newSipPlatPair.second) {
                    oldSipPlatIter->second = newSipPlatPair.second;
                }
            }
        }
        
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
class MySipServCatalogManage::MySipServCatalogInfoManageObject
{
public:
    MyStatus_t add(const std::string& servId, const MySipServAddrCfg_dt& servAddrCfg, bool isUpServ) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto mapPtr = &m_sipUpServInfoMap;
        if (!isUpServ) {
            mapPtr = &m_sipLowServInfoMap;
        }

        auto iter = mapPtr->find(servId);
        if (mapPtr->end() == iter) {
            MySipServAddrMap subMap;
            subMap.insert(std::make_pair(servAddrCfg.id, servAddrCfg));

            mapPtr->insert(std::make_pair(servId, subMap));
            return MyStatus_t::SUCCESS;
        }
        else {
            auto subIter = iter->second.find(servAddrCfg.id);
            if (iter->second.end() == subIter) {
                iter->second.insert(std::make_pair(servAddrCfg.id, servAddrCfg));
                return MyStatus_t::SUCCESS;
            }
        }
        return MyStatus_t::FAILED;
    }

    MyStatus_t del(const std::string& servId, const std::string& upServId, bool isUpServ) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto mapPtr = &m_sipUpServInfoMap;
        if (!isUpServ) {
            mapPtr = &m_sipLowServInfoMap;
        }

        auto iter = mapPtr->find(servId);
        if (mapPtr->end() == iter) {
            return MyStatus_t::FAILED;
        }

        auto subIter = iter->second.find(upServId);
        if (iter->second.end() == subIter) {
            return MyStatus_t::FAILED;
        }

        iter->second.erase(subIter);
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t delAll(const std::string& servId, bool isUpServ) {
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

    MyStatus_t has(const std::string& servId, const std::string& upServId, bool isUpServ) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto mapPtr = &m_sipUpServInfoMap;
        if (!isUpServ) {
            mapPtr = &m_sipLowServInfoMap;
        }

        auto iter = mapPtr->find(servId);
        if (mapPtr->end() == iter) {
            return MyStatus_t::FAILED;
        }

        auto subIter = iter->second.find(upServId);
        if (iter->second.end() == subIter) {
            return MyStatus_t::FAILED;
        }
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t get(const std::string& servId, MySipServAddrMap& servAddrCfgMap, bool isUpServ) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto mapPtr = &m_sipUpServInfoMap;
        if (!isUpServ) {
            mapPtr = &m_sipLowServInfoMap;
        }

        auto iter = mapPtr->find(servId);
        if (mapPtr->end() == iter) {
            return MyStatus_t::FAILED;
        }
        servAddrCfgMap = iter->second;
        return MyStatus_t::SUCCESS;
    }

private:
    //                                  读写互斥量
    boost::shared_mutex                 m_rwMutex;

    //                                  sip上级服务信息
    MySipServCatalogInfoAddrMap         m_sipUpServInfoMap;

    //                                  sip下级服务信息
    MySipServCatalogInfoAddrMap         m_sipLowServInfoMap;
};

static MySipServCatalogManage::MySipServCatalogManageObject         CatalogManageObject;
static MySipServCatalogManage::MySipServCatalogInfoManageObject     CatalogSipServgManageObject;

MyStatus_t MySipServCatalogManage::AddSipServCatalogInfo(const std::string& servId, const MySipCatalogInfo_dt& servCatalogInfo)
{
    return CatalogManageObject.add(servId, servCatalogInfo);
}

MyStatus_t MySipServCatalogManage::UptSipServCatalogInfo(const std::string& servId, const MY_COMMON::MySipCatalogInfo_dt& servCatalogInfo)
{
    return CatalogManageObject.upt(servId, servCatalogInfo);
}

MyStatus_t MySipServCatalogManage::DelSipServCatalogInfo(const std::string& servId)
{
    return CatalogManageObject.del(servId);
}

MyStatus_t MySipServCatalogManage::HasSipServCatalogInfo(const std::string& servId)
{
    return CatalogManageObject.has(servId);
}

MyStatus_t MySipServCatalogManage::GetSipServCatalogPlatCfgMap(const std::string& servId, MySipCatalogPlatCfgMap& platCfgMap)
{
    return CatalogManageObject.getPlatCfgMap(servId, platCfgMap);
}

MyStatus_t MySipServCatalogManage::GetSipServCatalogSubPlatCfgMap(const std::string& servId, MySipCatalogSubPlatCfgMap& subPlatCfgMap)
{
    return CatalogManageObject.getSubPlatCfgMap(servId, subPlatCfgMap);
}

MyStatus_t MySipServCatalogManage::GetSipServCatalogSubVirtualPlatCfgMap(const std::string& servId, MySipCatalogSubVirtualPlatCfgMap& subVirtualPlatCfgMap)
{
    return CatalogManageObject.getSubVirtualPlatCfgMap(servId, subVirtualPlatCfgMap);
}

MyStatus_t MySipServCatalogManage::GetSipServCatalogDeviceCfgMap(const std::string& servId, MySipCatalogDeviceCfgMap& deviceCfgMap)
{
    return CatalogManageObject.getDeviceCfgMap(servId, deviceCfgMap);
}

MyStatus_t MySipServCatalogManage::GetSipServCatalogSN(const std::string& servId, std::string& sn)
{
    return CatalogManageObject.getServSN(servId, sn);
}

MyStatus_t MySipServCatalogManage::UpdateSipServCatalogSN(const std::string& servId, const std::string& sn)
{
    return CatalogManageObject.uptServSN(servId, sn);
}

MyStatus_t MySipServCatalogManage::AddSipUpQueryServInfo(const std::string& servId, const MY_COMMON::MySipServAddrCfg_dt& servAddr)
{
    return CatalogSipServgManageObject.add(servId, servAddr, true);
}
          
MyStatus_t MySipServCatalogManage::DelSipUpQueryServInfo(const std::string& servId, const std::string& upServId)
{
    return CatalogSipServgManageObject.del(servId, upServId, true);
}
          
MyStatus_t MySipServCatalogManage::HasSipUpQueryServInfo(const std::string& servId, const std::string& upServId)
{
    return CatalogSipServgManageObject.has(servId, upServId, true);
}
          
MyStatus_t MySipServCatalogManage::GetSipUpQueryServInfo(const std::string& servId, MySipServAddrMap& servAddrMap)
{
    return CatalogSipServgManageObject.get(servId, servAddrMap, true);
}

MyStatus_t MySipServCatalogManage::AddSipLowRespServInfo(const std::string& servId, const MY_COMMON::MySipServAddrCfg_dt& servAddr)
{
    return CatalogSipServgManageObject.add(servId, servAddr, false);
}
          
MyStatus_t MySipServCatalogManage::DelSipLowRespServInfo(const std::string& servId, const std::string& lowServId)
{
    return CatalogSipServgManageObject.del(servId, lowServId, false);
}
          
MyStatus_t MySipServCatalogManage::HasSipLowRespServInfo(const std::string& servId, const std::string& lowServId)
{
    return CatalogSipServgManageObject.has(servId, lowServId, false);
}
          
MyStatus_t MySipServCatalogManage::GetSipLowRespServInfoMap(const std::string& servId, MySipServAddrMap& servAddrMap)
{
    return CatalogSipServgManageObject.get(servId, servAddrMap, false);
}

}; // namespace MY_MANAGER