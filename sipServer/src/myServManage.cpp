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

    MyStatus_t get(const std::string& servId, MySipServer::SmtWkPtr& sipServWkPtr) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServMap.find(servId);
        if (m_sipServMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        sipServWkPtr = iter->second;
        return MyStatus_t::SUCCESS;
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
            MySipServAddrCfg_dt curServAddrCfg;
            if (MyStatus_t::SUCCESS != iter.second->getSipServAddrCfg(curServAddrCfg)) {
                return MyStatus_t::FAILED;
            }
            
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
    boost::shared_mutex                         m_rwMutex;
    // key = server id, value = sip server      
    std::map<std::string, MySipServer::SmtPtr>  m_sipServMap;
};

static MyServManage::MyServManageObject ManageObject;

MyStatus_t MyServManage::Init()
{
    // 获取所有sip服务
    MySipServAddrMap sipServAddrCfgMap;
    if (MyStatus_t::SUCCESS != MySystemConfig::GetSipServAddrCfgMap(sipServAddrCfgMap)) {
        return MyStatus_t::FAILED;
    }

    // 获取sip服务线程内存配置
    MySipEvThdMemCfg_dt sipEvThdMemCfg;
    if (MyStatus_t::SUCCESS != MySystemConfig::GetSipEvThdMemCfg(sipEvThdMemCfg)) {
        return MyStatus_t::FAILED;
    }

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

MyStatus_t MyServManage::GetSipServer(const std::string& servId, MySipServer::SmtWkPtr& sipServWkPtr)
{
    return ManageObject.get(servId, sipServWkPtr);
}

MyStatus_t MyServManage::GetSipServAddrCfg(const std::string& servId, MySipServAddrCfg_dt& sipServAddrCfg)
{
    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != ManageObject.get(servId, sipServWkPtr)) {
        return MyStatus_t::FAILED;
    }
    
    if (MyStatus_t::SUCCESS != sipServWkPtr.lock()->getSipServAddrCfg(sipServAddrCfg)) {
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServManage::HasSipServer(const std::string& servId)
{   
    return ManageObject.has(servId);
}

}; // namespace MY_MANAGER