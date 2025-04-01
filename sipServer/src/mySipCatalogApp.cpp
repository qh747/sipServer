#include <thread>
#include <cstring>
#include <algorithm>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "envir/mySystemConfg.h"
#include "envir/mySystemPjsip.h"
#include "envir/mySystemThreadPool.h"
#include "utils/myXmlHelper.h"
#include "utils/mySipAppHelper.h"
#include "utils/mySipMsgHelper.h"
#include "utils/myServerHelper.h"
#include "manager/myServManage.h"
#include "manager/mySipRegManage.h"
#include "manager/mySipCatalogManage.h"
#include "app/mySipCatalogApp.h"
using namespace MY_UTILS;
using namespace MY_COMMON;
using MY_ENVIR::MySystemPjsip;
using MY_ENVIR::MySystemConfig;
using MY_ENVIR::MySystemThdPool;
using MY_MANAGER::MyServManage;
using MY_MANAGER::MySipRegManage;
using MY_MANAGER::MySipCatalogManage;
using MY_MANAGER::MySipCatalogManageView;

namespace MY_APP {

/**
 * @brief SIP Catalog App推送目录设备线程参数
 */
typedef struct {
    //                              消息sn号
    std::string                     m_sn;

    //                              消息序列号
    unsigned int                    m_cseq;

    //                              上级sip服务注册配置
    MySipRegUpServCfg_dt            m_regUpServCfg;

    //                              本级服务地址
    MySipServAddrCfg_dt             m_servAddrCfg;
    
} MySipCatalogAppPushCatalogThdParam_dt;

/**
 * @brief SIP Catalog App更新目录设备线程参数
 */
 typedef struct {
    //                              服务id
    std::string                     m_servId;

    //                              sip响应消息内容
    MySipCatalogRespMsgBody_dt      m_catalogRespMsgBody;
    
} MySipCatalogAppUptCatalogThdParam_dt;

typedef MySipCatalogAppPushCatalogThdParam_dt* MySipCatalogAppPushCatalogThdParamPtr;
typedef MySipCatalogAppUptCatalogThdParam_dt*  MySipCatalogAppUptCatalogThdParamPtr;

int MySipCatalogApp::OnPushCatalogCb(MyFuncCbParamPtr arg)
{
    // pjsip要求自定义线程进行注册才能使用
    MySystemPjsip::RegistSelfThread();

    // 内存管理, new in function: MySipCatalogApp::onRecvSipCatalogQueryReqMsg()
    auto thdParamPtr = static_cast<MySipCatalogAppPushCatalogThdParamPtr>(arg);
    std::unique_ptr<MySipCatalogAppPushCatalogThdParam_dt> thdParamMemManagePtr(thdParamPtr);

    unsigned int cseq = thdParamPtr->m_cseq;
    const std::string& sn = thdParamPtr->m_sn;
    const MySipRegUpServCfg_dt& regUpServCfg = thdParamPtr->m_regUpServCfg;
    const MySipServAddrCfg_dt& localServAddrCfg = thdParamPtr->m_servAddrCfg;

    std::string upRegServInfo;
    MyServerHelper::PrintSipUpRegServInfo(regUpServCfg, upRegServInfo);

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip catalog app push local server catalog failed. get pjsip endpoint failed. " << upRegServInfo << ".";
        return -1;
    }

    // 创建推送设备目录回调函数
    auto pushCatalogFunc = [endptPtr, &sn, &regUpServCfg, &upRegServInfo, &cseq](const MySipServAddrCfg_dt& servAddrCfg) {
        // 获取设备目录配置
        MySipCatalogPlatCfgMap platCfgMap;
        if (MyStatus_t::SUCCESS != MySipCatalogManageView::GetSipCatalogPlatCfgMap(servAddrCfg.id, platCfgMap)) {
            LOG(ERROR) << "Sip catalog app push local server catalog failed. get catalog plat cfg failed. "  << upRegServInfo << ".";
            return;
        }

        MySipCatalogSubPlatCfgMap subPlatCfgMap;
        if (MyStatus_t::SUCCESS != MySipCatalogManageView::GetSipCatalogSubPlatCfgMap(servAddrCfg.id, subPlatCfgMap)) {
            LOG(ERROR) << "Sip catalog app push local server catalog failed. get catalog sub plat cfg failed. " << upRegServInfo << ".";
            return;
        }

        // 删除无效子平台
        auto subPlatCfgMapIter = subPlatCfgMap.begin();
        while (subPlatCfgMap.end() != subPlatCfgMapIter) {
            if (platCfgMap.end() == platCfgMap.find(subPlatCfgMapIter->second.parentID)) {
                subPlatCfgMapIter = subPlatCfgMap.erase(subPlatCfgMapIter);
            }
            else {
                ++subPlatCfgMapIter;
            }
        }

        MySipCatalogSubVirtualPlatCfgMap subVirtualCfgMap;
        if (MyStatus_t::SUCCESS != MySipCatalogManageView::GetSipCatalogSubVirtualPlatCfgMap(servAddrCfg.id, subVirtualCfgMap)) {
            LOG(ERROR) << "Sip catalog app push local server catalog failed. get catalog sub virtual plat cfg failed. " << upRegServInfo << ".";
            return;
        }

        auto subVirtualCfgMapIter = subVirtualCfgMap.begin();
        while (subVirtualCfgMapIter != subVirtualCfgMap.end()) {
            // 删除无效虚拟子平台
            if (platCfgMap.end()    == platCfgMap.find(subVirtualCfgMapIter->second.parentID) &&
                subPlatCfgMap.end() == subPlatCfgMap.find(subVirtualCfgMapIter->second.parentID)) {
                subVirtualCfgMapIter = subVirtualCfgMap.erase(subVirtualCfgMapIter);
            }
            else {
                ++subVirtualCfgMapIter;
            }
        }

        MySipCatalogDeviceCfgMap deviceCfgMap;
        if (MyStatus_t::SUCCESS != MySipCatalogManageView::GetSipCatalogDeviceCfgMap(servAddrCfg.id, deviceCfgMap)) {
            LOG(ERROR) << "Sip catalog app push local server catalog failed. get catalog device cfg failed. " << upRegServInfo << ".";
            return;
        }

        auto deviceCfgMapIter = deviceCfgMap.begin();
        while (deviceCfgMapIter != deviceCfgMap.end()) {
            // 删除无效设备
            if (platCfgMap.end()       == platCfgMap.find(deviceCfgMapIter->second.parentID) &&
                subPlatCfgMap.end()    == subPlatCfgMap.find(deviceCfgMapIter->second.parentID) &&
                subVirtualCfgMap.end() == subVirtualCfgMap.find(deviceCfgMapIter->second.parentID)) {
                deviceCfgMapIter = deviceCfgMap.erase(deviceCfgMapIter);
            }
            else {
                ++deviceCfgMapIter;
            }
        }

        // 获取设备目录总数
        std::size_t sumNum = platCfgMap.size() + subPlatCfgMap.size() + subVirtualCfgMap.size() + deviceCfgMap.size();

        // sip catalog消息首部生成
        std::string sURL;
        MySipMsgHelper::GenerateSipMsgURL(regUpServCfg.upSipServRegAddrCfg.id, regUpServCfg.upSipServRegAddrCfg.ipAddr, 
                                          regUpServCfg.upSipServRegAddrCfg.port, regUpServCfg.proto, sURL);

        std::string sFromHdr;
        MySipMsgHelper::GenerateSipMsgFromHeader(servAddrCfg.id, servAddrCfg.ipAddr, sFromHdr);

        std::string sToHdr;
        MySipMsgHelper::GenerateSipMsgToHeader(regUpServCfg.upSipServRegAddrCfg.id, regUpServCfg.upSipServRegAddrCfg.ipAddr, sToHdr);

        pj_str_t sipMsgURL     = pj_str(const_cast<char*>(sURL.c_str()));
        pj_str_t sipMsgFromHdr = pj_str(const_cast<char*>(sFromHdr.c_str()));
        pj_str_t sipMsgToHdr   = pj_str(const_cast<char*>(sToHdr.c_str()));

        pjsip_method sipMethod = {PJSIP_OTHER_METHOD, {const_cast<char*>("MESSAGE"), (pj_ssize_t)strlen("MESSAGE")}};

        // 记录上级服务响应消息状态
        auto cbStatuePtr = new std::atomic<MyStatus_t>(MyStatus_t::FAILED);
        std::unique_ptr<std::atomic<MyStatus_t>> cbStatusMemManagePtr(cbStatuePtr);

        // 推送平台
        for (const auto& platCfgPair : platCfgMap) {
            // sip catalog消息创建
            MySipTxDataPtr txDataPtr = nullptr;
            if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, 
                nullptr, nullptr, cseq, nullptr, &txDataPtr)) {
                LOG(ERROR) << "Sip catalog app push local server catalog failed. create pjsip request failed. " << upRegServInfo << ".";
                return;
            }

