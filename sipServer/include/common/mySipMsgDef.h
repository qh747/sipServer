#pragma once
#include <string>
#include <cstdbool>

namespace MY_COMMON {

/**
 * @brief sip keepalive消息内容
 */
typedef struct MySipKeepAliveMsgBodyDataType 
{
    //              消息类型cmdType
    std::string     cmdType;

    //              下级服务id DeviceID
    std::string     deviceId;

    //              保活索引SN
    std::string     sn;

    //              保活状态Status
    bool            status = false;                 

} MySipKeepAliveMsgBidy_dt; 


}; // namespace MY_COMMON