#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <boost/thread/shared_mutex.hpp>
#include "envir/mySystemConfg.h"
#include "manager/myServManage.h"
#include "manager/myAppManage.h"
using namespace MY_COMMON;
using MY_ENVIR::MySystemConfig;

namespace MY_MANAGER {

/**
 * 应用管理对象类
 */
class MyAppManage::MyAppManageObject
{
public:
    MyStatus_t addSipMsgProcApp(const MySipMsgProcApp::SmtPtr& sipMsgProcAppSmtPtr) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr != m_sipMsgProcAppSmtPtr) {
            std::string appId;
            if (MyStatus_t::SUCCESS != m_sipMsgProcAppSmtPtr->getId(appId)) {
                LOG(ERROR) << "addSipMsgProcApp failed. appId: " << appId << " get app id failed.";
                return MyStatus_t::FAILED;
            }
        }

        m_sipMsgProcAppSmtPtr = sipMsgProcAppSmtPtr;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t delSipMsgProcApp() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr != m_sipMsgProcAppSmtPtr) {
            m_sipMsgProcAppSmtPtr.reset();
        }
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getSipMsgProcApp(MySipMsgProcApp::SmtWkPtr& appSmtWkPtr) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_sipMsgProcAppSmtPtr) {
            LOG(ERROR) << "getSipMsgProcApp failed, sipMsgProcAppSmtPtr is invalid.";
            return MyStatus_t::FAILED;
        }
        appSmtWkPtr = m_sipMsgProcAppSmtPtr;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t hasSipMsgProcApp() {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        return (nullptr != m_sipMsgProcAppSmtPtr ? MyStatus_t::SUCCESS : MyStatus_t::FAILED);
    }

    MyStatus_t startSipMsgProcApp() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_sipMsgProcAppSmtPtr) {
            LOG(ERROR) << "startSipMsgProcApp failed, sipMsgProcAppSmtPtr is invalid.";
            return MyStatus_t::FAILED;
        }
        return m_sipMsgProcAppSmtPtr->run();
    }

    MyStatus_t stopSipMsgProcApp() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_sipMsgProcAppSmtPtr) {
            LOG(ERROR) << "stopSipMsgProcApp failed, sipMsgProcAppSmtPtr is invalid.";
            return MyStatus_t::FAILED;
        }

        m_sipMsgProcAppSmtPtr->shutdown();
        m_sipMsgProcAppSmtPtr.reset();
        return MyStatus_t::SUCCESS;
    }

public:
    MyStatus_t addSipRegApp(const MySipRegApp::SmtPtr& sipRegAppSmtPtr) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr != m_sipRegAppSmtPtr) {
            LOG(ERROR) << "addSipRegApp failed. app exists.";
            return MyStatus_t::FAILED;
        }   

        m_sipRegAppSmtPtr = sipRegAppSmtPtr;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t delSipRegApp() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr != m_sipRegAppSmtPtr) {
            m_sipRegAppSmtPtr.reset();
        }
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getSipRegApp(MySipRegApp::SmtWkPtr& appSmtWkPtr) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_sipRegAppSmtPtr) {
            LOG(ERROR) << "getSipRegApp failed, sipRegAppSmtPtr is invalid.";
            return MyStatus_t::FAILED;
        }
        appSmtWkPtr = m_sipRegAppSmtPtr;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t hasSipRegApp() {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        return (nullptr != m_sipRegAppSmtPtr ? MyStatus_t::SUCCESS : MyStatus_t::FAILED);
    }

    MyStatus_t startSipRegApp() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_sipRegAppSmtPtr) {
            LOG(ERROR) << "startSipRegApp failed, sipRegAppSmtPtr is invalid.";
            return MyStatus_t::FAILED;
        }
        return m_sipRegAppSmtPtr->run();
    }

    MyStatus_t stopSipRegApp() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_sipRegAppSmtPtr) {
            LOG(ERROR) << "stopSipRegApp failed, sipRegAppSmtPtr is invalid.";
            return MyStatus_t::FAILED;
        }

        m_sipRegAppSmtPtr->shutdown();
        m_sipRegAppSmtPtr.reset();
        return MyStatus_t::SUCCESS;
    }

