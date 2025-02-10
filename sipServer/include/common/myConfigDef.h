#pragma once
#include <map>
#include <string>
#include <cstdint>
#include <cstdbool>

namespace MY_COMMON {

/**
 * @brief 传输层协议枚举类型
 */
typedef enum class MyTransportLayerProtocolType : int
{
    //                                              tcp协议
    TCP                                             = 0,  

    //                                              udp协议
    UDP                                             = 1,           

} MyTpProto_t;

/**
 * @brief 服务端日志配置
 */
typedef struct MySipServerLogConfigDataType 
{
    //                                              日志级别
    std::string                                     logLevel;   

    //                                              日志路径              
    std::string                                     logPath;   

    //                                              是否输出到控制台
    bool                                            enableOutputToConsole;   

    //                                              是否使用不同颜色显示 
    bool                                            enableUseDiffColorDisplay;  

} MyServLogCfg_dt; 

/**
 * @brief SIP协议栈配置
 */
typedef struct MySipStackConfigDataType 
{
    //                                              sip协议栈大小
    std::size_t                                     sipStackSize;  

    //                                              sip协议栈名称        
    std::string                                     sipStackName;               

} MySipStackCfg_dt; 

/**
 * @brief SIP事件线程内存配置
 */
typedef struct MySipEventThreadMemoryConfigDataType 
{
    //                                              sip事件线程内存初始大小
    std::size_t                                     sipEvThdInitSize;  

    //                                              sip事件线程内存增加        
    std::size_t                                     sipEvThdIncreSize;            

} MySipEvThdMemCfg_dt; 

/**
 * @brief sip服务端地址配置
 */
typedef struct MySipServerAddressConfigDataType 
{
    //                                              服务端ID
    std::string                                     id;

    //                                              服务端IP地址
    std::string                                     ipAddr;    

    //                                              服务端端口号
    uint16_t                                        port; 

    //                                              服务端注册使用端口号
    uint16_t                                        regPort; 

    //                                              服务端名称
    std::string                                     name;  

    //                                              服务端域名
    std::string                                     domain;                     

} MySipServAddrCfg_dt; 

/**
 * @brief SIP服务鉴权配置
 */
typedef struct {
    //                                              启用鉴权
    bool                                            enableAuth;

    //                                              鉴权名称
    std::string                                     authName;   

    //                                              鉴权密码
    std::string                                     authPwd;

    //                                              鉴权域名
    std::string                                     authRealm;

} MySipServAuthCfg_dt;

/**
 * @brief SIP上级注册服务配置
 */
typedef struct MySipRegistUpperServerConfigDataType 
{
    //                                              SIP上级注册服务地址配置
    MySipServAddrCfg_dt                             upSipServAddrCfg;  

    //                                              SIP上级注册服务鉴权配置
    MySipServAuthCfg_dt                             upSipServAuthCfg;

    //                                              传输层协议类型
    MyTpProto_t                                     proto;

} MySipRegUpServCfg_dt; 

/**
 * @brief SIP下级注册服务配置
 */
typedef struct MySipRegistLowerServerConfigDataType 
{
    //                                              SIP下级注册服务地址配置
    MySipServAddrCfg_dt                             lowSipServAddrCfg;  

    //                                              SIP下级注册服务鉴权配置
    MySipServAuthCfg_dt                             lowSipServAuthCfg;

    //                                              传输层协议类型
    MyTpProto_t                                     proto;

} MySipRegLowServCfg_dt; 

/**
 * @brief SIP注册服务配置
 */
typedef struct MySipRegistServerConfigDataType 
{
    //                                              本机SIP服务ID
    std::string                                     localServId;  

    //                                              SIP上级注册服务map
    std::map<std::string, MySipRegUpServCfg_dt>     upRegSipServMap;

    //                                              SIP下级注册服务map
    std::map<std::string, MySipRegLowServCfg_dt>    lowRegSipServMap;

} MySipRegServCfg_dt; 


}; // MY_COMMON