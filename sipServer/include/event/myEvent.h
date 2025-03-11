#pragma once
#include <string>

namespace MY_EVENT {

// 媒体流请求事件
extern const std::string kMyMediaRequestEvent; 
#define MY_MEDIA_REQ_EV_ARGS const std::string& deviceId, const MY_COMMON::MyHttpReqMediaInfo_dt& reqInfo, MY_COMMON::MyStatus_t& status, std::string& respInfo

}; // namespace MY_EVENT