public:
    MyStatus_t addSipCatalogApp(const MySipCatalogApp::SmtPtr& sipCatalogAppSmtPtr) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr != m_sipCatalogAppSmtPtr) {
            std::string appId;
            if (MyStatus_t::SUCCESS != m_sipCatalogAppSmtPtr->getId(appId)) {
                LOG(ERROR) << "addSipCatalogApp failed. appId: " << appId << " get app id failed.";
                return MyStatus_t::FAILED;
            }
        }

        m_sipCatalogAppSmtPtr = sipCatalogAppSmtPtr;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t delSipCatalogApp() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr != m_sipCatalogAppSmtPtr) {
            m_sipCatalogAppSmtPtr.reset();
        }
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getSipCatalogApp(MySipCatalogApp::SmtWkPtr& appSmtWkPtr) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_sipCatalogAppSmtPtr) {
            LOG(ERROR) << "getSipCatalogApp failed, sipCatalogAppSmtPtr is invalid.";
            return MyStatus_t::FAILED;
        }
        appSmtWkPtr = m_sipCatalogAppSmtPtr;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t hasSipCatalogApp() {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        return (nullptr != m_sipCatalogAppSmtPtr ? MyStatus_t::SUCCESS : MyStatus_t::FAILED);
    }

    MyStatus_t startSipCatalogApp() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_sipCatalogAppSmtPtr) {
            LOG(ERROR) << "startSipCatalogApp failed, sipCatalogAppSmtPtr is invalid.";
            return MyStatus_t::FAILED;
        }
        return m_sipCatalogAppSmtPtr->run();
    }

    MyStatus_t stopSipCatalogApp() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_sipCatalogAppSmtPtr) {
            LOG(ERROR) << "stopSipCatalogApp failed, sipCatalogAppSmtPtr is invalid.";
            return MyStatus_t::FAILED;
        }

        m_sipCatalogAppSmtPtr->shutdown();
        m_sipCatalogAppSmtPtr.reset();
        return MyStatus_t::SUCCESS;
    }
    
public:
    MyStatus_t addSipInviteApp(const MySipInviteApp::SmtPtr& sipInviteAppSmtPtr) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr != m_sipInviteAppSmtPtr) {
            std::string appId;
            if (MyStatus_t::SUCCESS != sipInviteAppSmtPtr->getId(appId)) {
                LOG(ERROR) << "addSipInviteApp failed. appId: " << appId << " get app id failed.";
                return MyStatus_t::FAILED;
            }
        }

        m_sipInviteAppSmtPtr = sipInviteAppSmtPtr;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t delSipInviteApp() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr != m_sipInviteAppSmtPtr) {
            m_sipInviteAppSmtPtr.reset();
        }
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t getSipInviteApp(MySipInviteApp::SmtWkPtr& appSmtWkPtr) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_sipInviteAppSmtPtr) {
            LOG(ERROR) << "getSipInviteApp failed, sipInviteAppSmtPtr is invalid.";
            return MyStatus_t::FAILED;
        }
        appSmtWkPtr = m_sipInviteAppSmtPtr;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t hasSipInviteApp() {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        return (nullptr != m_sipInviteAppSmtPtr ? MyStatus_t::SUCCESS : MyStatus_t::FAILED);
    }

    MyStatus_t startSipInviteApp() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_sipInviteAppSmtPtr) {
            LOG(ERROR) << "startSipInviteApp failed, sipInviteAppSmtPtr is invalid.";
            return MyStatus_t::FAILED;
        }
        return m_sipInviteAppSmtPtr->run();
    }

    MyStatus_t stopSipInviteApp() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_sipInviteAppSmtPtr) {
            LOG(ERROR) << "stopSipInviteApp failed, sipInviteAppSmtPtr is invalid.";
            return MyStatus_t::FAILED;
        }

        m_sipInviteAppSmtPtr->shutdown();
        m_sipInviteAppSmtPtr.reset();
        return MyStatus_t::SUCCESS;
    }

