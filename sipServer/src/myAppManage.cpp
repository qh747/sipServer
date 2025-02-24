#include <map>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <boost/thread/shared_mutex.hpp>
#include "common/myConfigDef.h"
#include "envir/mySystemConfg.h"
#include "manager/myServManage.h"
#include "manager/myAppManage.h"
using namespace MY_COMMON;
using MY_ENVIR::MySystemConfig;

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
            std::string appId;
            if (MyStatus_t::SUCCESS != sipMsgProcAppSmtPtr->getId(appId)) {
                LOG(ERROR) << "addSipMsgProcApp failed, servId: " << servId << "appId: " << appId << " get app id failed.";
            }
            LOG(ERROR) << "addSipMsgProcApp failed, servId: " << servId << "appId: " << appId << " already exist.";
        }
    }

    void delSipMsgProcApp(const std::string& servId) {
        if (MyStatus_t::SUCCESS != this->del(servId, m_sipMsgProcAppSmtPtrMap)) {
            LOG(ERROR) << "delSipMsgProcApp failed, servId: " << servId << " not exist.";
        }
    }

    MyStatus_t getSipMsgProcApp(const std::string& servId, MySipMsgProcApp::SmtWkPtr& appSmtWkPtr) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipMsgProcAppSmtPtrMap.find(servId);
        if (m_sipMsgProcAppSmtPtrMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        appSmtWkPtr = iter->second;
        return MyStatus_t::SUCCESS;
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

public:
    void addSipRegApp(const std::string& servId, MySipRegApp::SmtPtr sipRegAppSmtPtr) {
        if (MyStatus_t::SUCCESS != this->add(servId, sipRegAppSmtPtr, m_sipRegAppSmtPtrMap)) {
            std::string appId;
            if (MyStatus_t::SUCCESS != sipRegAppSmtPtr->getId(appId)) {
                LOG(ERROR) << "addSipRegApp failed, servId: " << servId << "appId: " << appId << " get app id failed.";
            }

            LOG(ERROR) << "addSipRegApp failed, servId: " << servId << "appId: " << appId << " already exist.";
        }
    }

    void delSipRegApp(const std::string& servId) {
        if (MyStatus_t::SUCCESS != this->del(servId, m_sipRegAppSmtPtrMap)) {
            LOG(ERROR) << "delSipRegApp failed, servId: " << servId << " not exist.";
        }
    }

    MyStatus_t getSipRegApp(const std::string& servId, MySipRegApp::SmtWkPtr& appSmtWkPtr) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipRegAppSmtPtrMap.find(servId);
        if (m_sipRegAppSmtPtrMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        appSmtWkPtr = iter->second;
        return MyStatus_t::SUCCESS;
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

public:
    void addSipCatalogApp(const std::string& servId, MySipCatalogApp::SmtPtr sipCatalogAppSmtPtr) {
        if (MyStatus_t::SUCCESS != this->add(servId, sipCatalogAppSmtPtr, m_sipCatalogAppSmtPtrMap)) {
            std::string appId;
            if (MyStatus_t::SUCCESS != sipCatalogAppSmtPtr->getId(appId)) {
                LOG(ERROR) << "addSipCatalogApp failed, servId: " << servId << "appId: " << appId << " get app id failed.";
            }

            LOG(ERROR) << "addSipCatalogApp failed, servId: " << servId << "appId: " << appId << " already exist.";
        }
    }

    void delSipCatalogApp(const std::string& servId) {
        if (MyStatus_t::SUCCESS != this->del(servId, m_sipCatalogAppSmtPtrMap)) {
            LOG(ERROR) << "delSipCatalogApp failed, servId: " << servId << " not exist.";
        }
    }

    MyStatus_t getSipCatalogApp(const std::string& servId, MySipCatalogApp::SmtWkPtr& appSmtWkPtr) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipCatalogAppSmtPtrMap.find(servId);
        if (m_sipCatalogAppSmtPtrMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        appSmtWkPtr = iter->second;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t hasSipCatalogApp(const std::string& servId) {
        return this->has(servId, m_sipCatalogAppSmtPtrMap);
    }

    MyStatus_t startSipCatalogApp() {
        return this->start(m_sipCatalogAppSmtPtrMap);
    }

    MyStatus_t stopSipCatalogApp() {
        return this->stop(m_sipCatalogAppSmtPtrMap);
    }

public:
    void addSipInviteApp(const std::string& servId, MySipInviteApp::SmtPtr sipInviteAppSmtPtr) {
        if (MyStatus_t::SUCCESS != this->add(servId, sipInviteAppSmtPtr, m_sipInviteAppSmtPtrMap)) {
            std::string appId;
            if (MyStatus_t::SUCCESS != sipInviteAppSmtPtr->getId(appId)) {
                LOG(ERROR) << "addSipInviteApp failed, servId: " << servId << "appId: " << appId << " get app id failed.";
            }

            LOG(ERROR) << "addSipInviteApp failed, servId: " << servId << "appId: " << appId << " already exist.";
        }
    }

    void delSipInviteApp(const std::string& servId) {
        if (MyStatus_t::SUCCESS != this->del(servId, m_sipInviteAppSmtPtrMap)) {
            LOG(ERROR) << "delSipInviteApp failed, servId: " << servId << " not exist.";
        }
    }

    MyStatus_t getSipInviteApp(const std::string& servId, MySipInviteApp::SmtWkPtr& appSmtWkPtr) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        auto iter = m_sipInviteAppSmtPtrMap.find(servId);
        if (m_sipInviteAppSmtPtrMap.end() == iter) {
            return MyStatus_t::FAILED;
        }
        appSmtWkPtr = iter->second;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t hasSipInviteApp(const std::string& servId) {
        return this->has(servId, m_sipInviteAppSmtPtrMap);
    }

    MyStatus_t startSipInviteApp() {
        return this->start(m_sipInviteAppSmtPtrMap);
    }

    MyStatus_t stopSipInviteApp() {
        return this->stop(m_sipInviteAppSmtPtrMap);
    }

private:
    boost::shared_mutex                             m_rwMutex;
    // key = server id, value = sip msg proc app
    std::map<std::string, MySipMsgProcApp::SmtPtr>  m_sipMsgProcAppSmtPtrMap;
    // key = server id, value = sip reg app
    std::map<std::string, MySipRegApp::SmtPtr>      m_sipRegAppSmtPtrMap;
    // key = server id, value = sip catalog app
    std::map<std::string, MySipCatalogApp::SmtPtr>  m_sipCatalogAppSmtPtrMap;
    // key = server id, value = sip invite app
    std::map<std::string, MySipInviteApp::SmtPtr>   m_sipInviteAppSmtPtrMap;
};

static MyAppManage::MyAppManageObject ManageObject;

MyStatus_t MyAppManage::Init()
{
    // 获取sip服务配置
    MySipServAddrMap serAddrMap;
    if (MyStatus_t::SUCCESS != MySystemConfig::GetSipServAddrCfgMap(serAddrMap)) {
        LOG(ERROR) << "Init sip app manager failed. get sip serv addr cfg map failed.";
        return MyStatus_t::FAILED;
    }

    unsigned int idx = 1;
    for (const auto& pair : serAddrMap) {
        if (MyStatus_t::SUCCESS != MyServManage::HasSipServer(pair.second.id)) {
            LOG(WARNING) << "Init sip app failed. sip server not exist. servId: " << pair.second.id;
            continue;
        }

        // 初始化sip消息处理应用
        {
            std::string appId = "sipMsgProcApp_" + std::to_string(idx);

            MySipMsgProcApp::SmtPtr sipMsgProcAppSmtPtr = std::make_shared<MySipMsgProcApp>();
            if (MyStatus_t::SUCCESS != sipMsgProcAppSmtPtr->init(pair.second.id, appId, PJSIP_MOD_PRIORITY_APPLICATION)) {
                LOG(WARNING) << "Init sip app failed. sip msg proc app init failed.";
            }
            else {
                ManageObject.addSipMsgProcApp(pair.second.id, sipMsgProcAppSmtPtr);
            }
        }
        
        // 初始化sip注册应用
        {
            std::string appId = "sipRegApp_" + std::to_string(idx);

            MySipRegApp::SmtPtr sipRegAppSmtPtr = std::make_shared<MySipRegApp>();
            if (MyStatus_t::SUCCESS != sipRegAppSmtPtr->init(pair.second.id, appId, appId, PJSIP_MOD_PRIORITY_APPLICATION)) {
                LOG(WARNING) << "Init sip app failed. sip reg app init failed.";
            }
            else{
                ManageObject.addSipRegApp(pair.second.id, sipRegAppSmtPtr);
            }
        }
        
        // 初始化sip目录应用
        {
            std::string appId = "sipCatalogApp_" + std::to_string(idx);

            MySipCatalogApp::SmtPtr sipCatalogAppSmtPtr = std::make_shared<MySipCatalogApp>();
            if (MyStatus_t::SUCCESS != sipCatalogAppSmtPtr->init(pair.second.id, appId, appId, PJSIP_MOD_PRIORITY_APPLICATION)) {
                LOG(WARNING) << "Init sip app failed. sip catalog app init failed.";
            }
            else{
                ManageObject.addSipCatalogApp(pair.second.id, sipCatalogAppSmtPtr);
            }
        }
        
        // 初始化sip会话邀请应用
        {
            std::string appId = "sipInviteApp_" + std::to_string(idx);

            MySipInviteApp::SmtPtr sipInviteAppSmtPtr = std::make_shared<MySipInviteApp>();
            if (MyStatus_t::SUCCESS != sipInviteAppSmtPtr->init(pair.second.id, appId, PJSIP_MOD_PRIORITY_APPLICATION)) {
                LOG(WARNING) << "Init sip app failed. sip invite app init failed.";
            }
            else{
                ManageObject.addSipInviteApp(pair.second.id, sipInviteAppSmtPtr);
            }
        }
        
        ++idx;
    }
    return MyStatus_t::SUCCESS;
}                    

MyStatus_t MyAppManage::Run()
{
    // 启动sip消息处理应用
    ManageObject.startSipMsgProcApp();

    // 启动sip注册应用
    ManageObject.startSipRegApp();

    // 启动sip目录应用
    ManageObject.startSipCatalogApp();

    // 启动sip会话邀请应用
    ManageObject.startSipInviteApp();

    return MyStatus_t::SUCCESS;
}

MyStatus_t MyAppManage::Shutdown()
{
    // 停止sip消息处理应用
    ManageObject.stopSipMsgProcApp();

    // 停止sip注册应用
    ManageObject.stopSipRegApp();

    // 停止sip目录应用
    ManageObject.stopSipCatalogApp();

    // 停止sip会话邀请应用
    ManageObject.stopSipInviteApp();

    return MyStatus_t::SUCCESS;
}

MyStatus_t MyAppManage::GetSipRegApp(const std::string& servId, MySipRegApp::SmtWkPtr& appSmtWkPtr)
{
    return ManageObject.getSipRegApp(servId, appSmtWkPtr);
}

MyStatus_t MyAppManage::GetSipMsgProcApp(const std::string& servId, MySipMsgProcApp::SmtWkPtr& appSmtWkPtr)
{
    return ManageObject.getSipMsgProcApp(servId, appSmtWkPtr);
}

MyStatus_t MyAppManage::GetSipCatalogApp(const std::string& servId, MySipCatalogApp::SmtWkPtr& appSmtWkPtr)
{
    return ManageObject.getSipCatalogApp(servId, appSmtWkPtr);
}

MyStatus_t MyAppManage::GetSipInviteApp(const std::string& servId, MySipInviteApp::SmtWkPtr& appSmtWkPtr)
{
    return ManageObject.getSipInviteApp(servId, appSmtWkPtr);
}

}; // namespace MY_MANAGER