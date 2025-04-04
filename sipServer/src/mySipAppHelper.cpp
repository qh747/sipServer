#include <sstream>
#include "utils/mySipAppHelper.h"
using namespace MY_COMMON;

namespace MY_UTILS {

MyStatus_t MySipAppHelper::GetSipAppInfo(const MySipAppIdCfg_dt& sipAppId, std::string& info)
{
    std::stringstream ss;
    ss << "SipApp id: " << sipAppId.id << ", name: " << sipAppId.name << ", priority: " << sipAppId.priority;

    info = ss.str();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipAppHelper::GetSipAppThdPoolName(const MY_COMMON::MySipAppIdCfg_dt& sipAppId, std::string& name)
{
    std::stringstream ss;
    ss << "sipAppThdPool_" << sipAppId.id << "_" << sipAppId.name;

    name = ss.str();
    return MyStatus_t::SUCCESS;
}

} // namespace MY_UTILS