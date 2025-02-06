#pragma once
#include <memory>
#include <pjsip.h>
#include <pjlib.h>
#include <pjlib-util.h>
#include <pjmedia.h>
#include <pjsip_ua.h>
#include <pjsip/sip_msg.h>
#include <pjsip/sip_auth.h>
#include "common/myDataDef.h"
#include "common/myConfigDef.h"

namespace MY_COMMON {

typedef         MySipRegUpServCfg_dt                      SipRegUpServCfg;
typedef         MySipLowRegServInfo_dt                    SipLowRegServInfo;
typedef         MySipServAddrCfg_dt                       SipServAddrCfg;
typedef         MySipUpRegServInfo_dt                     SipUpRegServInfo;
typedef         MySipEvThdMemCfg_dt                       SipEvThdMemCfg;

typedef const   pj_str_t*                                 SipStrCstPtr;
typedef         pj_pool_t*                                SipPoolPtr;
typedef         pjsip_event*                              SipEvPtr;
typedef         pjsip_cred_info*                          SipCredInfoPtr;
typedef         pjsip_endpoint*                           SipEndptPtr; 
typedef         pjmedia_endpt*                            SipMediaEndptPtr;
typedef         pjsip_regc*                               SipRegcPtr;
typedef         pjsip_hdr*                                SipMsgHdrPtr;
typedef         pjsip_rx_data*                            SipRxDataPtr;
typedef         pjsip_tx_data*                            SipTxDataPtr;
typedef         pjsip_date_hdr*                           SipMsgDateHdrPtr;
typedef         pjsip_www_authenticate_hdr*               SipMsgAuthHdrPtr;
typedef struct  pjsip_regc_cbparam*                       SipRegCbParamPtr;
typedef         pjsip_transaction*                        SipTsxPtr;
typedef         pj_thread_t*                              SipThdPtr; 
typedef         void*                                     SipEvThdCbParamPtr;

typedef         std::shared_ptr<MySipAppIdCfg_dt>         SipIdSmtPtr;
typedef         std::shared_ptr<SipUpRegServInfo>         SipUpRegServInfoSmtPtr;
typedef         std::shared_ptr<SipLowRegServInfo>        SipLowRegServInfoSmtPtr;
typedef         std::shared_ptr<pjsip_module>             SipModSmtPtr;
typedef         std::shared_ptr<SipServAddrCfg>           SipServAddrCfgSmtPtr;


}; // namespace MY_COMMON