            // 指定传输端口为注册端口
            pjsip_tpselector tpSelector;
            tpSelector.type = PJSIP_TPSELECTOR_TRANSPORT;
            tpSelector.disable_connection_reuse = false;

            MySipTransportPtr transportPtr = nullptr;
            if (MyTpProto_t::UDP == regUpServCfg.proto) {
                if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegUdpTp(&transportPtr)) {
                    LOG(ERROR) << "Sip catalog app push local server catalog failed. get sip serv reg udp tp failed. " << upRegServInfo << ".";
                    return;
                }
            }
            else {
                const auto& upRegServAddr = regUpServCfg.upSipServRegAddrCfg;
                if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegTcpTp(&transportPtr, upRegServAddr.ipAddr, upRegServAddr.port)) {
                    LOG(ERROR) << "Sip reg app register up server failed. get sip serv reg tcp tp failed. " << upRegServInfo << ".";
                    return;
                }
            }
        
            tpSelector.u.transport = transportPtr;
            pjsip_tx_data_set_transport(txDataPtr, &tpSelector);

            // sip catalog消息内容填充
            std::string msgBody;
            MyXmlHelper::GenerateSipCatalogPlatCfgMsgBody(platCfgPair.second, sn, sumNum, msgBody);

            pj_str_t type    = pj_str(const_cast<char*>("Application"));
            pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
            pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

            txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

            // sip catalog消息发送
            cbStatuePtr->store(MyStatus_t::FAILED);
            if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, cbStatuePtr, &MySipCatalogApp::OnRespPushCatalogCb)) {
                LOG(ERROR) << "Sip catalog app push local server catalog platform config failed. send pjsip request failed. "
                           << upRegServInfo << ".";
                return;
            }

            // 记录等待消息响应开始时间
            auto start = std::chrono::high_resolution_clock::now();
            while (MyStatus_t::SUCCESS != cbStatuePtr->load()) {
                // 记录当前时间
                auto end = std::chrono::high_resolution_clock::now();

                // 计算时间差
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
                if (duration.count() > 10) {
                    LOG(ERROR) << "Sip catalog app push local server catalog platform config failed. recv up serv response failed. "
                               << upRegServInfo << ".";
                    return;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        // 推送子平台
        for (const auto& subPlatCfgPair : subPlatCfgMap) {
            // sip catalog消息创建
            MySipTxDataPtr txDataPtr = nullptr;
            if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, 
                nullptr, nullptr, cseq, nullptr, &txDataPtr)) {
                LOG(ERROR) << "Sip catalog app push local server catalog failed. create pjsip request failed. " << upRegServInfo << ".";
                return;
            }

            // 指定传输端口为注册端口
            pjsip_tpselector tpSelector;
            tpSelector.type = PJSIP_TPSELECTOR_TRANSPORT;
            tpSelector.disable_connection_reuse = false;

            MySipTransportPtr transportPtr = nullptr;
            if (MyTpProto_t::UDP == regUpServCfg.proto) {
                if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegUdpTp(&transportPtr)) {
                    LOG(ERROR) << "Sip catalog app push local server catalog failed. get sip serv reg udp tp failed. " << upRegServInfo << ".";
                    return;
                }
            }
            else {
                const auto& upRegServAddr = regUpServCfg.upSipServRegAddrCfg;
                if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegTcpTp(&transportPtr, upRegServAddr.ipAddr, upRegServAddr.port)) {
                    LOG(ERROR) << "Sip reg app register up server failed. get sip serv reg tcp tp failed. " << upRegServInfo << ".";
                    return;
                }
            }
        
            tpSelector.u.transport = transportPtr;
            pjsip_tx_data_set_transport(txDataPtr, &tpSelector);

            // sip catalog消息内容填充
            std::string msgBody;
            MyXmlHelper::GenerateSipCatalogSubPlatCfgMsgBody(subPlatCfgPair.second, sn, sumNum, msgBody);

            pj_str_t type    = pj_str(const_cast<char*>("Application"));
            pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
            pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

            txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

            // sip catalog消息发送
            cbStatuePtr->store(MyStatus_t::FAILED);
            if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, cbStatuePtr, &MySipCatalogApp::OnRespPushCatalogCb)) {
                LOG(ERROR) << "Sip catalog app push local server catalog subPlatform config failed. send pjsip request failed. "
                           << upRegServInfo << ".";
                return;
            }

            // 记录等待消息响应开始时间
            auto start = std::chrono::high_resolution_clock::now();
            while (MyStatus_t::SUCCESS != cbStatuePtr->load()) {
                // 记录当前时间
                auto end = std::chrono::high_resolution_clock::now();

                // 计算时间差
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
                if (duration.count() > 10) {
                    LOG(ERROR) << "Sip catalog app push local server subPlatform platform config failed. recv up serv response failed. "
                               << upRegServInfo << ".";
                    return;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        // 推送虚拟子平台
        for (const auto& subVirtualPlatCfgPair : subVirtualCfgMap) {
            // sip catalog消息创建
            MySipTxDataPtr txDataPtr = nullptr;
            if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, 
                nullptr, nullptr, cseq, nullptr, &txDataPtr)) {
                LOG(ERROR) << "Sip catalog app push local server catalog failed. create pjsip request failed. " << upRegServInfo << ".";
                return;
            }

            // 指定传输端口为注册端口
            pjsip_tpselector tpSelector;
            tpSelector.type = PJSIP_TPSELECTOR_TRANSPORT;
            tpSelector.disable_connection_reuse = false;

            MySipTransportPtr transportPtr = nullptr;
            if (MyTpProto_t::UDP == regUpServCfg.proto) {
                if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegUdpTp(&transportPtr)) {
                    LOG(ERROR) << "Sip catalog app push local server catalog failed. get sip serv reg udp tp failed. " << upRegServInfo << ".";
                    return;
                }
            }
            else {
                const auto& upRegServAddr = regUpServCfg.upSipServRegAddrCfg;
                if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegTcpTp(&transportPtr, upRegServAddr.ipAddr, upRegServAddr.port)) {
                    LOG(ERROR) << "Sip reg app register up server failed. get sip serv reg tcp tp failed. " << upRegServInfo << ".";
                    return;
                }
            }
        
            tpSelector.u.transport = transportPtr;
            pjsip_tx_data_set_transport(txDataPtr, &tpSelector);

            // sip catalog消息内容填充
            std::string msgBody;
            MyXmlHelper::GenerateSipCatalogSubVirtualPlatCfgMsgBody(subVirtualPlatCfgPair.second, sn, sumNum, msgBody);

            pj_str_t type    = pj_str(const_cast<char*>("Application"));
            pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
            pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

            txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

            // sip catalog消息发送
            cbStatuePtr->store(MyStatus_t::FAILED);
            if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, cbStatuePtr, &MySipCatalogApp::OnRespPushCatalogCb)) {
                LOG(ERROR) << "Sip catalog app push local server catalog subVirtualPlatform config failed. send pjsip request failed. "
                           << upRegServInfo << ".";
                return;
            }

            // 记录等待消息响应开始时间
            auto start = std::chrono::high_resolution_clock::now();
            while (MyStatus_t::SUCCESS != cbStatuePtr->load()) {
                // 记录当前时间
                auto end = std::chrono::high_resolution_clock::now();

                // 计算时间差
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
                if (duration.count() > 10) {
                    LOG(ERROR) << "Sip catalog app push local server subVirtualPlatform platform config failed. recv up serv response failed. "
                               << upRegServInfo << ".";
                    return;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        // 推送设备
        for (const auto& deviceCfgPair : deviceCfgMap) {
            // sip catalog消息创建
            MySipTxDataPtr txDataPtr = nullptr;
            if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, 
                nullptr, nullptr, cseq, nullptr, &txDataPtr)) {
                LOG(ERROR) << "Sip catalog app push local server catalog failed. create pjsip request failed. " << upRegServInfo << ".";
                return;
            }

            // 指定传输端口为注册端口
            pjsip_tpselector tpSelector;
            tpSelector.type = PJSIP_TPSELECTOR_TRANSPORT;
            tpSelector.disable_connection_reuse = false;

            MySipTransportPtr transportPtr = nullptr;
            if (MyTpProto_t::UDP == regUpServCfg.proto) {
                if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegUdpTp(&transportPtr)) {
                    LOG(ERROR) << "Sip catalog app push local server catalog failed. get sip serv reg udp tp failed. " << upRegServInfo << ".";
                    return;
                }
            }
            else {
                const auto& upRegServAddr = regUpServCfg.upSipServRegAddrCfg;
                if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegTcpTp(&transportPtr, upRegServAddr.ipAddr, upRegServAddr.port)) {
                    LOG(ERROR) << "Sip reg app register up server failed. get sip serv reg tcp tp failed. " << upRegServInfo << ".";
                    return;
                }
            }
        
            tpSelector.u.transport = transportPtr;
            pjsip_tx_data_set_transport(txDataPtr, &tpSelector);

            // sip catalog消息内容填充
            std::string msgBody;
            MyXmlHelper::GenerateSipCatalogDeviceCfgMsgBody(deviceCfgPair.second, sn, sumNum, msgBody);

            pj_str_t type    = pj_str(const_cast<char*>("Application"));
            pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
            pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

            txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

            // sip catalog消息发送
            cbStatuePtr->store(MyStatus_t::FAILED);
            if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, cbStatuePtr, &MySipCatalogApp::OnRespPushCatalogCb)) {
                LOG(ERROR) << "Sip catalog app push local server catalog device config failed. send pjsip request failed. " << upRegServInfo << ".";
                return;
            }

            // 记录等待消息响应开始时间
            auto start = std::chrono::high_resolution_clock::now();
            while (MyStatus_t::SUCCESS != cbStatuePtr->load()) {
                // 记录当前时间
                auto end = std::chrono::high_resolution_clock::now();

                // 计算时间差
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
                if (duration.count() > 10) {
                    LOG(ERROR) << "Sip catalog app push local server device platform config failed. recv up serv response failed. "
                               << upRegServInfo << ".";
                    return;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    };

    // 推送本级设备目录
    pushCatalogFunc(localServAddrCfg);

    // 获取下级sip服务信息
    MySipRegLowServCfgMap lowRegCfgMap;
    if (MyStatus_t::SUCCESS == MySipRegManage::GetSipRegLowServCfgMap(lowRegCfgMap)) {
        // 推送下级设备目录
        for (const auto& lowRegCfgPair : lowRegCfgMap) {
            MySipServAddrCfg_dt lowRegServAddr;
            lowRegServAddr.id     = lowRegCfgPair.first;
            lowRegServAddr.ipAddr = lowRegCfgPair.second.lowSipServRegAddrCfg.ipAddr;
            lowRegServAddr.port   = lowRegCfgPair.second.lowSipServRegAddrCfg.port;
            lowRegServAddr.name   = lowRegCfgPair.second.lowSipServRegAddrCfg.name;
            lowRegServAddr.domain = lowRegCfgPair.second.lowSipServRegAddrCfg.domain;

            pushCatalogFunc(lowRegServAddr);
        }
    }

    return 0;
}

