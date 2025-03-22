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
    void addSipServ(MySipServer::SmtPtr servPtr) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_sipServPtr) {
            m_sipServPtr = servPtr;
            return;
        }
        LOG(ERROR) << "addSipServ failed. server already exist.";
    }

    void delSipServ() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr != m_sipServPtr) {
            m_sipServPtr.reset();
            return;
        }
        LOG(ERROR) << "delSipServ failed. server not exist.";
    }

    MyStatus_t getSipServ(MySipServer::SmtWkPtr& servWkPtr) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_sipServPtr) {
            return MyStatus_t::FAILED;
        }
        servWkPtr = m_sipServPtr;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t IsSipServValid() {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        return (nullptr != m_sipServPtr ? MyStatus_t::SUCCESS : MyStatus_t::FAILED);
    }

public:
    void addHttpServ(MyHttpServer::SmtPtr servPtr) {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_httpServPtr) {
            m_httpServPtr = servPtr;
            return;
        }
        LOG(ERROR) << "addHttpServ failed. server already exist.";
    }

    void delHttpServ() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr != m_httpServPtr) {
            m_httpServPtr.reset();
            return;
        }
        LOG(ERROR) << "delHttpServ failed. server not exist.";
    }

    MyStatus_t getHttpServ(MyHttpServer::SmtWkPtr& servWkPtr) {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);

        if (nullptr == m_httpServPtr) {
            return MyStatus_t::FAILED;
        }
        servWkPtr = m_httpServPtr;
        return MyStatus_t::SUCCESS;
    }

    MyStatus_t IsHttpServValid() {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        return (nullptr != m_httpServPtr ? MyStatus_t::SUCCESS : MyStatus_t::FAILED);
    }

public:
    MyStatus_t start() {
        boost::shared_lock<boost::shared_mutex> lock(m_rwMutex);
        
        m_sipServPtr->run();
        m_httpServPtr->run();

        return MyStatus_t::SUCCESS;
    }

    MyStatus_t stop() {
        boost::unique_lock<boost::shared_mutex> lock(m_rwMutex);

        m_sipServPtr->shutdown();
        m_sipServPtr.reset();

        m_httpServPtr->shutdown();
        m_httpServPtr.reset();

        return MyStatus_t::SUCCESS;
    }

private:
    boost::shared_mutex     m_rwMutex;
    MySipServer::SmtPtr     m_sipServPtr;
    MyHttpServer::SmtPtr    m_httpServPtr;
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
        ManageObject.addSipServ(sipServPtr);
    }

    // 获取http服务配置
    MyHttpServAddrCfg_dt httpServAddrCfg;
    if (MyStatus_t::SUCCESS != MySystemConfig::GetHttpServAddrCfg(httpServAddrCfg)) {
        return MyStatus_t::FAILED;
    }

    // 创建http服务对象
    MyHttpServer::SmtPtr httpServPtr = std::make_shared<MyHttpServer>();
    if (MyStatus_t::SUCCESS == httpServPtr->init(httpServAddrCfg)) {
        ManageObject.addHttpServ(httpServPtr);
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
    return ManageObject.IsSipServValid();
}

MyStatus_t MyServManage::GetSipServer(MySipServer::SmtWkPtr& servWkPtr)
{
    return ManageObject.getSipServ(servWkPtr);
}

MyStatus_t MyServManage::GetSipServId(std::string& id)
{
    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != ManageObject.getSipServ(sipServWkPtr)) {
        return MyStatus_t::FAILED;
    }
    
    MySipServAddrCfg_dt sipServAddrCfg;
    if (MyStatus_t::SUCCESS != sipServWkPtr.lock()->getServAddrCfg(sipServAddrCfg)) {
        return MyStatus_t::FAILED;
    }

    id = sipServAddrCfg.id;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServManage::GetSipServAddrCfg(MySipServAddrCfg_dt& servAddrCfg)
{
    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != ManageObject.getSipServ(sipServWkPtr)) {
        return MyStatus_t::FAILED;
    }
    
    if (MyStatus_t::SUCCESS != sipServWkPtr.lock()->getServAddrCfg(servAddrCfg)) {
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServManage::GetSipServPool(MY_COMMON::MySipPoolPtrAddr poolAddr)
{
    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != ManageObject.getSipServ(sipServWkPtr)) {
        return MyStatus_t::FAILED;
    }

    if (MyStatus_t::SUCCESS != sipServWkPtr.lock()->getServPool(poolAddr)) {
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServManage::GetSipServUdpTp(MySipTransportPtrAddr udpTpPtrAddr)
{
    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != ManageObject.getSipServ(sipServWkPtr)) {
        return MyStatus_t::FAILED;
    }
    
    if (MyStatus_t::SUCCESS != sipServWkPtr.lock()->getServUdpTp(udpTpPtrAddr)) {
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServManage::GetSipServRegUdpTp(MySipTransportPtrAddr udpTpPtrAddr)
{
    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != ManageObject.getSipServ(sipServWkPtr)) {
        return MyStatus_t::FAILED;
    }
    
    if (MyStatus_t::SUCCESS != sipServWkPtr.lock()->getServRegUdpTp(udpTpPtrAddr)) {
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServManage::GetSipServTcpTp(MySipTransportPtrAddr tcpTpPtrAddr, const std::string& remoteIp, uint16_t remotePort)
{   
    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != ManageObject.getSipServ(sipServWkPtr)) {
        return MyStatus_t::FAILED;
    }
    
    if (MyStatus_t::SUCCESS != sipServWkPtr.lock()->getServTcpTp(tcpTpPtrAddr, remoteIp, remotePort)) {
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServManage::GetSipServRegTcpTp(MySipTransportPtrAddr tcpTpPtrAddr, const std::string& remoteIp, uint16_t remotePort)
{
    MySipServer::SmtWkPtr sipServWkPtr;
    if (MyStatus_t::SUCCESS != ManageObject.getSipServ(sipServWkPtr)) {
        return MyStatus_t::FAILED;
    }
    
    if (MyStatus_t::SUCCESS != sipServWkPtr.lock()->getServTcpTp(tcpTpPtrAddr, remoteIp, remotePort)) {
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServManage::IsHttpServValid()
{
    return ManageObject.IsHttpServValid();
}

MyStatus_t MyServManage::GetHttpServer(MyHttpServer::SmtWkPtr& servWkPtr)
{
    return ManageObject.getHttpServ(servWkPtr);
}

MyStatus_t MyServManage::GetHttpServId(std::string& id)
{
    MyHttpServer::SmtWkPtr servWkPtr;
    if (MyStatus_t::SUCCESS != ManageObject.getHttpServ(servWkPtr)) {
        return MyStatus_t::FAILED;
    }
    
    MyHttpServAddrCfg_dt servAddrCfg;
    if (MyStatus_t::SUCCESS != servWkPtr.lock()->getServAddrCfg(servAddrCfg)) {
        return MyStatus_t::FAILED;
    }

    id = servAddrCfg.id;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServManage::GetHttpServAddrCfg(MyHttpServAddrCfg_dt& servAddrCfg)
{
    MyHttpServer::SmtWkPtr servWkPtr;
    if (MyStatus_t::SUCCESS != ManageObject.getHttpServ(servWkPtr)) {
        return MyStatus_t::FAILED;
    }
    
    if (MyStatus_t::SUCCESS != servWkPtr.lock()->getServAddrCfg(servAddrCfg)) {
        return MyStatus_t::FAILED;
    }

    return MyStatus_t::SUCCESS;
}

}; // namespace MY_MANAGER