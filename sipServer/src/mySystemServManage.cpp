#include <cstdbool>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <boost/thread/shared_mutex.hpp>
#include "server/mySipServer.h"
#include "utils/myJsonHelper.h"
#include "utils/mySipServerHelper.h"
#include "envir/mySystemConfg.h"
#include "envir/mySystemServManage.h"
using MY_COMMON::MyStatus_t;
using MY_COMMON::MySipServAddrCfg_dt;
using MY_COMMON::MySipRegUpServCfg_dt;
using MY_COMMON::MySipRegLowServCfg_dt;
using MY_SERVER::MySipServer;
using MY_UTILS::MyJsonHelper;
using MY_UTILS::MySipServerHelper;

namespace MY_ENVIR {

/**
 * 服务管理对象类
 */
class MySystemServManage::MySystemServManageObject
{
public:
    typedef std::map<std::string, MySystemServManage::SipServSmtPtr>     SipServMap;
    typedef std::map<std::string, std::vector<MySipRegUpServCfg_dt>>     RegUpServBindMap;
    typedef std::map<std::string, std::vector<MySipRegLowServCfg_dt>>    RegLowServBindMap;

public:
    void addSipServ(const std::string& servId, MySystemServManage::SipServSmtPtr sipServPtr, const RegUpServBindMap& upRegServMap, const RegLowServBindMap& lowRegServMap) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (m_sipServMap.end() == m_sipServMap.find(servId)) {
            m_sipServMap.insert(std::make_pair(servId, sipServPtr));

            m_sipServRegUpBindMap  = upRegServMap;
            m_sipServRegLowBindMap = lowRegServMap;
            return;
        }
        LOG(ERROR) << "addSipServ failed, servId: " << servId << " already exist.";
    }

    void delSipServ(const std::string& servId) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipServMap.find(servId);  
        if (iter != m_sipServMap.end()) {
            m_sipServMap.erase(iter);

            auto upRegIter = m_sipServRegUpBindMap.find(servId);
            if (upRegIter != m_sipServRegUpBindMap.end()) {
                m_sipServRegUpBindMap.erase(upRegIter);
            }

            auto lowRegIter = m_sipServRegLowBindMap.find(servId);
            if (lowRegIter != m_sipServRegLowBindMap.end()) {
                m_sipServRegLowBindMap.erase(lowRegIter);
            }
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
        m_sipServRegUpBindMap.clear();
        m_sipServRegLowBindMap.clear();

        return MyStatus_t::SUCCESS;
    }

private:
    boost::shared_mutex                                 m_rwMutex;
    // key = server id, value = sip server          
    SipServMap                                          m_sipServMap;

    // key = server id, value = reg up server id    
    RegUpServBindMap                                    m_sipServRegUpBindMap;

    // key = server id, value = reg low server id
    RegLowServBindMap                                   m_sipServRegLowBindMap;
};

static MySystemServManage::MySystemServManageObject ManageObject;

MyStatus_t MySystemServManage::Init()
{
    // 初始化sip服务
    const MySipServAddrCfg_dt& sipServAddrCfg = MySystemConfig::GetSipServAddrCfg();

    if (MyStatus_t::SUCCESS != ManageObject.hasSipServ(sipServAddrCfg.id)) {
        
        MySystemServManageObject::RegUpServBindMap regUpBindMap;
        const MyJsonHelper::SipRegUpServJsonMap& regUpServMap = MySystemConfig::GetSipUpRegServCfgMap();
        if (!regUpServMap.empty()) {
            std::vector<MY_COMMON::MySipRegUpServCfg_dt> regUpBindVec;

            for (const auto& iter : regUpServMap) {
                regUpBindVec.push_back(iter.second);
            }

            regUpBindMap.insert(std::make_pair(sipServAddrCfg.id, regUpBindVec));
        }

        MySystemServManageObject::RegLowServBindMap regLowBindMap;
        const MyJsonHelper::SipRegLowServJsonMap& regLowServMap = MySystemConfig::GetSipLowRegServCfgMap();
        if (!regLowServMap.empty()) {
            std::vector<MY_COMMON::MySipRegLowServCfg_dt> regLowBindVec;

            for (const auto& iter : regLowServMap) {
                regLowBindVec.push_back(iter.second);
            }

            regLowBindMap.insert(std::make_pair(sipServAddrCfg.id, regLowBindVec));
        }
        
        SipServSmtPtr sipServPtr = std::make_shared<MySipServer>();
        if (MyStatus_t::SUCCESS == sipServPtr->init(sipServAddrCfg, MySystemConfig::GetSipEvThdMemCfg())) {
            ManageObject.addSipServ(sipServAddrCfg.id, sipServPtr, regUpBindMap, regLowBindMap);
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