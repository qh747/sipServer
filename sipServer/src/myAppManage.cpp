#include <map>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <boost/thread/shared_mutex.hpp>
#include "common/myConfigDef.h"
#include "utils/mySipServerHelper.h"
#include "envir/mySystemConfg.h"
#include "manager/myServManage.h"
#include "manager/myAppManage.h"
using namespace MY_COMMON;
using MY_ENVIR::MySystemConfig;
using MY_MANAGER::MyServManage;
using MY_UTILS::MySipServerHelper;

namespace MY_MANAGER {

/**
 * 应用管理对象类
 * @todo 将app抽象出基类，方便扩展
 */
class MyAppManage::MyAppManageObject
{
private:
    template <typename ServIdType, typename SipAppSmtPtrType, typename SipAppSmtPtrMapType>
    MyStatus_t add(const ServIdType& servId, SipAppSmtPtrType sipAppSmtPtr, SipAppSmtPtrMapType& sipAppSmtPtrMap) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (sipAppSmtPtrMap.end() == sipAppSmtPtrMap.find(servId)) {
            sipAppSmtPtrMap.insert(std::make_pair(servId, sipAppSmtPtr));
            return MyStatus_t::SUCCESS;
        }
        return MyStatus_t::FAILED;
    }

    template <typename ServIdType, typename SipAppSmtPtrMapType>
    MyStatus_t del(const ServIdType& servId, SipAppSmtPtrMapType& sipAppSmtPtrMap) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = sipAppSmtPtrMap.find(servId);
        if (sipAppSmtPtrMap.end() != iter) {
            sipAppSmtPtrMap.erase(iter);
            return MyStatus_t::SUCCESS;
        }
        return MyStatus_t::FAILED;
    }

    template <typename ServIdType, typename SipAppSmtPtrMapType>
    MyStatus_t has(const ServIdType& servId, SipAppSmtPtrMapType& sipAppSmtPtrMap) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        if (sipAppSmtPtrMap.end() == sipAppSmtPtrMap.find(servId)) {
            return MyStatus_t::FAILED;
        }
        return MyStatus_t::SUCCESS;
    }

    template <typename SipAppSmtPtrMapType>
    MyStatus_t start(SipAppSmtPtrMapType& sipAppSmtPtrMap) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        for (auto& iter : sipAppSmtPtrMap) {
            iter.second->run();
        }
        return MyStatus_t::SUCCESS;
    }

    template <typename SipAppSmtPtrMapType>
    MyStatus_t stop(SipAppSmtPtrMapType& sipAppSmtPtrMap) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        for (auto& iter : sipAppSmtPtrMap) {
            iter.second->shutdown();
        }
        sipAppSmtPtrMap.clear();
        return MyStatus_t::SUCCESS;
    }

public:
    void addSipMsgProcApp(const std::string& servId, MySipMsgProcApp::SmtPtr sipMsgProcAppSmtPtr) {
        if (MyStatus_t::SUCCESS != this->add(servId, sipMsgProcAppSmtPtr, m_sipMsgProcAppSmtPtrMap)) {
            LOG(ERROR) << "addSipMsgProcApp failed, servId: " << servId << "appId: " << sipMsgProcAppSmtPtr->getId() << " already exist.";
        }
    }

    void delSipMsgProcApp(const std::string& servId) {
        if (MyStatus_t::SUCCESS != this->del(servId, m_sipMsgProcAppSmtPtrMap)) {
            LOG(ERROR) << "delSipMsgProcApp failed, servId: " << servId << " not exist.";
        }
    }

    MySipMsgProcApp::SmtWkPtr getSipMsgProcApp(const std::string& servId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipMsgProcAppSmtPtrMap.find(servId);
        if (m_sipMsgProcAppSmtPtrMap.end() != iter) {
            return iter->second->getSipMsgProcApp();
        }
        return MySipMsgProcApp::SmtWkPtr();
    }

    MyStatus_t hasSipMsgProcApp(const std::string& servId) {
        return this->has(servId, m_sipMsgProcAppSmtPtrMap);
    }

    MyStatus_t startSipMsgProcApp() {
        return this->start(m_sipMsgProcAppSmtPtrMap);
    }

    MyStatus_t stopSipMsgProcApp() {
        return this->stop(m_sipMsgProcAppSmtPtrMap);
    }

    void addSipRegApp(const std::string& servId, MySipRegApp::SmtPtr sipRegAppSmtPtr) {
        if (MyStatus_t::SUCCESS != this->add(servId, sipRegAppSmtPtr, m_sipRegAppSmtPtrMap)) {
            LOG(ERROR) << "addSipRegApp failed, servId: " << servId << "appId: " << sipRegAppSmtPtr->getId() << " already exist.";
        }
    }

    void delSipRegApp(const std::string& servId) {
        if (MyStatus_t::SUCCESS != this->del(servId, m_sipRegAppSmtPtrMap)) {
            LOG(ERROR) << "delSipRegApp failed, servId: " << servId << " not exist.";
        }
    }

    MySipRegApp::SmtWkPtr getSipRegApp(const std::string& servId) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipRegAppSmtPtrMap.find(servId);
        if (m_sipRegAppSmtPtrMap.end() != iter) {
            return iter->second->getSipRegApp();
        }
        return MySipRegApp::SmtWkPtr();
    }

    MyStatus_t hasSipRegApp(const std::string& servId) {
        return this->has(servId, m_sipRegAppSmtPtrMap);
    }

    MyStatus_t startSipRegApp() {
        return this->start(m_sipRegAppSmtPtrMap);
    }

    MyStatus_t stopSipRegApp() {
        return this->stop(m_sipRegAppSmtPtrMap);
    }

