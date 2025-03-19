#include "sdp/mySdpReg.h"

namespace MY_SDP {

std::map<std::string, MySdpReg::OnCreateSdpItem, MySdpReg::MyStrCaseCompare> MySdpReg::MySdpItemCreatMap;

bool MySdpReg::RegisterAllIterm()
{
    RegisterSdpItem<MySdpString<'v'>>();
    RegisterSdpItem<MySdpString<'s'>>();
    RegisterSdpItem<MySdpTime>();
    RegisterSdpItem<MySdpOrigin>();
    RegisterSdpItem<MySdpConnection>();
    RegisterSdpItem<MySdpMediaLine>();
    RegisterSdpItem<MySdpAttr>();
    RegisterSdpItem<MySdpAttrSetup>();
    RegisterSdpItem<MySdpAttrRtpMap>();
    RegisterSdpItem<MySdpAttrSSRC>();
    RegisterSdpItem<MySdpGB28181SSRC>();
    RegisterSdpItem<MySdpDirectSendonly>();
    RegisterSdpItem<MySdpDirectRecvonly>();
    RegisterSdpItem<MySdpDirectSendrecv>();
    RegisterSdpItem<MySdpDirectInactive>();

    return true;
}

} // namespace MY_SDP