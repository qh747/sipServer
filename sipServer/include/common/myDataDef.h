#pragma once
#include <string>
#include <cstdint>
#include <cstdbool>

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
 * 线程状态类型
 */
typedef enum class ThreadStateType : std::uint8_t 
{ 
    //                              线程在阻塞  
    STOPED                          = 0,
    //                              线程在运行
    RUNNING                         = 1,
    //                              线程已终止
    TERMINATED                      = 3

} ThreadState_t; 

/**
 * @brief 服务端地址配置
 */
typedef struct MyServerAddrConfigDataType 
{
    std::string ipAddr;                     // 服务端IP地址
    uint16_t    port;                       // 服务端端口号
    std::string name;                       // 服务端名称
    std::string domain;                     // 服务端域名

} MyServerAddrConfig_dt; 

/**
 * @brief 服务端日志配置
 */
typedef struct MyServerLogConfigDataType 
{
    std::string logLevel;                   // 日志级别
    std::string logPath;                    // 日志路径
    bool        enableOutputToConsole;      // 是否输出到控制台
    bool        enableUseDiffColorDisplay;  // 是否使用不同颜色显示

} MyServerLogConfig_dt; 

/**
 * @brief 服务端线程池配置
 */
typedef struct MyServerThreadConfigDataType 
{
    std::size_t initIhreadCount;            // 初始线程数量

} MyServerThreadConfig_dt; 

/**
 * @brief SIP协议栈配置
 */
typedef struct MySipStackConfigDataType 
{
    std::size_t sipStackSize;               // sip协议栈大小
    std::string sipStackName;               // sip协议栈名称

} MySipStackConfig_dt; 


}; // MY_COMMON