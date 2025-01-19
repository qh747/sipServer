#include <sstream>
#include "utils/mySipAppHelper.h"
using MY_COMMON::MySipAppIdCfg_dt;

namespace MY_UTILS {

std::string MySipAppHelper::GetSipAppInfo(const MySipAppIdCfg_dt& sipAppId)
{
    std::stringstream ss;
    ss << "SipApp id: " << sipAppId.id << ", name: " << sipAppId.name << ", priority: " << sipAppId.priority;

    return ss.str();
}

}; // namespace MY_UTILS