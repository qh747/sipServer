#include <sstream>
#include "utils/mySipServerHelper.h"
using namespace MY_COMMON;

namespace MY_UTILS {

MyStatus_t MySipServerHelper::GetSipServInfo(const MySipServAddrCfg_dt& sipServAddrCfg, std::string& info)
{
    std::stringstream ss;
    ss << "name: " << sipServAddrCfg.name   << " domain: " << sipServAddrCfg.domain << " id: " << sipServAddrCfg.id
       << " ip: "  << sipServAddrCfg.ipAddr << " port: "   << sipServAddrCfg.port   << " protocol: udp/tcp";

    info = ss.str();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServerHelper::GetSipUpRegServInfo(const MySipRegUpServCfg_dt& sipUpRegServCfg, std::string& info)
{
    return MySipServerHelper::GetSipServInfo(sipUpRegServCfg.upSipServAddrCfg, info);
}

MyStatus_t MySipServerHelper::GetSipLowRegServInfo(const MySipRegLowServCfg_dt& sipLowRegServCfg, std::string& info)
{
    return MySipServerHelper::GetSipServInfo(sipLowRegServCfg.lowSipServAddrCfg, info);
}

MyStatus_t MySipServerHelper::GetSipServThdPoolName(const MySipServAddrCfg_dt& sipServAddrCfg, std::string& name)
{
    std::stringstream ss;
    ss << "sipServThdPool_" << sipServAddrCfg.id << "_" << sipServAddrCfg.ipAddr << "_" << sipServAddrCfg.port;

    name = ss.str();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipServerHelper::GetSipServEvThdName(const MySipServAddrCfg_dt& sipServAddrCfg, std::string& name)
{
    std::stringstream ss;
    ss << "sipServEvThd_" << sipServAddrCfg.id << "_" << sipServAddrCfg.ipAddr << "_" << sipServAddrCfg.port;

    name = ss.str();
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_VIEW