private:
    boost::shared_mutex                             m_rwMutex;
    // key = server id, value = sip msg proc app
    std::map<std::string, MySipMsgProcApp::SmtPtr>  m_sipMsgProcAppSmtPtrMap;
    // key = server id, value = sip reg app
    std::map<std::string, MySipRegApp::SmtPtr>      m_sipRegAppSmtPtrMap;
};

static MyAppManage::MyAppManageObject ManageObject;

MyStatus_t MyAppManage::Init()
{
    // 获取sip服务配置
    MySipServAddrMap serAddrMap = MySystemConfig::GetSipServAddrCfgMap();
    for (const auto& pair : serAddrMap) {
        if (MyStatus_t::SUCCESS != MyServManage::HasSipServer(pair.second.id)) {
            LOG(WARNING) << "Init sip app failed. sip server not exist. servId: " << pair.second.id;
            continue;
        }

        // 初始化sip消息处理应用
        MySipMsgProcApp::SmtPtr sipMsgProcAppSmtPtr = std::make_shared<MySipMsgProcApp>();
        if (MyStatus_t::SUCCESS != sipMsgProcAppSmtPtr->init(pair.second.id, "sipMsgProcApp_1", PJSIP_MOD_PRIORITY_APPLICATION)) {
            LOG(WARNING) << "Init sip app failed. sip msg proc app init failed.";
        }
        else {
            ManageObject.addSipMsgProcApp(pair.second.id, sipMsgProcAppSmtPtr);
        }
        
        // 初始化sip注册应用
        MySipRegApp::SmtPtr sipRegAppSmtPtr = std::make_shared<MySipRegApp>();
        if (MyStatus_t::SUCCESS != sipRegAppSmtPtr->init(pair.second.id, "sipRegApp_1", "sipRegApp_1", PJSIP_MOD_PRIORITY_APPLICATION)) {
            LOG(WARNING) << "Init sip app failed. sip reg app init failed.";
        }
        else{
            ManageObject.addSipRegApp(pair.second.id, sipRegAppSmtPtr);
        }
    }
    return MyStatus_t::SUCCESS;
}                    

MyStatus_t MyAppManage::Run()
{
    // 启动sip消息处理应用
    ManageObject.startSipMsgProcApp();

    // 启动sip注册应用
    ManageObject.startSipRegApp();

    return MyStatus_t::SUCCESS;
}

MyStatus_t MyAppManage::Shutdown()
{
    // 停止sip消息处理应用
    ManageObject.stopSipMsgProcApp();

    // 停止sip注册应用
    ManageObject.stopSipRegApp();

    return MyStatus_t::SUCCESS;
}

MySipRegApp::SmtWkPtr MyAppManage::GetSipRegApp(const std::string& servId)
{
    return ManageObject.getSipRegApp(servId);
}

MySipMsgProcApp::SmtWkPtr MyAppManage::GetSipMsgProcApp(const std::string& servId)
{
    return ManageObject.getSipMsgProcApp(servId);
}

}; // namespace MY_MANAGER