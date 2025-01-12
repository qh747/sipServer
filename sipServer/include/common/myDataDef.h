#pragma once
#include <string>
#include <cstdint>
#include <cstdbool>

namespace MY_COMMON {

// sipServer事件处理线程内存大小(1M)
const std::size_t SIPSERV_EVTRD_INIT_MEM_SIZE     = 1024 * 1024 * 1; 

// sipServer事件处理线程增量内存大小(1M)
const std::size_t SIPSERV_EVTRD_INCREM_MEM_SIZE   = 1024 * 1024 * 1; 

/**
 * @brief 自定义返回值枚举类型
 */
typedef enum class MyStatusType : int
{
    //                              成功
    SUCCESS                         = 0,  
    //                              失败
    FAILED                          = -1,           

} MyStatus_t;

/**
 * @brief 服务端地址配置
 */
typedef struct MyServerAddrConfigDataType 
{
    //                              服务端ID
    std::string                     id;
    //                              服务端IP地址
    std::string                     ipAddr;    
    //                              服务端端口号
    uint16_t                        port;  
    //                              服务端名称
    std::string                     name;      
    //                              服务端域名
    std::string                     domain;                     

} MyServerAddrConfig_dt; 

/**
 * @brief 服务端日志配置
 */
typedef struct MyServerLogConfigDataType 
{
    //                              日志级别
    std::string                     logLevel;     
    //                              日志路径              
    std::string                     logPath;    
    //                              是否输出到控制台
    bool                            enableOutputToConsole;     
    //                              是否使用不同颜色显示 
    bool                            enableUseDiffColorDisplay;  

} MyServerLogConfig_dt; 

/**
 * @brief 服务端线程池配置
 */
typedef struct MyServerThreadConfigDataType 
{
    //                              初始线程数量
    std::size_t                     initIhreadCount;            

} MyServerThreadConfig_dt; 

/**
 * @brief SIP协议栈配置
 */
typedef struct MySipStackConfigDataType 
{
    //                              sip协议栈大小
    std::size_t                     sipStackSize;       
    //                              sip协议栈名称        
    std::string                     sipStackName;               

} MySipStackConfig_dt; 

/**
 * @brief SIP上级注册服务信息
 */
typedef struct MySipRegUpServInfoDataType 
{
    //                              服务端ID
    std::string                     id;
    //                              服务端IP地址
    std::string                     ipAddr;    
    //                              服务端端口号
    uint16_t                        port;  
    //                              服务端名称
    std::string                     name;      
    //                              服务端域名
    std::string                     domain;   
    //                              注册时长
    double                          duration;             

} MySipRegUpServInfo_dt; 

/**
 * @brief SIP上级注册服务结果
 */
typedef struct MySipRegUpServResultDataType 
{
    //                              服务端ID
    std::string                     id;
    //                              服务端IP地址
    std::string                     ipAddr;    
    //                              服务端端口号
    uint16_t                        port;  
    //                              服务端名称
    std::string                     name;      
    //                              服务端域名
    std::string                     domain;  
    //                              注册结果
    MyStatus_t                      result;
    //                              注册时长
    double                          duration;
    //                              发起注册时间, 格式为: YYYY-MM-DD HH:MM:SS
    std::string                     time;

} MySipRegUpServResult_dt; 

/**
 * @brief SIP下级注册服务信息
 */
typedef struct MySipRegLowServInfoDataType 
{
    //                              服务端ID
    std::string                     id;
    //                              服务端IP地址
    std::string                     ipAddr;    
    //                              服务端端口号
    uint16_t                        port;  
    //                              服务端名称
    std::string                     name;      
    //                              服务端域名
    std::string                     domain;    
    //                              注册时长
    double                          duration;            

} MySipRegLowServInfo_dt; 

/**
 * @brief SIP下级注册服务结果
 */
typedef struct MySipRegLowServResultDataType 
{
    //                              服务端ID
    std::string                     id;
    //                              服务端IP地址
    std::string                     ipAddr;    
    //                              服务端端口号
    uint16_t                        port;  
    //                              服务端名称
    std::string                     name;      
    //                              服务端域名
    std::string                     domain;  
    //                              注册结果
    MyStatus_t                      result;
    //                              注册时长
    double                          duration;
    //                              发起注册时间, 格式为: YYYY-MM-DD HH:MM:SS
    std::string                     time;

} MySipRegLowServResult_dt; 

}; // MY_COMMON