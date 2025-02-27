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
    void add(MySipServer::SmtPtr sipServPtr) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_sipServPtr) {
            m_sipServPtr = sipServPtr;
            return;
        }
        LOG(ERROR) << "addSipServ failed. server already exist.";
    }

    void del() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr != m_sipServPtr) {
            m_sipServPtr.reset();
            return;
        }
        LOG(ERROR) << "delSipServ failed. server not exist.";
    }

    MyStatus_t get(MySipServer::SmtWkPtr& sipServWkPtr) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_sipServPtr) {
            return MyStatus_t::FAILED;
        }
        sipServWkPtr = m_sipServPtr;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t IsValid() {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        return (nullptr != m_sipServPtr ? MyStatus_t::SUCCESS : MyStatus_t::FAILED);
    }

    MyStatus_t start() {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        
        m_sipServPtr->run();
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t stop() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        m_sipServPtr->shutdown();
        m_sipServPtr.reset();
        return MyStatus_t::SUCCESS;
    }

private:
    boost::shared_mutex     m_rwMutex;
    MySipServer::SmtPtr     m_sipServPtr;
};

static MyServManage::MyServManageObject ManageObject;

MyStatus_t MyServManage::Init()
{
    // 获取sip服务配置
    MySipServAddrCfg_dt sipServAddrCfg;
    if (MyStatus_t::SUCCESS != MySystemConfig::GetSipServAddrCfg(sipServAddrCfg)) {
        return MyStatus_t::FAILED;
    }

    // 获取sip服务线程内存配置
    MySipEvThdMemCfg_dt sipEvThdMemCfg;
    if (MyStatus_t::SUCCESS != MySystemConfig::GetSipEvThdMemCfg(sipEvThdMemCfg)) {
        return MyStatus_t::FAILED;
    }

    // 创建sip服务对象
    MySipServer::SmtPtr sipServPtr = std::make_shared<MySipServer>();
    if (MyStatus_t::SUCCESS == sipServPtr->init(sipServAddrCfg, sipEvThdMemCfg)) {
        ManageObject.add(sipServPtr);
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

MyStatus_t MyServManage::IsSipServValid()
{   
    return ManageObject.IsValid();
}

MyStatus_t MyServManage::GetSipServer(MySipServer::SmtWkPtr& sipServWkPtr)
{
    return ManageObject.get(sipServWkPtr);
}

MyStatus_t MyServManage::GetSipServId(std::string& id)
{
    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != ManageObject.get(sipServWkPtr)) {
        return MyStatus_t::FAILED;
    }
    
    MySipServAddrCfg_dt sipServAddrCfg;
    if (MyStatus_t::SUCCESS != sipServWkPtr.lock()->getSipServAddrCfg(sipServAddrCfg)) {
        return MyStatus_t::FAILED;
    }

    id = sipServAddrCfg.id;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServManage::GetSipServAddrCfg(MySipServAddrCfg_dt& sipServAddrCfg)
{
    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != ManageObject.get(sipServWkPtr)) {
        return MyStatus_t::FAILED;
    }
    
    if (MyStatus_t::SUCCESS != sipServWkPtr.lock()->getSipServAddrCfg(sipServAddrCfg)) {
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServManage::GetSipServUdpTp(MySipTransportPtrAddr udpTpPtrAddr)
{
    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != ManageObject.get(sipServWkPtr)) {
        return MyStatus_t::FAILED;
    }
    
    if (MyStatus_t::SUCCESS != sipServWkPtr.lock()->getSipServUdpTp(udpTpPtrAddr)) {
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServManage::GetSipServRegUdpTp(MySipTransportPtrAddr udpTpPtrAddr)
{
    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != ManageObject.get(sipServWkPtr)) {
        return MyStatus_t::FAILED;
    }
    
    if (MyStatus_t::SUCCESS != sipServWkPtr.lock()->getSipServRegUdpTp(udpTpPtrAddr)) {
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServManage::GetSipServTcpTp(MySipTransportPtrAddr tcpTpPtrAddr, const std::string& remoteIp, uint16_t remotePort)
{   
    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != ManageObject.get(sipServWkPtr)) {
        return MyStatus_t::FAILED;
    }
    
    if (MyStatus_t::SUCCESS != sipServWkPtr.lock()->getSipServTcpTp(tcpTpPtrAddr, remoteIp, remotePort)) {
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServManage::GetSipServRegTcpTp(MySipTransportPtrAddr tcpTpPtrAddr, const std::string& remoteIp, uint16_t remotePort)
{
    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != ManageObject.get(sipServWkPtr)) {
        return MyStatus_t::FAILED;
    }
    
    if (MyStatus_t::SUCCESS != sipServWkPtr.lock()->getSipServTcpTp(tcpTpPtrAddr, remoteIp, remotePort)) {
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_MANAGER