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
    MyStatus_t add(const std::string& servId, const MY_COMMON::MySipCatalogInfo_dt& servCatalogInfo) {
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

    MySipCatalogPlatCfgMap getPlatCfgMap(const std::string& servId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServCatalogInfoMap.find(servId);
        if (m_sipServCatalogInfoMap.end() == iter) {
            return MySipCatalogPlatCfgMap();
        }
        return iter->second.sipPlatMap;
    }

    MySipCatalogSubPlatCfgMap getSubPlatCfgMap(const std::string& servId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServCatalogInfoMap.find(servId);
        if (m_sipServCatalogInfoMap.end() == iter) {
            return MySipCatalogSubPlatCfgMap();
        }
        return iter->second.sipSubPlatMap;
    }

    MySipCatalogSubVirtualPlatCfgMap getSubVirtualPlatCfgMap(const std::string& servId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServCatalogInfoMap.find(servId);
        if (m_sipServCatalogInfoMap.end() == iter) {
            return MySipCatalogSubVirtualPlatCfgMap();
        }
        return iter->second.sipSubVirtualPlatMap;
    }

    MySipCatalogDeviceCfgMap getDeviceCfgMap(const std::string& servId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServCatalogInfoMap.find(servId);
        if (m_sipServCatalogInfoMap.end() == iter) {
            return MySipCatalogDeviceCfgMap();
        }
        return iter->second.sipDeviceMap;
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
    //                                                  读写互斥量
    boost::shared_mutex                                 m_rwMutex;

    //                                                  sip服务设备目录信息
    MySipServCatalogManage::MySipServCatalogInfoMap     m_sipServCatalogInfoMap;
};

static MySipServCatalogManage::MySipServCatalogManageObject ManageObject;

MyStatus_t MySipServCatalogManage::AddSipServCatalogInfo(const std::string& servId, const MY_COMMON::MySipCatalogInfo_dt& servCatalogInfo)
{
    return ManageObject.add(servId, servCatalogInfo);
}

MyStatus_t MySipServCatalogManage::DelSipServCatalogInfo(const std::string& servId)
{
    return ManageObject.del(servId);
}

MyStatus_t MySipServCatalogManage::HasSipServCatalogInfo(const std::string& servId)
{
    return ManageObject.has(servId);
}

MySipCatalogPlatCfgMap MySipServCatalogManage::GetSipServCatalogPlatCfgMap(const std::string& servId)
{
    return ManageObject.getPlatCfgMap(servId);
}

MySipCatalogSubPlatCfgMap MySipServCatalogManage::GetSipServCatalogSubPlatCfgMap(const std::string& servId)
{
    return ManageObject.getSubPlatCfgMap(servId);
}

MySipCatalogSubVirtualPlatCfgMap MySipServCatalogManage::GetSipServCatalogSubVirtualPlatCfgMap(const std::string& servId)
{
    return ManageObject.getSubVirtualPlatCfgMap(servId);
}

MySipCatalogDeviceCfgMap MySipServCatalogManage::GetSipServCatalogDeviceCfgMap(const std::string& servId)
{
    return ManageObject.getDeviceCfgMap(servId);
}

MyStatus_t MySipServCatalogManage::UpdateSipServCatalogSN(const std::string& servId, const std::string& sn)
{
    return ManageObject.uptServSN(servId, sn);
}

}; // namespace MY_MANAGER