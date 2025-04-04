#pragma once
#include <map>
#include <atomic>
#include <string>
#include <memory>
#include <pjsip.h>
#include <pjlib.h>
#include <pjlib-util.h>
#include <pjmedia.h>
#include <pjmedia/sdp_neg.h>
#include <pjsip_ua.h>
#include <pjsip/sip_uri.h>
#include <pjsip/sip_msg.h>
#include <pjsip/sip_auth.h>
#include <pjsip/sip_dialog.h>
#include "common/myDataDef.h"
#include "common/myConfigDef.h"

namespace MY_COMMON {

typedef         pj_str_t*                                                   MySipStrPtr;
typedef const   pj_str_t*                                                   MySipStrCstPtr;
typedef         pj_pool_t*                                                  MySipPoolPtr;
typedef         pj_pool_t**                                                 MySipPoolPtrAddr;
typedef         pjsip_event*                                                MySipEvPtr;
typedef         pjsip_cred_info*                                            MySipCredInfoPtr;
typedef         pjsip_tpmgr*                                                MySipTpmgrPtr;
typedef         pjsip_endpoint*                                             MySipEndptPtr; 
typedef         pjsip_endpoint**                                            MySipEndptPtrAddr; 
typedef         pjmedia_endpt*                                              MySipMediaEndptPtr;
typedef         pjmedia_endpt**                                             MySipMediaEndptPtrAddr;
typedef         pjsip_transport*                                            MySipTransportPtr;
typedef         pjsip_transport**                                           MySipTransportPtrAddr;
typedef         pjsip_tpfactory*                                            MySipTransportFactoryPtr;
typedef         pjsip_tpfactory**                                           MySipTransportFactoryPtrAddr;
typedef         pjsip_regc*                                                 MySipRegcPtr;
typedef         pjsip_hdr*                                                  MySipMsgHdrPtr;
typedef         pjsip_rx_data*                                              MySipRxDataPtr;
typedef         pjsip_tx_data*                                              MySipTxDataPtr;
typedef         pjsip_date_hdr*                                             MySipMsgDateHdrPtr;
typedef         pjsip_www_authenticate_hdr*                                 MySipMsgAuthHdrPtr;
typedef         pjsip_generic_string_hdr*                                   MySipMsgGenericHdrPtr;
typedef         pjsip_regc_cbparam*                                         MySipRegCbParamPtr;
typedef         pjsip_inv_session*                                          MySipInvSessionPtr;
typedef         pjmedia_sdp_session*                                        MySipSdpSessionPtr;
typedef         const pjmedia_sdp_session*                                  MySipSdpSessionCstPtr;
typedef         pjmedia_sdp_session**                                       MySipSdpSessionPtrAddr;
typedef         pjmedia_sdp_conn*                                           MySipSdpConnPtr;
typedef         pjmedia_sdp_media*                                          MySipSdpMediaPtr;
typedef         pjmedia_sdp_attr*                                           MySipSdpAttrPtr;
typedef         pjsip_rdata_sdp_info*                                       MySipRdataSdpInfoPtr;
typedef         pjsip_transaction*                                          MySipTsxPtr;
typedef         pj_thread_t*                                                MySipThdPtr; 
typedef         pjsip_dialog*                                               MySipDialogPtr;
typedef         pjsip_dialog**                                              MySipDialogPtrAddr;
typedef         pjsip_module*                                               MySipModPtr;
typedef         std::shared_ptr<pjsip_module>                               MySipModSmtPtr;
typedef         std::shared_ptr<pjsip_inv_callback>                         MySipInvCbSmtPtr;

typedef         void*                                                       MyFuncCbParamPtr;
typedef         MySipServAddrCfg_dt*                                        MySipServAddrCfgPtr;
typedef         MySipRegUpServCfg_dt*                                       MySipRegUpServCfgPtr;
typedef         std::atomic<MyStatus_t>*                                    MySipStatusPtr;
typedef         std::atomic<MY_COMMON::MyStatus_t>                          MyAtomicStatus;

typedef         void*                                                       MyEventTagPtr;

// key = reg sip server id, value = reg sip server address config
typedef         std::map<std::string, MySipServRegAddrCfg_dt>               MySipRegServAddrMap;
// key = remote server ip + remote port, value = sip server transport
typedef         std::map<std::string, MySipTransportPtr>                    MySipServTpMap;
// key = up reg sip server id, value = up reg sip server config     
typedef         std::map<std::string, MySipRegUpServCfg_dt>                 MySipRegUpServCfgMap;
// key = low reg sip server id, value = low reg sip server config       
typedef         std::map<std::string, MySipRegLowServCfg_dt>                MySipRegLowServCfgMap;
// key = device id, value = sip server platform config
typedef         std::map<std::string, MySipCatalogPlatCfg_dt>               MySipCatalogPlatCfgMap;
// key = device id, value = sip server sub platform config
typedef         std::map<std::string, MySipCatalogSubPlatCfg_dt>            MySipCatalogSubPlatCfgMap;
// key = device id, value = sip server virtual sub platform config
typedef         std::map<std::string, MySipCatalogVirtualSubPlatCfg_dt>     MySipCatalogSubVirtualPlatCfgMap;
// key = device id, value = sip device config
typedef         std::map<std::string, MySipCatalogDeviceCfg_dt>             MySipCatalogDeviceCfgMap;

}; // namespace MY_COMMON