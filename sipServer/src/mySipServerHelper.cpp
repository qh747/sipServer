#include <sstream>
#include "utils/mySipServerHelper.h"
using namespace MY_COMMON;

namespace MY_UTILS {

MyStatus_t MySipServerHelper::PrintSipServInfo(const MySipServAddrCfg_dt& sipServAddrCfg, std::string& info)
{
    std::stringstream ss;
    ss << "name: " << sipServAddrCfg.name   << " domain: " << sipServAddrCfg.domain << " id: " << sipServAddrCfg.id
       << " ip: "  << sipServAddrCfg.ipAddr << " port: "   << sipServAddrCfg.port   << " protocol: udp/tcp";

    info = ss.str();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServerHelper::PrintSipRegServInfo(const MySipServRegAddrCfg_dt& sipRegServAddrCfg, std::string& info)
{
    std::stringstream ss;
    ss << "name: " << sipRegServAddrCfg.name   << " domain: " << sipRegServAddrCfg.domain << " id: " << sipRegServAddrCfg.id
       << " ip: "  << sipRegServAddrCfg.ipAddr << " port: "   << sipRegServAddrCfg.port   << " protocol: udp/tcp";

    info = ss.str();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServerHelper::PrintSipUpRegServInfo(const MySipRegUpServCfg_dt& sipUpRegServCfg, std::string& info)
{
    return MySipServerHelper::PrintSipRegServInfo(sipUpRegServCfg.upSipServRegAddrCfg, info);
}

MyStatus_t MySipServerHelper::PrintSipLowRegServInfo(const MySipRegLowServCfg_dt& sipLowRegServCfg, std::string& info)
{
    return MySipServerHelper::PrintSipRegServInfo(sipLowRegServCfg.lowSipServRegAddrCfg, info);
}

MyStatus_t MySipServerHelper::PrintSipServThdPoolName(const MySipServAddrCfg_dt& sipServAddrCfg, std::string& name)
{
    std::stringstream ss;
    ss << "sipServThdPool_" << sipServAddrCfg.id << "_" << sipServAddrCfg.ipAddr << "_" << sipServAddrCfg.port;

    name = ss.str();
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_VIEW