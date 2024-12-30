#pragma once
#include <string>
#include <cstdint>

namespace MY_COMMON {
/**
 * @brief 自定义返回值枚举类型
 */
typedef enum class MyStatusType : int
{
    SUCCESS = 0,            // 成功
    FAILED  = -1,           // 失败

} MyStatus_t;

/**
 * @brief 服务端配置
 */
typedef struct MyServerConfigDataType 
{
    std::string ipAddr;     // 服务端IP地址
    uint16_t    port;       // 服务端端口号
    std::string name;       // 服务端名称
    std::string domain;     // 服务端域名

} MyServerConfig_dt; 
}; // MY_COMMON