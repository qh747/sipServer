#include <cstdbool>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <boost/thread/shared_mutex.hpp>
#include "envir/mySystemConfg.h"
#include "manager/myServManage.h"
using namespace MY_COMMON;
using MY_ENVIR::MySystemConfig;

namespace MY_MANAGER {

/**
 * 服务管理对象类
 */
class MyServManage::MyServManageObject
{
public:
    void add(const std::string& servId, MySipServer::SmtPtr sipServPtr) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (m_sipServMap.end() == m_sipServMap.find(servId)) {
            m_sipServMap.insert(std::make_pair(servId, sipServPtr));
            return;
        }
        LOG(ERROR) << "addSipServ failed, servId: " << servId << " already exist.";
    }

    void del(const std::string& servId) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServMap.find(servId);  
        if (iter != m_sipServMap.end()) {
            m_sipServMap.erase(iter);
            return;
        }
        LOG(ERROR) << "delSipServ failed, servId: " << servId << " not exist.";
    }

    MySipServer::SmtWkPtr get(const std::string& servId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServMap.find(servId);
        if (iter != m_sipServMap.end()) {
            return iter->second->getSipServer();
        }
        return MySipServer::SmtWkPtr();
    }

    MyStatus_t has(const std::string& servId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServMap.find(servId);
        if (iter != m_sipServMap.end()) {
            return MyStatus_t::SUCCESS;
        }
        return MyStatus_t::FAILED;
    }

    MyStatus_t conflict(uint16_t port, uint16_t regPort) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        for (auto& iter : m_sipServMap) {
            auto curServAddrCfg = iter.second->getSipServAddrCfg();
            if ((port == curServAddrCfg.port) || (regPort == curServAddrCfg.regPort) || 
                (port == curServAddrCfg.regPort) || (regPort == curServAddrCfg.port)) {
                return MyStatus_t::SUCCESS;
            }
        }
        return MyStatus_t::FAILED;
    }

    MyStatus_t start() {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        
        for (auto& iter : m_sipServMap) {
            iter.second->run();
        }
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t stop() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        for (auto& iter : m_sipServMap) {
            iter.second->shutdown();
        }

        m_sipServMap.clear();
        return MyStatus_t::SUCCESS;
    }

private:
    boost::shared_mutex                                 m_rwMutex;
    // key = server id, value = sip server          
    std::map<std::string, MySipServer::SmtPtr>          m_sipServMap;
};

static MyServManage::MyServManageObject ManageObject;

MyStatus_t MyServManage::Init()
{
    // 获取所有sip服务
    MySipServAddrMap sipServAddrCfgMap = MySystemConfig::GetSipServAddrCfgMap();

    // 获取sip服务线程内存配置
    MySipEvThdMemCfg_dt sipEvThdMemCfg = MySystemConfig::GetSipEvThdMemCfg();

    // 创建sip服务对象
    for (const auto& pair : sipServAddrCfgMap) {
        // 判断服务id是否冲突
        if (MyStatus_t::SUCCESS == ManageObject.has(pair.second.id)) {
            continue;
        }

        // 判断服务端口是否冲突
        if (MyStatus_t::SUCCESS == ManageObject.conflict(pair.second.port, pair.second.regPort)) {
            continue;
        }

        MySipServer::SmtPtr sipServPtr = std::make_shared<MySipServer>();
        if (MyStatus_t::SUCCESS == sipServPtr->init(pair.second, sipEvThdMemCfg)) {
            ManageObject.add(pair.second.id, sipServPtr);
        }
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServManage::Run()
{
    // 启动sip服务
    ManageObject.start();
    return MyStatus_t::SUCCESS;
}
    
MyStatus_t MyServManage::Shutdown()
{
    // 关闭sip服务
    ManageObject.stop();
    return MyStatus_t::SUCCESS;
}

MySipServer::SmtWkPtr MyServManage::GetSipServer(const std::string& servId)
{
    return ManageObject.get(servId);
}

MySipServAddrCfg_dt MyServManage::GetSipServAddrCfg(const std::string& servId)
{
    auto sipServWkPtr = ManageObject.get(servId);
    if (sipServWkPtr.expired()) {
        return MySipServAddrCfg_dt();
    }
    return sipServWkPtr.lock()->getSipServAddrCfg();
}

MySipPoolPtr MyServManage::GetSipServThdPoolPtr(const std::string& servId)
{
    auto sipServWkPtr = ManageObject.get(servId);
    if (sipServWkPtr.expired()) {
        return nullptr;
    }
    return sipServWkPtr.lock()->getSipThdPoolPtr();
}

MyStatus_t MyServManage::HasSipServer(const std::string& servId)
{   
    return ManageObject.has(servId);
}

}; // namespace MY_MANAGER