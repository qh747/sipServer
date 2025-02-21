#pragma once
#include <map>
#include <string>
#include <memory>
#include <pjsip.h>
#include <pjlib.h>
#include <pjlib-util.h>
#include <pjmedia.h>
#include <pjsip_ua.h>
#include <pjsip/sip_uri.h>
#include <pjsip/sip_msg.h>
#include <pjsip/sip_auth.h>
#include "common/myDataDef.h"
#include "common/myConfigDef.h"

namespace MY_COMMON {

typedef const   pj_str_t*                                                   MySipStrCstPtr;
typedef         pj_pool_t*                                                  MySipPoolPtr;
typedef         pjsip_event*                                                MySipEvPtr;
typedef         pjsip_cred_info*                                            MySipCredInfoPtr;
typedef         pjsip_endpoint*                                             MySipEndptPtr; 
typedef         pjsip_endpoint**                                            MySipEndptPtrAddr; 
typedef         pjmedia_endpt*                                              MySipMediaEndptPtr;
typedef         pjmedia_endpt**                                             MySipMediaEndptPtrAddr;
typedef         pjsip_regc*                                                 MySipRegcPtr;
typedef         pjsip_hdr*                                                  MySipMsgHdrPtr;
typedef         pjsip_rx_data*                                              MySipRxDataPtr;
typedef         pjsip_tx_data*                                              MySipTxDataPtr;
typedef         pjsip_date_hdr*                                             MySipMsgDateHdrPtr;
typedef         pjsip_www_authenticate_hdr*                                 MySipMsgAuthHdrPtr;
typedef         pjsip_regc_cbparam*                                         MySipRegCbParamPtr;
typedef         pjsip_transaction*                                          MySipTsxPtr;
typedef         pj_thread_t*                                                MySipThdPtr; 
typedef         void*                                                       MySipEvThdCbParamPtr;

typedef         MySipServAddrCfg_dt*                                        MySipServAddrCfgPtr;
typedef         MySipRegUpServCfg_dt*                                       MySipRegUpServCfgPtr;
typedef         std::shared_ptr<pjsip_module>                               MySipModSmtPtr;
typedef         std::shared_ptr<MySipUpRegServInfo_dt>                      MySipUpRegServInfoSmtPtr;
typedef         std::shared_ptr<MySipLowRegServInfo_dt>                     MySipLowRegServInfoSmtPtr;

// key = local sip server id, value = sip server address config
typedef         std::map<std::string, MySipServAddrCfg_dt>                  MySipServAddrMap;
// key = local sip server id, value = reg sip server config(inclue up reg sip server and low reg sip server)
typedef         std::map<std::string, MySipRegServCfg_dt>                   MySipRegServCfgMap;
// key = up reg sip server id, value = up reg sip server config     
typedef         std::map<std::string, MySipRegUpServCfg_dt>                 MySipRegUpServCfgMap;
// key = low reg sip server id, value = low reg sip server config       
typedef         std::map<std::string, MySipRegLowServCfg_dt>                MySipRegLowServCfgMap;
// key = devide id, value = sip server platform config
typedef         std::map<std::string, MySipCatalogPlatCfg_dt>               MySipCatalogPlatCfgMap;
// key = local sip server id, value = sip server platform config map
typedef         std::map<std::string, MySipCatalogPlatCfgMap>               MySipServCatalogPlatCfgMap;
// key = devide id, value = sip server sub platform config
typedef         std::map<std::string, MySipCatalogSubPlatCfg_dt>            MySipCatalogSubPlatCfgMap;
// key = local sip server id, value = sip server sub platform config map
typedef         std::map<std::string, MySipCatalogSubPlatCfgMap>            MySipServCatalogSubPlatCfgMap;
// key = device id, value = sip server virtual sub platform config
typedef         std::map<std::string, MySipCatalogVirtualSubPlatCfg_dt>     MySipCatalogSubVirtualPlatCfgMap;
// key = local sip server id, value = sip server virtual sub platform config map
typedef         std::map<std::string, MySipCatalogSubVirtualPlatCfgMap>     MySipServCatalogSubVirtualPlatCfgMap;
// key = device id, value = sip device config
typedef         std::map<std::string, MySipCatalogDeviceCfg_dt>             MySipCatalogDeviceCfgMap;
// key = local sip server id, value = sip device config map
typedef         std::map<std::string, MySipCatalogDeviceCfgMap>             MySipServCatalogDeviceCfgMap;

}; // namespace MY_COMMON