void MySipCatalogApp::OnRespPushCatalogCb(MY_COMMON::MyFuncCbParamPtr arg, MY_COMMON::MySipEvPtr ev)
{
    MySipStatusPtr cbStatuePtr =  static_cast<MySipStatusPtr>(arg);

    if (200 != ev->body.rx_msg.rdata->msg_info.msg->line.status.code) {
        LOG(ERROR) << "Sip catalog app push local server catalog failed. recv up serv error response.";
        cbStatuePtr->store(MyStatus_t::FAILED);
        return;
    }
    cbStatuePtr->store(MyStatus_t::SUCCESS);
}

int MySipCatalogApp::OnUpdateCatalogCb(MyFuncCbParamPtr arg)
{
    // pjsip要求自定义线程进行注册才能使用
    MySystemPjsip::RegistSelfThread();

    auto thdParamPtr = static_cast<MySipCatalogAppUptCatalogThdParamPtr>(arg);
    if (nullptr == thdParamPtr) {
        LOG(ERROR) << "Sip catalog app update lower server catalog failed. thd param is null.";
        return -1;
    }

    const std::string&                servId             = thdParamPtr->m_servId;
    const MySipCatalogRespMsgBody_dt& catalogRespMsgBody = thdParamPtr->m_catalogRespMsgBody;

    // new in function: MySipCatalogApp::onRecvSipCatalogResponseReqMsg()
    std::unique_ptr<MySipCatalogAppUptCatalogThdParam_dt> memManagePtr(thdParamPtr);

    // 获取需要更新的下级服务设备目录信息
    MySipCatalogPlatCfgMap uptLowPlatCfgMap;
    {
        MySipCatalogPlatCfgMap oldLowPlatCfgMap;
        MySipCatalogManageView::GetSipCatalogPlatCfgMap(catalogRespMsgBody.deviceId, oldLowPlatCfgMap);

        if (!catalogRespMsgBody.platCfgMap.empty()) {
            for (const auto& newLowPlatCfgPair : catalogRespMsgBody.platCfgMap) {
                auto iter = oldLowPlatCfgMap.find(newLowPlatCfgPair.first);
                if (oldLowPlatCfgMap.end() == iter) {
                    uptLowPlatCfgMap.insert(newLowPlatCfgPair);
                }
                else if (newLowPlatCfgPair.second != iter->second) {
                    uptLowPlatCfgMap.insert(newLowPlatCfgPair);
                }
            }
        }
    }

    MySipCatalogSubPlatCfgMap uptLowSubPlatCfgMap;
    {
        MySipCatalogSubPlatCfgMap oldLowSubPlatCfgMap;
        MySipCatalogManageView::GetSipCatalogSubPlatCfgMap(catalogRespMsgBody.deviceId, oldLowSubPlatCfgMap);

        if (!catalogRespMsgBody.subPlatCfgMap.empty()) {
            for (const auto& newLowSubPlatCfgPair : catalogRespMsgBody.subPlatCfgMap) {
                const auto& oldLowSubPlatCfgIter = oldLowSubPlatCfgMap.find(newLowSubPlatCfgPair.first);
                if (oldLowSubPlatCfgMap.end() == oldLowSubPlatCfgIter) {
                    uptLowSubPlatCfgMap.insert(newLowSubPlatCfgPair);
                }
                else if (newLowSubPlatCfgPair.second != oldLowSubPlatCfgIter->second) {
                    uptLowSubPlatCfgMap.insert(newLowSubPlatCfgPair);
                }
            }
        }
    }

    MySipCatalogSubVirtualPlatCfgMap uptLowSubVirtualPlatCfgMap;
    {
        MySipCatalogSubVirtualPlatCfgMap oldLowSubVirtualPlatCfgMap;
        MySipCatalogManageView::GetSipCatalogSubVirtualPlatCfgMap(catalogRespMsgBody.deviceId, oldLowSubVirtualPlatCfgMap);

        if (!catalogRespMsgBody.subVirtualPlatCfgMap.empty()) {
            for (const auto& newLowSubVirtualPlatCfgPair : catalogRespMsgBody.subVirtualPlatCfgMap) {
                const auto& oldLowSubVirtualPlatCfgIter = oldLowSubVirtualPlatCfgMap.find(newLowSubVirtualPlatCfgPair.first);
                if (oldLowSubVirtualPlatCfgMap.end() == oldLowSubVirtualPlatCfgIter) {
                    uptLowSubVirtualPlatCfgMap.insert(newLowSubVirtualPlatCfgPair);
                }
                else if (newLowSubVirtualPlatCfgPair.second != oldLowSubVirtualPlatCfgIter->second) {
                    uptLowSubVirtualPlatCfgMap.insert(newLowSubVirtualPlatCfgPair);
                }
            }
        }
    }
    
    MySipCatalogDeviceCfgMap uptLowDeviceCfgMap;
    {
        MySipCatalogDeviceCfgMap oldLowDeviceCfgMap;
        MySipCatalogManageView::GetSipCatalogDeviceCfgMap(catalogRespMsgBody.deviceId, oldLowDeviceCfgMap);

        if (!catalogRespMsgBody.deviceCfgMap.empty()) {
            for (const auto& newLowDeviceCfgPair : catalogRespMsgBody.deviceCfgMap) {
                const auto& oldLowDeviceCfgIter = oldLowDeviceCfgMap.find(newLowDeviceCfgPair.first);
                if (oldLowDeviceCfgMap.end() == oldLowDeviceCfgIter) {
                    uptLowDeviceCfgMap.insert(newLowDeviceCfgPair);
                }
                else if (newLowDeviceCfgPair.second != oldLowDeviceCfgIter->second) {
                    uptLowDeviceCfgMap.insert(newLowDeviceCfgPair);
                }
            }
        }
    }

    MySipCatalogInfo_dt sipCatalogInfo;
    sipCatalogInfo.sn                   = catalogRespMsgBody.sn;
    sipCatalogInfo.sipPlatMap           = uptLowPlatCfgMap;
    sipCatalogInfo.sipSubPlatMap        = uptLowSubPlatCfgMap;
    sipCatalogInfo.sipSubVirtualPlatMap = uptLowSubVirtualPlatCfgMap;
    sipCatalogInfo.sipDeviceMap         = uptLowDeviceCfgMap;

    // 更新本级设备目录信息
    if (MyStatus_t::SUCCESS != MySipCatalogManageView::HasSipCatalogInfo(catalogRespMsgBody.deviceId)) {
        MySipCatalogManage::AddSipCatalogInfo(catalogRespMsgBody.deviceId, sipCatalogInfo);
    }
    else {
        MySipCatalogManage::UptSipCatalogInfo(catalogRespMsgBody.deviceId, sipCatalogInfo);
    }

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip catalog app update catalog to up server failed. invalid endpoint. local serv id: " << servId << ".";
        return -1;
    }

    // 获取本地sip服务地址配置
    MySipServAddrCfg_dt servAddrCfg;
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServAddrCfg(servAddrCfg)) {
        LOG(ERROR) << "Sip catalog app update catalog to up server failed. get sip serv addr cfg failed. local serv id: " << servId << ".";
        return -1;
    }

    // 查询请求过本级设备目录信息的上级服务(用于向上级服务发送更新)
    MySipRegServAddrMap upServAddrMap;
    MySipCatalogManageView::GetSipQueryInfo(upServAddrMap);

    // 获取上级服务地址为空时，直接返回
    if (upServAddrMap.empty()) {
        return 0;
    }

    // 记录上级服务响应消息状态
    auto cbStatuePtr = new std::atomic<MyStatus_t>(MyStatus_t::FAILED);
    std::unique_ptr<std::atomic<MyStatus_t>> cbStatusMemManagePtr(cbStatuePtr);

    for (const auto& upServAddrCfgPair: upServAddrMap) {
        std::string sn;
        if (MyStatus_t::SUCCESS != MySipCatalogManageView::GetSipCatalogSN(upServAddrCfgPair.second.id, sn)) {
            LOG(ERROR) << "Sip catalog app update catalog to up server failed. get catalog sn failed. local serv id: " << servId << ".";
            return -1;
        }

        std::string upRegServInfo;
        MyServerHelper::PrintSipRegServInfo(upServAddrCfgPair.second, upRegServInfo);

        MyTpProto_t proto = MyTpProto_t::UDP;
        MySipRegManage::GetSipRegUpServProto(upServAddrCfgPair.second.id, proto);

        // 获取设备目录总数
        std::size_t sumNum = uptLowPlatCfgMap.size() + uptLowSubPlatCfgMap.size() + uptLowSubVirtualPlatCfgMap.size() + uptLowDeviceCfgMap.size();

        // sip catalog消息首部生成
        std::string sURL;
        MySipMsgHelper::GenerateSipMsgURL(upServAddrCfgPair.second.id, upServAddrCfgPair.second.ipAddr, 
            upServAddrCfgPair.second.port, proto, sURL);

        std::string sFromHdr;
        MySipMsgHelper::GenerateSipMsgFromHeader(servAddrCfg.id, servAddrCfg.ipAddr, sFromHdr);

        std::string sToHdr;
        MySipMsgHelper::GenerateSipMsgToHeader(upServAddrCfgPair.second.id, upServAddrCfgPair.second.ipAddr, sToHdr);

        pj_str_t sipMsgURL     = pj_str(const_cast<char*>(sURL.c_str()));
        pj_str_t sipMsgFromHdr = pj_str(const_cast<char*>(sFromHdr.c_str()));
        pj_str_t sipMsgToHdr   = pj_str(const_cast<char*>(sToHdr.c_str()));

        pjsip_method sipMethod = {PJSIP_OTHER_METHOD, {const_cast<char*>("MESSAGE"), (pj_ssize_t)strlen("MESSAGE")}};

        // 推送平台
        for (const auto& platCfgPair : uptLowPlatCfgMap) {
            // sip catalog消息创建
            MySipTxDataPtr txDataPtr = nullptr;
            if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, nullptr,
                nullptr, -1, nullptr, &txDataPtr)) {
                LOG(ERROR) << "Sip catalog app update catalog to up server failed. create pjsip request failed. " << upRegServInfo << ".";
                return -1;
            }

            // 指定传输端口为注册端口
            pjsip_tpselector tpSelector;
            tpSelector.type = PJSIP_TPSELECTOR_TRANSPORT;
            tpSelector.disable_connection_reuse = false;

            MySipTransportPtr transportPtr = nullptr;
            if (MyTpProto_t::UDP == proto) {
                if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegUdpTp(&transportPtr)) {
                    LOG(ERROR) << "Sip catalog app update catalog to up server failed. get sip serv reg udp tp failed. " << upRegServInfo << ".";
                    return -1;
                }
            }
            else {
                const auto& upRegServAddr = upServAddrCfgPair.second;
                if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegTcpTp(&transportPtr, upRegServAddr.ipAddr, upRegServAddr.port)) {
                    LOG(ERROR) << "Sip reg app register up server failed. get sip serv reg tcp tp failed. " << upRegServInfo << ".";
                    return -1;
                }
            }
        
            tpSelector.u.transport = transportPtr;
            pjsip_tx_data_set_transport(txDataPtr, &tpSelector);

            // sip catalog消息内容填充
            std::string msgBody;
            MyXmlHelper::GenerateSipCatalogPlatCfgMsgBody(platCfgPair.second, sn, sumNum, msgBody);

            pj_str_t type    = pj_str(const_cast<char*>("Application"));
            pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
            pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

            txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

            // sip catalog消息发送
            cbStatuePtr->store(MyStatus_t::FAILED);
            if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, cbStatuePtr, &MySipCatalogApp::OnRespUpdateCatalogCb)) {
                LOG(ERROR) << "Sip catalog app update catalog plat cfg to up server failed. send pjsip request failed. " << upRegServInfo << ".";
                return -1;
            }

            // 记录等待消息响应开始时间
            auto start = std::chrono::high_resolution_clock::now();
            while (MyStatus_t::SUCCESS != cbStatuePtr->load()) {
                // 记录当前时间
                auto end = std::chrono::high_resolution_clock::now();

                // 计算时间差
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
                if (duration.count() > 5) {
                    LOG(ERROR) << "Sip catalog app update catalog plat cfg to up server failed. recv up serv response failed. " << upRegServInfo << ".";
                    return -1;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        // 推送子平台
        for (const auto& subPlatCfgPair : uptLowSubPlatCfgMap) {
            // sip catalog消息创建
            MySipTxDataPtr txDataPtr = nullptr;
            if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, nullptr,
                nullptr, -1, nullptr, &txDataPtr)) {
                LOG(ERROR) << "Sip catalog app update catalog to up server failed. create pjsip request failed. " << upRegServInfo << ".";
                return -1;
            }

            // 指定传输端口为注册端口
            pjsip_tpselector tpSelector;
            tpSelector.type = PJSIP_TPSELECTOR_TRANSPORT;
            tpSelector.disable_connection_reuse = false;

            MySipTransportPtr transportPtr = nullptr;
            if (MyTpProto_t::UDP == proto) {
                if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegUdpTp(&transportPtr)) {
                    LOG(ERROR) << "Sip catalog app update catalog to up server failed. get sip serv reg udp tp failed. " << upRegServInfo << ".";
                    return -1;
                }
            }
            else {
                const auto& upRegServAddr = upServAddrCfgPair.second;
                if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegTcpTp(&transportPtr, upRegServAddr.ipAddr, upRegServAddr.port)) {
                    LOG(ERROR) << "Sip reg app register up server failed. get sip serv reg tcp tp failed. " << upRegServInfo << ".";
                    return -1;
                }
            }
        
            tpSelector.u.transport = transportPtr;
            pjsip_tx_data_set_transport(txDataPtr, &tpSelector);

            // sip catalog消息内容填充
            std::string msgBody;
            MyXmlHelper::GenerateSipCatalogSubPlatCfgMsgBody(subPlatCfgPair.second, sn, sumNum, msgBody);

            pj_str_t type    = pj_str(const_cast<char*>("Application"));
            pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
            pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

            txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

            // sip catalog消息发送
            cbStatuePtr->store(MyStatus_t::FAILED);
            if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, cbStatuePtr, &MySipCatalogApp::OnRespUpdateCatalogCb)) {
                LOG(ERROR) << "Sip catalog app update catalog sub plat cfg to up server failed. send pjsip request failed. "
                           << upRegServInfo << ".";
                return -1;
            }

            // 记录等待消息响应开始时间
            auto start = std::chrono::high_resolution_clock::now();
            while (MyStatus_t::SUCCESS != cbStatuePtr->load()) {
                // 记录当前时间
                auto end = std::chrono::high_resolution_clock::now();

                // 计算时间差
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
                if (duration.count() > 5) {
                    LOG(ERROR) << "Sip catalog app update catalog sub plat cfg to up server failed. recv up serv response failed. "
                               << upRegServInfo << ".";
                    return -1;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        // 推送虚拟子平台
        for (const auto& subVirtualPlatCfgPair : uptLowSubVirtualPlatCfgMap) {
            // sip catalog消息创建
            MySipTxDataPtr txDataPtr = nullptr;
            if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr,
                nullptr, nullptr, -1, nullptr, &txDataPtr)) {
                LOG(ERROR) << "Sip catalog app update catalog to up server failed. create pjsip request failed. " << upRegServInfo << ".";
                return -1;
            }

            // 指定传输端口为注册端口
            pjsip_tpselector tpSelector;
            tpSelector.type = PJSIP_TPSELECTOR_TRANSPORT;
            tpSelector.disable_connection_reuse = false;

            MySipTransportPtr transportPtr = nullptr;
            if (MyTpProto_t::UDP == proto) {
                if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegUdpTp(&transportPtr)) {
                    LOG(ERROR) << "Sip catalog app update catalog to up server failed. get sip serv reg udp tp failed. " << upRegServInfo << ".";
                    return -1;
                }
            }
            else {
                const auto& upRegServAddr = upServAddrCfgPair.second;
                if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegTcpTp(&transportPtr, upRegServAddr.ipAddr, upRegServAddr.port)) {
                    LOG(ERROR) << "Sip reg app register up server failed. get sip serv reg tcp tp failed. " << upRegServInfo << ".";
                    return -1;
                }
            }
        
            tpSelector.u.transport = transportPtr;
            pjsip_tx_data_set_transport(txDataPtr, &tpSelector);

            // sip catalog消息内容填充
            std::string msgBody;
            MyXmlHelper::GenerateSipCatalogSubVirtualPlatCfgMsgBody(subVirtualPlatCfgPair.second, sn, sumNum, msgBody);

            pj_str_t type    = pj_str(const_cast<char*>("Application"));
            pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
            pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

            txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

            // sip catalog消息发送
            cbStatuePtr->store(MyStatus_t::FAILED);
            if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, cbStatuePtr, &MySipCatalogApp::OnRespUpdateCatalogCb)) {
                LOG(ERROR) << "Sip catalog app update catalog sub plat virtual cfg to up server failed. send pjsip request failed. " 
                           << upRegServInfo << ".";
                return -1;
            }

            // 记录等待消息响应开始时间
            auto start = std::chrono::high_resolution_clock::now();
            while (MyStatus_t::SUCCESS != cbStatuePtr->load()) {
                // 记录当前时间
                auto end = std::chrono::high_resolution_clock::now();

                // 计算时间差
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
                if (duration.count() > 5) {
                    LOG(ERROR) << "Sip catalog app update catalog sub virtual plat cfg to up server failed. recv up serv response failed. "
                               << upRegServInfo << ".";
                    return -1;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        // 推送设备
        for (const auto& deviceCfgPair : uptLowDeviceCfgMap) {
            // sip catalog消息创建
            MySipTxDataPtr txDataPtr = nullptr;
            if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, nullptr,
                nullptr, -1, nullptr, &txDataPtr)) {
                LOG(ERROR) << "Sip catalog app update catalog to up server failed. create pjsip request failed. " << upRegServInfo << ".";
                return -1;
            }

            // 指定传输端口为注册端口
            pjsip_tpselector tpSelector;
            tpSelector.type = PJSIP_TPSELECTOR_TRANSPORT;
            tpSelector.disable_connection_reuse = false;

            MySipTransportPtr transportPtr = nullptr;
            if (MyTpProto_t::UDP == proto) {
                if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegUdpTp(&transportPtr)) {
                    LOG(ERROR) << "Sip catalog app update catalog to up server failed. get sip serv reg udp tp failed. " 
                               << upRegServInfo << ".";
                    return -1;
                }
            }
            else {
                const auto& upRegServAddr = upServAddrCfgPair.second;
                if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegTcpTp(&transportPtr, upRegServAddr.ipAddr, upRegServAddr.port)) {
                    LOG(ERROR) << "Sip reg app register up server failed. get sip serv reg tcp tp failed. " << upRegServInfo << ".";
                    return -1;
                }
            }
        
            tpSelector.u.transport = transportPtr;
            pjsip_tx_data_set_transport(txDataPtr, &tpSelector);

            // sip catalog消息内容填充
            std::string msgBody;
            MyXmlHelper::GenerateSipCatalogDeviceCfgMsgBody(deviceCfgPair.second, sn, sumNum, msgBody);

            pj_str_t type    = pj_str(const_cast<char*>("Application"));
            pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
            pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

            txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

            // sip catalog消息发送
            cbStatuePtr->store(MyStatus_t::FAILED);
            if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, cbStatuePtr, &MySipCatalogApp::OnRespUpdateCatalogCb)) {
                LOG(ERROR) << "Sip catalog app update catalog device cfg to up server failed. send pjsip request failed. " 
                           << upRegServInfo << ".";
                return -1;
            }

            // 记录等待消息响应开始时间
            auto start = std::chrono::high_resolution_clock::now();
            while (MyStatus_t::SUCCESS != cbStatuePtr->load()) {
                // 记录当前时间
                auto end = std::chrono::high_resolution_clock::now();

                // 计算时间差
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
                if (duration.count() > 5) {
                    LOG(ERROR) << "Sip catalog app update catalog device cfg to up server failed. recv up serv response failed. " 
                               << upRegServInfo << ".";
                    return -1;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }
    return 0;
}

