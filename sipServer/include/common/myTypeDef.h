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

typedef const   pj_str_t*                                       MySipStrCstPtr;
typedef         pj_pool_t*                                      MySipPoolPtr;
typedef         pjsip_event*                                    MySipEvPtr;
typedef         pjsip_cred_info*                                MySipCredInfoPtr;
typedef         pjsip_endpoint*                                 MySipEndptPtr; 
typedef         pjmedia_endpt*                                  MySipMediaEndptPtr;
typedef         pjsip_regc*                                     MySipRegcPtr;
typedef         pjsip_hdr*                                      MySipMsgHdrPtr;
typedef         pjsip_rx_data*                                  MySipRxDataPtr;
typedef         pjsip_tx_data*                                  MySipTxDataPtr;
typedef         pjsip_date_hdr*                                 MySipMsgDateHdrPtr;
typedef         pjsip_www_authenticate_hdr*                     MySipMsgAuthHdrPtr;
typedef struct  pjsip_regc_cbparam*                             MySipRegCbParamPtr;
typedef         pjsip_transaction*                              MySipTsxPtr;
typedef         pj_thread_t*                                    MySipThdPtr; 
typedef         void*                                           MySipEvThdCbParamPtr;
        
typedef         MySipRegUpServCfg_dt*                           MySipRegUpServCfgPtr;
typedef         std::shared_ptr<pjsip_module>                   MySipModSmtPtr;
typedef         std::shared_ptr<MySipUpRegServInfo_dt>          MySipUpRegServInfoSmtPtr;
typedef         std::shared_ptr<MySipLowRegServInfo_dt>         MySipLowRegServInfoSmtPtr;

// key = local sip server id, value = sip server address config
typedef         std::map<std::string, MySipServAddrCfg_dt>      MySipServAddrMap;
// key = local sip server id, value = reg sip server config(inclue up reg sip server and low reg sip server)
typedef         std::map<std::string, MySipRegServCfg_dt>       MySipRegServCfgMap;
// key = up reg sip server id, value = up reg sip server config
typedef         std::map<std::string, MySipRegUpServCfg_dt>     MySipRegUpServCfgMap;
// key = low reg sip server id, value = low reg sip server config
typedef         std::map<std::string, MySipRegLowServCfg_dt>    MySipRegLowServCfgMap;


}; // namespace MY_COMMON