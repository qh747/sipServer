#include <map>
#include <boost/thread/shared_mutex.hpp>
#include "manager/mySipSessionManage.h"
using namespace MY_COMMON;

namespace MY_MANAGER {

/**
 * sip会话管理对象类
 */
class MySipSessionManage::MySipSessionManageObject
{
public:
    friend class MySipSessionManageView;

public:
    // key = device id, value = sip up reg serv info ptr
    typedef std::map<std::string, MySipSessionInfo_dt> MySipSessionInfoMap;

public:
    MyStatus_t add(const std::string& devId, const MySipSessionInfo_dt& sessionInfo) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (m_SipSessionInfoMap.end() != m_SipSessionInfoMap.find(devId)) {
            return MyStatus_t::FAILED;
        }

        m_SipSessionInfoMap.insert(std::make_pair(devId, sessionInfo));
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t upt(const std::string& devId, const MySipSessionInfo_dt& sessionInfo) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        m_SipSessionInfoMap[devId] = sessionInfo;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t get(const std::string& devId, MySipSessionInfo_dt& sessionInfo) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_SipSessionInfoMap.find(devId);
        if (m_SipSessionInfoMap.end() == iter) {
            return MyStatus_t::FAILED;
        }

        sessionInfo = iter->second;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t del(const std::string& devId) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        m_SipSessionInfoMap.erase(devId);
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t has(const std::string& devId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        return (m_SipSessionInfoMap.end() != m_SipSessionInfoMap.find(devId) ? MyStatus_t::SUCCESS : MyStatus_t::FAILED);
    }

private:
    //                  读写互斥量
    boost::shared_mutex m_rwMutex;

    //                  sip会话管理map
    MySipSessionInfoMap m_SipSessionInfoMap;
};

static MySipSessionManage::MySipSessionManageObject SipSessionManageObject;

MyStatus_t MySipSessionManage::AddSipSessionInfo(const std::string& deviceId, const MySipSessionInfo_dt& sessionInfo)
{
    return SipSessionManageObject.add(deviceId, sessionInfo);
}

MyStatus_t MySipSessionManage::DelSipSessionInfo(const std::string& deviceId)
{
    return SipSessionManageObject.del(deviceId);
}

MyStatus_t MySipSessionManage::UptSipSessionInfo(const std::string& deviceId, const MySipSessionInfo_dt& sessionInfo)
{
    return SipSessionManageObject.upt(deviceId, sessionInfo);
}

MyStatus_t MySipSessionManageView::HasSipSessionInfo(const std::string& deviceId)
{
    return SipSessionManageObject.has(deviceId);
}

MyStatus_t MySipSessionManageView::GetSipSessionInfo(const std::string& deviceId, MySipSessionInfo_dt& sessionInfo)
{
    return SipSessionManageObject.get(deviceId, sessionInfo);
}

} // namespace MY_MANAGER