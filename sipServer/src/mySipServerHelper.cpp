#include <sstream>
#include "utils/mySipServerHelper.h"
using MY_COMMON::MySipServAddrCfg_dt;
using MY_COMMON::MySipRegUpServCfg_dt;
using MY_COMMON::MySipRegLowServCfg_dt;

namespace MY_UTILS {

std::string MySipServerHelper::GetSipServInfo(const MySipServAddrCfg_dt& sipServAddrCfg)
{
    std::stringstream ss;
    ss << "name: " << sipServAddrCfg.name   << " domain: " << sipServAddrCfg.domain << " id: " << sipServAddrCfg.id
       << " ip: "  << sipServAddrCfg.ipAddr << " port: "   << sipServAddrCfg.port   << " protocol: udp/tcp";

    return ss.str();
}

std::string MySipServerHelper::GetSipUpRegServInfo(const MySipRegUpServCfg_dt& sipUpRegServCfg)
{
    return MySipServerHelper::GetSipServInfo(sipUpRegServCfg.upSipServAddrCfg);
}

std::string MySipServerHelper::GetSipLowRegServInfo(const MySipRegLowServCfg_dt& sipLowRegServCfg)
{
    return MySipServerHelper::GetSipServInfo(sipLowRegServCfg.lowSipServAddrCfg);
}

std::string MySipServerHelper::GetSipServThdPoolName(const MySipServAddrCfg_dt& sipServAddrCfg)
{
    std::stringstream ss;
    ss << "sipServThdPool_" << sipServAddrCfg.id << "_" << sipServAddrCfg.ipAddr << "_" << sipServAddrCfg.port;

    return ss.str();
}

std::string MySipServerHelper::GetSipServEvThdName(const MySipServAddrCfg_dt& sipServAddrCfg)
{
    std::stringstream ss;
    ss << "sipServEvThd_" << sipServAddrCfg.id << "_" << sipServAddrCfg.ipAddr << "_" << sipServAddrCfg.port;

    return ss.str();
}

}; // namespace MY_VIEW