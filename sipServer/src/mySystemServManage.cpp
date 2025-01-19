#include <map>
#include <memory>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <boost/thread/shared_mutex.hpp>
#include "server/mySipServer.h"
#include "utils/mySipServerHelper.h"
#include "envir/mySystemConfg.h"
#include "envir/mySystemServManage.h"
using MY_COMMON::MyStatus_t;
using MY_COMMON::MySipServAddrCfg_dt;
using MY_SERVER::MySipServer;
using MY_UTILS::MySipServerHelper;

namespace MY_ENVIR {

/**
 * 服务管理对象类
 */
class MySystemServManage::MySystemServManageObject
{
public:
    void addSipServ(const std::string& servId, MySystemServManage::SipServSmtPtr sipServPtr) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (m_sipServMap.end() == m_sipServMap.find(servId)) {
            m_sipServMap.insert(std::make_pair(servId, sipServPtr));
            return;
        }
        LOG(ERROR) << "addSipServ failed, servId: " << servId << " already exist.";
    }

    void delSipServ(const std::string& servId) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServMap.find(servId);  
        if (iter != m_sipServMap.end()) {
            m_sipServMap.erase(iter);
            return;
        }
        LOG(ERROR) << "delSipServ failed, servId: " << servId << " not exist.";
    }

    MySystemServManage::SipServSmtWkPtr getSipServ(const std::string& servId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServMap.find(servId);
        if (iter != m_sipServMap.end()) {
            return iter->second->getSipServer();
        }
        return MySystemServManage::SipServSmtWkPtr();
    }

    MyStatus_t hasSipServ(const std::string& servId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServMap.find(servId);
        if (iter != m_sipServMap.end()) {
            return MyStatus_t::SUCCESS;
        }
        return MyStatus_t::FAILED;
    }

    MyStatus_t startSipServ() {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        
        for (auto& iter : m_sipServMap) {
            iter.second->run();
        }
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t stopSipServ() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        for (auto& iter : m_sipServMap) {
            iter.second->shutdown();
        }

        m_sipServMap.clear();
        return MyStatus_t::SUCCESS;
    }

private:
    boost::shared_mutex                                         m_rwMutex;
    // key = server id, value = sip server
    std::map<std::string, MySystemServManage::SipServSmtPtr>    m_sipServMap;
};

static MySystemServManage::MySystemServManageObject ManageObject;

MyStatus_t MySystemServManage::Init()
{
    // 初始化sip服务
    const MySipServAddrCfg_dt& sipServAddrCfg = MySystemConfig::GetSipServAddrCfg();

    if (MyStatus_t::SUCCESS != ManageObject.hasSipServ(sipServAddrCfg.id)) {
        SipServSmtPtr sipServPtr = std::make_shared<MySipServer>();

        if (MyStatus_t::SUCCESS == sipServPtr->init(sipServAddrCfg, MySystemConfig::GetSipEvThdMemCfg())) {
            ManageObject.addSipServ(sipServAddrCfg.id, sipServPtr);
        }
        else {
            LOG(ERROR) << "Init sip server failed. " << MySipServerHelper::GetSipServInfo(sipServAddrCfg);
        }
    }

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySystemServManage::Run()
{
    // 启动sip服务
    ManageObject.startSipServ();

    return MyStatus_t::SUCCESS;
}
    
MyStatus_t MySystemServManage::Shutdown()
{
    // 关闭sip服务
    ManageObject.stopSipServ();

    return MyStatus_t::SUCCESS;
}

MySystemServManage::SipServSmtWkPtr MySystemServManage::GetSipServer(const std::string& servId)
{
    return ManageObject.getSipServ(servId);
}

}; // namespace MY_ENVIR