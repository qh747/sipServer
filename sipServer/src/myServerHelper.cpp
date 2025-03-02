#include <sstream>
#include "utils/myServerHelper.h"
using namespace MY_COMMON;

namespace MY_UTILS {

MyStatus_t MyServerHelper::PrintSipServInfo(const MySipServAddrCfg_dt& servAddrCfg, std::string& info)
{
    std::stringstream ss;
    ss << "name: " << servAddrCfg.name   << " domain: " << servAddrCfg.domain << " id: " << servAddrCfg.id
       << " ip: "  << servAddrCfg.ipAddr << " port: "   << servAddrCfg.port   << " protocol: udp/tcp";

    info = ss.str();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServerHelper::PrintSipRegServInfo(const MySipServRegAddrCfg_dt& sipRegServAddrCfg, std::string& info)
{
    std::stringstream ss;
    ss << "name: " << sipRegServAddrCfg.name   << " domain: " << sipRegServAddrCfg.domain << " id: " << sipRegServAddrCfg.id
       << " ip: "  << sipRegServAddrCfg.ipAddr << " port: "   << sipRegServAddrCfg.port   << " protocol: udp/tcp";

    info = ss.str();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServerHelper::PrintSipUpRegServInfo(const MySipRegUpServCfg_dt& sipUpRegServCfg, std::string& info)
{
    return MyServerHelper::PrintSipRegServInfo(sipUpRegServCfg.upSipServRegAddrCfg, info);
}

MyStatus_t MyServerHelper::PrintSipLowRegServInfo(const MySipRegLowServCfg_dt& sipLowRegServCfg, std::string& info)
{
    return MyServerHelper::PrintSipRegServInfo(sipLowRegServCfg.lowSipServRegAddrCfg, info);
}

MyStatus_t MyServerHelper::PrintSipServThdPoolName(const MySipServAddrCfg_dt& sipServAddrCfg, std::string& name)
{
    std::stringstream ss;
    ss << "sipServThdPool_" << sipServAddrCfg.id << "_" << sipServAddrCfg.ipAddr << "_" << sipServAddrCfg.port;

    name = ss.str();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyServerHelper::PrintHttpServInfo(const MyHttpServAddrCfg_dt& servAddrCfg, std::string& info)
{
    std::stringstream ss;
    ss << "name: " << servAddrCfg.name   << " domain: " << servAddrCfg.domain << " id: " << servAddrCfg.id
       << " ip: "  << servAddrCfg.ipAddr << " port: "   << servAddrCfg.port   << " protocol: tcp";

    info = ss.str();
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_VIEW