private:
    boost::shared_mutex        m_rwMutex;
    MySipMsgProcApp::SmtPtr    m_sipMsgProcAppSmtPtr;
    MySipRegApp::SmtPtr        m_sipRegAppSmtPtr;
    MySipCatalogApp::SmtPtr    m_sipCatalogAppSmtPtr;
    MySipInviteApp::SmtPtr     m_sipInviteAppSmtPtr;

};

static MyAppManage::MyAppManageObject ManageObject;

MyStatus_t MyAppManage::Init()
{
    // 获取sip服务配置
    MySipServAddrCfg_dt serAddrCfg;
    if (MyStatus_t::SUCCESS != MySystemConfig::GetSipServAddrCfg(serAddrCfg)) {
        LOG(ERROR) << "Init sip app manager failed. get sip serv addr cfg failed.";
        return MyStatus_t::FAILED;
    }

    if (MyStatus_t::SUCCESS != MyServManage::IsSipServValid()) {
        LOG(WARNING) << "Init sip app failed. sip server not exist. servId: " << serAddrCfg.id;
        return MyStatus_t::FAILED;
    }

    // 初始化sip消息处理应用
    {
        MySipMsgProcApp::SmtPtr sipMsgProcAppSmtPtr = std::make_shared<MySipMsgProcApp>();
        if (MyStatus_t::SUCCESS != sipMsgProcAppSmtPtr->init(serAddrCfg.id, "sipMsgProcApp_1", PJSIP_MOD_PRIORITY_APPLICATION)) {
            LOG(WARNING) << "Init sip app failed. sip msg proc app init failed.";
        }
        else {
            ManageObject.addSipMsgProcApp(sipMsgProcAppSmtPtr);
        }
    }
    
    // 初始化sip注册应用
    {
        MySipRegApp::SmtPtr sipRegAppSmtPtr = std::make_shared<MySipRegApp>();
        if (MyStatus_t::SUCCESS != sipRegAppSmtPtr->init(serAddrCfg.id, "sipRegApp_1", "sipRegApp_1", PJSIP_MOD_PRIORITY_APPLICATION)) {
            LOG(WARNING) << "Init sip app failed. sip reg app init failed.";
        }
        else{
            ManageObject.addSipRegApp(sipRegAppSmtPtr);
        }
    }
    
    // 初始化sip目录应用
    {
        MySipCatalogApp::SmtPtr sipCatalogAppSmtPtr = std::make_shared<MySipCatalogApp>();
        if (MyStatus_t::SUCCESS != sipCatalogAppSmtPtr->init(serAddrCfg.id, "sipCatalogApp_1", "sipCatalogApp_1", PJSIP_MOD_PRIORITY_APPLICATION)) {
            LOG(WARNING) << "Init sip app failed. sip catalog app init failed.";
        }
        else{
            ManageObject.addSipCatalogApp(sipCatalogAppSmtPtr);
        }
    }
    
    // 初始化sip会话邀请应用
    {
        MySipInviteApp::SmtPtr sipInviteAppSmtPtr = std::make_shared<MySipInviteApp>();
        if (MyStatus_t::SUCCESS != sipInviteAppSmtPtr->init(serAddrCfg.id, "sipInviteApp_1", PJSIP_MOD_PRIORITY_APPLICATION)) {
            LOG(WARNING) << "Init sip app failed. sip invite app init failed.";
        }
        else{
            ManageObject.addSipInviteApp(sipInviteAppSmtPtr);
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

MyStatus_t MyAppManage::GetSipRegApp(MySipRegApp::SmtWkPtr& appSmtWkPtr)
{
    return ManageObject.getSipRegApp(appSmtWkPtr);
}

MyStatus_t MyAppManage::GetSipMsgProcApp(MySipMsgProcApp::SmtWkPtr& appSmtWkPtr)
{
    return ManageObject.getSipMsgProcApp(appSmtWkPtr);
}

MyStatus_t MyAppManage::GetSipCatalogApp(MySipCatalogApp::SmtWkPtr& appSmtWkPtr)
{
    return ManageObject.getSipCatalogApp(appSmtWkPtr);
}

MyStatus_t MyAppManage::GetSipInviteApp(MySipInviteApp::SmtWkPtr& appSmtWkPtr)
{
    return ManageObject.getSipInviteApp(appSmtWkPtr);
}

}; // namespace MY_MANAGER