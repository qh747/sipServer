#pragma once
#include <string>

namespace MY_EVENT {

// 媒体流请求事件
extern const std::string kMyMediaRequestEvent; 
#define MY_MEDIA_REQ_EV_ARGS const std::string& deviceId, MY_COMMON::MyStatus_t& status, std::string& info

}; // namespace MY_EVENT