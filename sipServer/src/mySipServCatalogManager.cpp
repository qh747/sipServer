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
 * sip上级服务查询设备目录管理对象类
 */
class MySipServCatalogManage::MySipUpSevQueryCatalogManageObject
{
public:
    MyStatus_t add(const std::string& servId, const MySipServAddrCfg_dt& upServAddrCfg) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpServInfoMap.find(servId);
        if (m_sipUpServInfoMap.end() == iter) {
            MySipUpServInfoSubMap subMap;
            subMap.insert(std::make_pair(upServAddrCfg.id, upServAddrCfg));

            m_sipUpServInfoMap.insert(std::make_pair(servId, subMap));
            return MyStatus_t::SUCCESS;
        }
        else {
            auto subIter = iter->second.find(upServAddrCfg.id);
            if (iter->second.end() == subIter) {
                iter->second.insert(std::make_pair(upServAddrCfg.id, upServAddrCfg));
                return MyStatus_t::SUCCESS;
            }
        }
        return MyStatus_t::FAILED;
    }

    MyStatus_t del(const std::string& servId, const std::string& upServId) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpServInfoMap.find(servId);
        if (m_sipUpServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        auto subIter = iter->second.find(upServId);
        if (iter->second.end() == subIter) {
            return MyStatus_t::FAILED;
        }

        iter->second.erase(subIter);
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t delAll(const std::string& servId) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpServInfoMap.find(servId);
        if (m_sipUpServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        
        m_sipUpServInfoMap.erase(iter);
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t has(const std::string& servId, const std::string& upServId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpServInfoMap.find(servId);
        if (m_sipUpServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        auto subIter = iter->second.find(upServId);
        if (iter->second.end() == subIter) {
            return MyStatus_t::FAILED;
        }
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t get(const std::string& servId, MySipUpServInfoSubMap& upServAddrCfgMap) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipUpServInfoMap.find(servId);
        if (m_sipUpServInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        upServAddrCfgMap = iter->second;
        return MyStatus_t::SUCCESS;
    }

private:
    //                       读写互斥量
    boost::shared_mutex      m_rwMutex;

    //                       sip上级服务信息
    MySipUpServInfoMap       m_sipUpServInfoMap;
};

static MySipServCatalogManage::MySipServCatalogManageObject         CatalogManageObject;
static MySipServCatalogManage::MySipUpSevQueryCatalogManageObject   SipUpServgManageObject;

MyStatus_t MySipServCatalogManage::AddSipServCatalogInfo(const std::string& servId, const MySipCatalogInfo_dt& servCatalogInfo)
{
    return CatalogManageObject.add(servId, servCatalogInfo);
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

MyStatus_t MySipServCatalogManage::UpdateSipServCatalogSN(const std::string& servId, const std::string& sn)
{
    return CatalogManageObject.uptServSN(servId, sn);
}

MyStatus_t MySipServCatalogManage::AddSipUpQueryServInfo(const std::string& servId, const MY_COMMON::MySipServAddrCfg_dt& upServInfo)
{
    return SipUpServgManageObject.add(servId, upServInfo);
}
          
MyStatus_t MySipServCatalogManage::DelSipUpQueryServInfo(const std::string& servId, const std::string& upServId)
{
    return SipUpServgManageObject.del(servId, upServId);
}
          
MyStatus_t MySipServCatalogManage::HasSipUpQueryServInfo(const std::string& servId, const std::string& upServId)
{
    return SipUpServgManageObject.has(servId, upServId);
}
          
MyStatus_t MySipServCatalogManage::GetSipUpQueryServInfoMap(const std::string& servId, MySipUpServInfoSubMap& upServInfoMap)
{
    return SipUpServgManageObject.get(servId, upServInfoMap);
}

}; // namespace MY_MANAGER