void MySipCatalogApp::OnRespUpdateCatalogCb(MY_COMMON::MyFuncCbParamPtr arg, MY_COMMON::MySipEvPtr ev)
{
    MySipStatusPtr cbStatuePtr =  static_cast<MySipStatusPtr>(arg);

    if (200 != ev->body.rx_msg.rdata->msg_info.msg->line.status.code) {
        LOG(ERROR) << "Sip catalog app update local server catalog failed. recv up serv error response.";
        cbStatuePtr->store(MyStatus_t::FAILED);
        return;
    }
    cbStatuePtr->store(MyStatus_t::SUCCESS);
}

MySipCatalogApp::MySipCatalogApp() : m_status(MyStatus_t::FAILED)
{
}

MySipCatalogApp::~MySipCatalogApp()
{
    if (MyStatus_t::SUCCESS == m_status.load()) {
        this->shutdown();
    }
}

MyStatus_t MySipCatalogApp::init(const std::string& servId, const std::string& id, const std::string& name, pjsip_module_priority priority)
{
    // 状态检查
    if (MyStatus_t::SUCCESS == m_status.load()) {
        LOG(WARNING) << "Sip catalog app module init failed. MySipCatalogApp is init already.";
        return MyStatus_t::SUCCESS;
    }

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip catalog app module init failed. invalid endpoint.";
        return MyStatus_t::FAILED;
    }

    // 服务id赋值
    m_servId = servId;

    // app标识初始化 
    m_appIdCfg.id       = id;
    m_appIdCfg.name     = name;   
    m_appIdCfg.priority = priority;

    // 创建设备目录信息
    MySipCatalogInfo_dt catalogInfo;

    // 添加设备目录平台信息
    MySipCatalogCfg_dt catalogCfg;
    MySystemConfig::GetSipCatalogCfg(catalogCfg);

    // 确保平台id与服务id一致
    if (catalogCfg.catalogPlatCfg.deviceID != m_servId) {
        LOG(ERROR) << "Sip catalog app module init failed. invalid catalog plat cfg.";
        return MyStatus_t::FAILED;
    }

    // 插入设备目录平台配置
    catalogInfo.sipPlatMap.insert(std::make_pair(catalogCfg.catalogPlatCfg.deviceID, catalogCfg.catalogPlatCfg));
    
    // 添加设备目录子平台信息
    const auto& sipPlatMap = catalogInfo.sipPlatMap;
    for (const auto& subPlatCfgPair : catalogCfg.catalogSubPlatCfgMap) {
        // 确保子平台id关联的平台id存在
        if (sipPlatMap.end() == sipPlatMap.find(subPlatCfgPair.second.parentID)) {
            continue;
        }

        // 确保子平台id未重复
        if (catalogInfo.sipSubPlatMap.end() != catalogInfo.sipSubPlatMap.find(subPlatCfgPair.first)) {
            continue;
        }

        // 插入设备目录子平台配置
        catalogInfo.sipSubPlatMap.insert(std::make_pair(subPlatCfgPair.first, subPlatCfgPair.second));
    }

    // 添加设备目录虚拟子平台信息
    const auto& sipSubPlatMap = catalogInfo.sipSubPlatMap;
    for (const auto& subVirtualCfgPair : catalogCfg.catalogVirtualSubPlatCfgMap) {
        // 确保虚拟子平台id关联的平台/子平台id存在
        if (sipSubPlatMap.end() == sipSubPlatMap.find(subVirtualCfgPair.second.parentID) &&
            sipPlatMap.end() == sipPlatMap.find(subVirtualCfgPair.second.parentID)) {
            continue;
        }

        // 确保虚拟子平台id未重复
        if (catalogInfo.sipSubVirtualPlatMap.end() != catalogInfo.sipSubVirtualPlatMap.find(subVirtualCfgPair.first)) {
            continue;
        }

        // 插入设备目录虚拟子平台配置
        catalogInfo.sipSubVirtualPlatMap.insert(std::make_pair(subVirtualCfgPair.first, subVirtualCfgPair.second));
    }

    // 添加设备目录设备信息
    const auto& sipSubVirtualPlatMap = catalogInfo.sipSubVirtualPlatMap;
    for (const auto& deviceCfgPair : catalogCfg.catalogDeviceCfgMap) {
        // 确保设备id关联的虚拟子平台/子平台/平台id存在
        if (sipSubVirtualPlatMap.end() == sipSubVirtualPlatMap.find(deviceCfgPair.second.parentID) &&
            sipSubPlatMap.end() == sipSubPlatMap.find(deviceCfgPair.second.parentID) &&
            sipPlatMap.end() == sipPlatMap.find(deviceCfgPair.second.parentID)) {
            continue;
        }

        // 确保设备id未重复
        if (catalogInfo.sipDeviceMap.end() != catalogInfo.sipDeviceMap.find(deviceCfgPair.first)) {
            continue;
        }

        // 插入设备目录设备配置
        catalogInfo.sipDeviceMap.insert(std::make_pair(deviceCfgPair.first, deviceCfgPair.second));
    }

    // 添加设备目录信息
    MySipCatalogManage::AddSipCatalogInfo(catalogCfg.catalogPlatCfg.deviceID, catalogInfo);

    std::string sipAppInfo;
    MySipAppHelper::GetSipAppInfo(m_appIdCfg, sipAppInfo);
    LOG(INFO) << "Sip catalog app module init success. " << sipAppInfo << ".";

    m_status.store(MyStatus_t::SUCCESS);
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipCatalogApp::run()
{
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipCatalogApp::shutdown()
{
    // 状态检查
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "Sip catalog app module shutdown failed. MySipCatalogApp is not init.";
        return MyStatus_t::SUCCESS;
    }

    std::string appInfo;
    MySipAppHelper::GetSipAppInfo(m_appIdCfg, appInfo);
    LOG(INFO) << "Sip app module shutdown success. " << appInfo << ".";

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipCatalogApp::onRecvSipCatalogQueryReqMsg(MySipRxDataPtr rxDataPtr) const
{
    MySipCatalogReqMsgBody_dt catalogMsgBody;
    if (MyStatus_t::SUCCESS != MyXmlHelper::ParseSipCatalogQueryReqMsgBody(static_cast<char*>(rxDataPtr->msg_info.msg->body->data),
        catalogMsgBody)) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. parse catalog msg failed.";
        return MyStatus_t::FAILED;
    }

    // 服务id校验
    if (catalogMsgBody.deviceId != m_servId) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. invalid device id. " << catalogMsgBody.deviceId << ".";
        return MyStatus_t::FAILED;
    }

    // 解析上级服务id
    char fromHdrBuf[256] = {0};
    pjsip_uri_print(PJSIP_URI_IN_REQ_URI, rxDataPtr->msg_info.from->uri, fromHdrBuf, sizeof(fromHdrBuf));

    std::string upRegServId;
    std::string upRegServIpAddr;
    if (MyStatus_t::SUCCESS != MySipMsgHelper::ParseSipMsgFromHdr(fromHdrBuf, upRegServId, upRegServIpAddr)) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. parse from header failed.";
        return MyStatus_t::FAILED;
    }

    MySipRegUpServCfg_dt upServRegCfg;
    if (MyStatus_t::SUCCESS != MySipRegManage::GetSipRegUpServCfg(upRegServId, upServRegCfg)) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. get up reg serv cfg failed. " << upRegServId << ".";
        return MyStatus_t::FAILED;
    }

    if (upServRegCfg.upSipServRegAddrCfg.id.empty()) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. invalid up reg serv id. " << upRegServId << ".";
        return MyStatus_t::FAILED;
    }

    // 获取本地服务配置
    MySipServAddrCfg_dt localServCfg;
    if (MyStatus_t::SUCCESS != MyServManage::GetSipServAddrCfg(localServCfg)) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. get local serv cfg failed. " << m_servId << ".";
        return MyStatus_t::FAILED;
    }
    
    if (localServCfg.id.empty()) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. invalid local serv id. " << m_servId << ".";
        return MyStatus_t::FAILED;
    }

    // 设备目录信息不存在
    if (MyStatus_t::SUCCESS != MySipCatalogManageView::HasSipCatalogInfo(m_servId)) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. invalid catalog info. local serv id. " << m_servId << ".";
        return MyStatus_t::FAILED;
    }

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip catalog app module recv catalog message. get pjsip endpt ptr failed. ";
        return MyStatus_t::FAILED;
    }

    // 发送sip应答消息
    if (PJ_SUCCESS != pjsip_endpt_respond(endptPtr, nullptr, rxDataPtr, 200, nullptr, nullptr, nullptr,
        nullptr)) {
        LOG(ERROR) << "Sip catalog app receive catalog message failed. can't send response message. ";
        return MyStatus_t::FAILED;
    }

    // 更新SN号
    MySipCatalogManage::UpdateSipCatalogSN(m_servId, catalogMsgBody.sn);

    // 添加上级服务信息
    MySipCatalogManage::AddSipQueryInfo(upServRegCfg.upSipServRegAddrCfg);

    // 创建发送sip设备目录消息线程参数，delete in function: MySipCatalogApp::OnPushCatalogCb()
    MySipCatalogAppPushCatalogThdParamPtr thdParamPtr = new MySipCatalogAppPushCatalogThdParam_dt();
    thdParamPtr->m_sn           = catalogMsgBody.sn;
    thdParamPtr->m_cseq         = rxDataPtr->msg_info.cseq->cseq;
    thdParamPtr->m_regUpServCfg = upServRegCfg;
    thdParamPtr->m_servAddrCfg  = localServCfg;

    // 发送sip设备目录消息
    MySystemThdPool::ThreadPoolTaskFunc thdPoolFunc = [thdParamPtr]() {
        MySipCatalogApp::OnPushCatalogCb(thdParamPtr);
    };
    
    MySystemThdPool::ExecuteTask(thdPoolFunc);

    // 消息处理成功
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipCatalogApp::onRecvSipCatalogResponseReqMsg(MySipRxDataPtr rxDataPtr)
{
    // 获取消息内容
    std::string msgBodyStr = static_cast<const char*>(rxDataPtr->msg_info.msg->body->data);

    // delete in: MySipCatalogApp::OnUpdateCatalogCb()
    MySipCatalogAppUptCatalogThdParamPtr thdParamPtr = new MySipCatalogAppUptCatalogThdParam_dt();
    thdParamPtr->m_servId = m_servId;

    // 解析下级推送设备类型
    if (MyStatus_t::SUCCESS != MyXmlHelper::ParseSipCatalogRespMsgBody(msgBodyStr, thdParamPtr->m_catalogRespMsgBody)) {
        LOG(ERROR) << "Sip catalog app module recv catalog response message. parse message body failed.";

        delete thdParamPtr;
        return MyStatus_t::FAILED;
    }

    // 判断下级sip服务是否有效
    MySipRegLowServCfg_dt lowRegServCfg;
    if (MyStatus_t::SUCCESS != MySipRegManage::GetSipRegLowServCfg(thdParamPtr->m_catalogRespMsgBody.deviceId, lowRegServCfg)) {
        LOG(ERROR) << "Sip catalog app module recv catalog response message. invalid low serv id. " 
                   << thdParamPtr->m_catalogRespMsgBody.deviceId << ".";

        delete thdParamPtr;
        return MyStatus_t::FAILED;
    }

    // 添加下级服务信息
    MySipCatalogManage::AddSipRespInfo(lowRegServCfg.lowSipServRegAddrCfg);

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip catalog app module recv catalog response message failed. get pjsip endpoint failed. " 
                   << thdParamPtr->m_catalogRespMsgBody.deviceId << ".";

        delete thdParamPtr;
        return MyStatus_t::FAILED;
    }

    // 发送sip应答消息
    if (PJ_SUCCESS != pjsip_endpt_respond(endptPtr, nullptr, rxDataPtr, 200, nullptr, nullptr, nullptr,
        nullptr)) {
        LOG(ERROR) << "Sip catalog app module recv catalog response message failed. can't send response message. " 
                   << thdParamPtr->m_catalogRespMsgBody.deviceId << ".";

        delete thdParamPtr;
        return MyStatus_t::FAILED;
    }

    // 更新sip设备目录消息
    MySystemThdPool::ThreadPoolTaskFunc thdPoolFunc = [thdParamPtr]() {
        MySipCatalogApp::OnUpdateCatalogCb(thdParamPtr);
    };
    
    MySystemThdPool::ExecuteTask(thdPoolFunc);
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipCatalogApp::getState(MyStatus_t& status) const
{
    status = m_status.load();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipCatalogApp::getId(std::string& id) const
{
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(ERROR) << "Sip app module get id failed. MySipCatalogApp is not init.";
        return MyStatus_t::FAILED;
    }

    id = m_appIdCfg.id;
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipCatalogApp::getSipCatalogApp(MySipCatalogApp::SmtWkPtr& wkPtr)
{
    if (MyStatus_t::SUCCESS != m_status.load()) {
        LOG(WARNING) << "Sip app module get work pointer failed. MySipCatalogApp is not init.";
        return MyStatus_t::FAILED;
    }

    wkPtr = this->shared_from_this();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipCatalogApp::onReqLowServCatalog(const MySipRegLowServCfg_dt& regLowServCfg, const MySipServAddrCfg_dt& localServCfg) const
{
    std::string lowRegServInfo;
    MyServerHelper::PrintSipLowRegServInfo(regLowServCfg, lowRegServInfo);

    // 获取endpoint
    MySipEndptPtr endptPtr = nullptr;
    if (MyStatus_t::SUCCESS != MySystemPjsip::GetPjsipEndptPtr(&endptPtr)) {
        LOG(ERROR) << "Sip catalog app request low server catalog failed. get pjsip endpoint failed. " << lowRegServInfo << ".";
        return MyStatus_t::FAILED;
    }

    // sip catalog消息首部生成
    std::string sURL;
    MySipMsgHelper::GenerateSipMsgURL(regLowServCfg.lowSipServRegAddrCfg.id, regLowServCfg.lowSipServRegAddrCfg.ipAddr, 
        regLowServCfg.lowSipServRegAddrCfg.port, regLowServCfg.proto, sURL);

    std::string sFromHdr;
    MySipMsgHelper::GenerateSipMsgFromHeader(localServCfg.id, localServCfg.ipAddr, sFromHdr);

    std::string sToHdr;
    MySipMsgHelper::GenerateSipMsgToHeader(localServCfg.id, localServCfg.ipAddr, sToHdr);

    pj_str_t sipMsgURL     = pj_str(const_cast<char*>(sURL.c_str()));
    pj_str_t sipMsgFromHdr = pj_str(const_cast<char*>(sFromHdr.c_str()));
    pj_str_t sipMsgToHdr   = pj_str(const_cast<char*>(sToHdr.c_str()));

    pjsip_method sipMethod = {PJSIP_OTHER_METHOD, {const_cast<char*>("MESSAGE"), (pj_ssize_t)strlen("MESSAGE")}};

    // sip catalog消息创建
    MySipTxDataPtr txDataPtr = nullptr;
    if(PJ_SUCCESS != pjsip_endpt_create_request(endptPtr, &sipMethod, &sipMsgURL, &sipMsgFromHdr, &sipMsgToHdr, nullptr, nullptr,
        -1, nullptr, &txDataPtr)) {
        LOG(ERROR) << "Sip catalog app request low server catalog failed. create pjsip request failed. " << lowRegServInfo << ".";
        return MyStatus_t::FAILED;
    }

    // 指定传输端口为注册端口
    pjsip_tpselector tpSelector;
    tpSelector.type = PJSIP_TPSELECTOR_TRANSPORT;
    tpSelector.disable_connection_reuse = false;

    MySipTransportPtr transportPtr = nullptr;
    if (MyTpProto_t::UDP == regLowServCfg.proto) {
        if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegUdpTp(&transportPtr)) {
            LOG(ERROR) << "Sip catalog app request low server catalog failed. get sip serv reg udp tp failed. " << lowRegServInfo << ".";
            return MyStatus_t::FAILED;
        }
    }
    else {
        const auto& regLowServAddr = regLowServCfg.lowSipServRegAddrCfg;
        if (MyStatus_t::SUCCESS != MyServManage::GetSipServRegTcpTp(&transportPtr, regLowServAddr.ipAddr, regLowServAddr.port)) {
            LOG(ERROR) << "Sip catalog app request low server catalog failed. get sip serv reg tcp tp failed. " << lowRegServInfo << ".";
            return MyStatus_t::FAILED;
        }
    }

    tpSelector.u.transport = transportPtr;
    pjsip_tx_data_set_transport(txDataPtr, &tpSelector);

    // sip catalog消息内容填充
    std::string msgBody;
    MyXmlHelper::GenerateSipCatalogQueryReqMsgBody(regLowServCfg.lowSipServRegAddrCfg.id, msgBody);

    pj_str_t type    = pj_str(const_cast<char*>("Application"));
    pj_str_t subtype = pj_str(const_cast<char*>("MANSCDP+xml"));
    pj_str_t xmldata = pj_str(const_cast<char*>(msgBody.c_str()));

    txDataPtr->msg->body = pjsip_msg_body_create(txDataPtr->pool, &type, &subtype, &xmldata);

    // sip catalog消息发送
    if(PJ_SUCCESS != pjsip_endpt_send_request(endptPtr, txDataPtr, -1, nullptr, nullptr)) {
        LOG(ERROR) << "Sip catalog app request low server catalog failed. send pjsip request failed. " << lowRegServInfo << ".";
        return MyStatus_t::FAILED;
    }
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_APP