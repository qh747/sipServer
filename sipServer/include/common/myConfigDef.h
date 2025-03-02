#pragma once
#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <cstdbool>

namespace MY_COMMON {

// http url - 获取设备列表
const std::string HTTP_URL_GET_DEVICE_LIST              = "/index/getDeviceList";
// http url - 获取设备详细信息，完整url：/index/getDeviceInfo/deviceId
const std::string HTTP_URL_GET_DEVICE_INFO              = "/index/getDeviceInfo/";
// http url - 获取设备媒体流，完整url：/index/reqDeviceMedia/(video/audio)/deviceId
const std::string HTTP_URL_REQ_DEVICE_MEDIA             = "/index/reqDeviceMedia/";

/**
 * @brief 传输层协议枚举类型
 */
typedef enum class MyTransportLayerProtocolType : int
{
    //                                                  tcp协议
    TCP                                                 = 0,  
    //                                                  udp协议
    UDP                                                 = 1        

} MyTpProto_t;  

/** 
 * @brief 服务端日志配置    
 */ 
typedef struct MyServerLogConfigDataType     
{   
    //                                                  日志级别
    std::string                                         logLevel;   
    //                                                  日志路径              
    std::string                                         logPath;   
    //                                                  是否输出到控制台
    bool                                                enableOutputToConsole;   
    //                                                  是否使用不同颜色显示 
    bool                                                enableUseDiffColorDisplay; 

} MyServLogCfg_dt;  

/** 
 * @brief 服务端线程池配置    
 */ 
typedef struct MyServerThreadPoolConfigDataType     
{   
    //                                                  线程数量
    unsigned int                                        threadNum;   
    //                                                  线程优先级, 0 - 4              
    unsigned int                                        threadPriority;   
    //                                                  是否自动运行
    bool                                                enableAutoRun;   
    //                                                  是否使用不同颜色显示 
    bool                                                enableAffinity; 

} MyServThdPoolCfg_dt;  

/** 
 * @brief SIP协议栈配置 
 */ 
typedef struct MySipStackConfigDataType     
{   
    //                                                  sip协议栈大小
    std::size_t                                         sipStackSize;  
    //                                                  sip协议栈名称        
    std::string                                         sipStackName;   

} MySipStackCfg_dt;  

/** 
 * @brief SIP定时器配置 
 */ 
 typedef struct MySipTimerConfigDataType     
 {   
    //                                                  sip服务注册有效时间（秒） 
    unsigned int                                        sipServRegExpiredTimeInterval;  
    //                                                  sip本级服务判断上下级服务超时时间间隔（秒）   
    float                                               sipServRegistJugdeTimeInterval;   
 
 } MySipTimerCfg_dt;

/** 
 * @brief SIP事件线程内存配置   
 */ 
typedef struct MySipEventThreadMemoryConfigDataType     
{   
    //                                                  sip事件线程内存初始大小
    std::size_t                                         sipEvThdInitSize;  
    //                                                  sip事件线程内存增加        
    std::size_t                                         sipEvThdIncreSize;    

} MySipEvThdMemCfg_dt;  

/** 
 * @brief sip服务端地址配置 
 */ 
typedef struct MySipServerAddressConfigDataType     
{   
    //                                                  服务端ID
    std::string                                         id;
    //                                                  服务端IP地址
    std::string                                         ipAddr;    
    //                                                  服务端端口号
    uint16_t                                            port; 
    //                                                  服务端注册使用端口号
    uint16_t                                            regPort; 
    //                                                  服务端名称
    std::string                                         name;  
    //                                                  服务端域名
    std::string                                         domain;      

} MySipServAddrCfg_dt;

/** 
 * @brief http服务端地址配置 
 */ 
typedef struct MyHttpServerAddressConfigDataType     
{   
    //                                                  服务端ID
    std::string                                         id;
    //                                                  服务端IP地址
    std::string                                         ipAddr;    
    //                                                  服务端端口号
    uint16_t                                            port; 
    //                                                  服务端名称
    std::string                                         name;  
    //                                                  服务端域名
    std::string                                         domain;      

} MyHttpServAddrCfg_dt;  

/** 
 * @brief sip服务端注册地址配置 
 */ 
typedef struct MySipServerRegisterAddressConfigDataType     
{   
    //                                                  注册服务端ID
    std::string                                         id;
    //                                                  注册服务端IP地址
    std::string                                         ipAddr;    
    //                                                  注册服务端端口号
    uint16_t                                            port; 
    //                                                  注册服务端名称
    std::string                                         name;  
    //                                                  注册服务端域名
    std::string                                         domain;      

} MySipServRegAddrCfg_dt;  

/** 
 * @brief SIP服务鉴权配置   
 */ 
typedef struct {    
    //                                                  启用鉴权
    bool                                                enableAuth;
    //                                                  鉴权名称
    std::string                                         authName;   
    //                                                  鉴权密码
    std::string                                         authPwd;
    //                                                  鉴权域名
    std::string                                         authRealm;

} MySipServAuthCfg_dt;  

/** 
 * @brief SIP上级注册服务配置   
 */ 
typedef struct MySipRegisterUpperServerConfigDataType     
{   
    //                                                  SIP上级注册服务地址配置
    MySipServRegAddrCfg_dt                              upSipServRegAddrCfg;  
    //                                                  SIP上级注册服务鉴权配置
    MySipServAuthCfg_dt                                 upSipServRegAuthCfg;
    //                                                  传输层协议类型
    MyTpProto_t                                         proto;

} MySipRegUpServCfg_dt;     

/** 
 * @brief SIP下级注册服务配置   
 */ 
typedef struct MySipRegisterLowerServerConfigDataType     
{   
    //                                                  SIP下级注册服务地址配置
    MySipServRegAddrCfg_dt                              lowSipServRegAddrCfg;  
    //                                                  SIP下级注册服务鉴权配置
    MySipServAuthCfg_dt                                 lowSipServRegAuthCfg;
    //                                                  传输层协议类型
    MyTpProto_t                                         proto;

} MySipRegLowServCfg_dt;    

/** 
 * @brief SIP注册服务配置   
 */ 
typedef struct MySipRegisterServerConfigDataType  
{   
    //                                                  本机SIP服务ID
    std::string                                         localServId;  
    //                                                  SIP上级注册服务map, key = 上级服务ID, value = 上级服务配置
    std::map<std::string, MySipRegUpServCfg_dt>         upRegSipServMap;
    //                                                  SIP下级注册服务map, key = 下级服务ID, value = 下级服务配置
    std::map<std::string, MySipRegLowServCfg_dt>        lowRegSipServMap;

} MySipRegServCfg_dt;   

/** 
 * @brief SIP设备配置   
 */ 
typedef struct MySipDeviceConfigDataType    
{   
    //                                                  设备名称
    std::string                                         name;        
    //                                                  设备状态（ON/OFF）
    std::string                                         status; 
    //                                                  父设备标识（0表示没有父设备，1表示有父设备）
    std::string                                         parental;      
    //                                                  设备制造商        
    std::string                                         manufacturer;  
    //                                                  设备型号
    std::string                                         model;         
    //                                                  设备所属模块（如video）
    std::string                                         block;      
    //                                                  安全方式（0表示无安全认证，1表示有安全认证）
    std::string                                         safetyWay;            
    //                                                  注册方式（1表示SIP注册）
    std::string                                         registerWay;       
    //                                                  保密属性（0表示不保密，1表示保密）
    std::string                                         secrecy;      
    //                                                  设备唯一标识符
    std::string                                         deviceID;      
    //                                                  父设备ID
    std::string                                         parentID;     
    //                                                  平台ID
    std::string                                         platformID;
    //                                                  所属者
    std::string                                         owner;   
    //                                                  设备IP地址
    std::string                                         deviceIp;  
    //                                                  设备端口号
    std::string                                         devicePort;
    //                                                  设备通道号
    std::string                                         deviceChannel; 
    //                                                  设备流类型（如main）
    std::string                                         deviceStream; 
    //                                                  设备经度
    std::string                                         longitude;  
    //                                                  设备纬度
    std::string                                         latitude; 

    bool operator==(const MySipDeviceConfigDataType& other) const {
        if      (status         != other.status)         return false;
        else if (name           != other.name)           return false;
        else if (deviceID       != other.deviceID)       return false;
        else if (parental       != other.parental)       return false;
        else if (manufacturer   != other.manufacturer)   return false;
        else if (model          != other.model)          return false;
        else if (block          != other.block)          return false;
        else if (safetyWay      != other.safetyWay)      return false;
        else if (registerWay    != other.registerWay)    return false;
        else if (secrecy        != other.secrecy)        return false;
        else if (parentID       != other.parentID)       return false;
        else if (platformID     != other.platformID)     return false;
        else if (owner          != other.owner)          return false;
        else if (deviceIp       != other.deviceIp)       return false;
        else if (devicePort     != other.devicePort)     return false;
        else if (deviceChannel  != other.deviceChannel)  return false;
        else if (deviceStream   != other.deviceStream)   return false;
        else if (longitude      != other.longitude)      return false; 
        else if (latitude       != other.latitude)       return false;     
        else                                             return true;    
    }

    bool operator!=(const MySipDeviceConfigDataType& other) const {
        return !(*this == other);
    }

} MySipCatalogDeviceCfg_dt;     

/** 
 * @brief SIP虚拟子平台认证配置 
 */ 
typedef struct MySipVirtualSubPlatformAuthConfigDataType 
{   
    //                                                  认证端口号
    std::string                                         portNum; 

} MySipCatalogVirtualSubPlatAuthCfg_dt; 

/** 
 * @brief SIP虚拟子平台配置 
 */ 
typedef struct MySipVirtualSubPlatformConfigDataType     
{   
    //                                                  设备名称
    std::string                                         name;        
    //                                                  设备状态（ON/OFF）
    std::string                                         status; 
    //                                                  父设备标识（0表示没有父设备，1表示有父设备）
    std::string                                         parental;      
    //                                                  设备制造商        
    std::string                                         manufacturer;  
    //                                                  设备型号
    std::string                                         model;         
    //                                                  设备所属模块（如video）
    std::string                                         block;      
    //                                                  安全方式（0表示无安全认证，1表示有安全认证）
    std::string                                         safetyWay;            
    //                                                  注册方式（1表示SIP注册）
    std::string                                         registerWay;       
    //                                                  保密属性（0表示不保密，1表示保密）
    std::string                                         secrecy;
    //                                                  设备唯一标识符
    std::string                                         deviceID;      
    //                                                  父设备ID
    std::string                                         parentID;
    //                                                  平台ID
    std::string                                         platformID;       
    //                                                  平台IP地址
    std::string                                         platformIp;    
    //                                                  平台端口号
    std::string                                         platformPort;
    //                                                  所属者
    std::string                                         owner;
    //                                                  设备IP地址
    std::string                                         deviceIp;  
    //                                                  设备端口号
    std::string                                         devicePort;
    //                                                  设备通道号
    std::string                                         deviceChannel; 
    //                                                  设备流类型（如main）
    std::string                                         deviceStream; 
    //                                                  认证信息列表
    std::vector<MySipCatalogVirtualSubPlatAuthCfg_dt>   authVec; 
    //                                                  设备经度
    std::string                                         longitude;  
    //                                                  设备纬度
    std::string                                         latitude;

    bool operator==(const MySipVirtualSubPlatformConfigDataType& other) const {
        if      (status         != other.status)         return false;
        else if (name           != other.name)           return false;
        else if (deviceID       != other.deviceID)       return false;
        else if (parental       != other.parental)       return false;
        else if (manufacturer   != other.manufacturer)   return false;
        else if (model          != other.model)          return false;
        else if (block          != other.block)          return false;
        else if (safetyWay      != other.safetyWay)      return false;
        else if (registerWay    != other.registerWay)    return false;
        else if (secrecy        != other.secrecy)        return false;
        else if (parentID       != other.parentID)       return false;
        else if (platformID     != other.platformID)     return false;
        else if (platformIp     != other.platformIp)     return false;
        else if (platformPort   != other.platformPort)   return false;
        else if (owner          != other.owner)          return false;
        else if (deviceIp       != other.deviceIp)       return false;
        else if (devicePort     != other.devicePort)     return false;
        else if (deviceChannel  != other.deviceChannel)  return false;
        else if (deviceStream   != other.deviceStream)   return false;
        else if (longitude      != other.longitude)      return false; 
        else if (latitude       != other.latitude)       return false;     
        else                                             return true;    
    }

    bool operator!=(const MySipVirtualSubPlatformConfigDataType& other) const {
        return !(*this == other);
    }

} MySipCatalogVirtualSubPlatCfg_dt; 

/** 
 * @brief SIP子平台配置 
 */ 
typedef struct MySipSubPlatformConfigDataType   
{   
    //                                                  设备名称
    std::string                                         name;        
    //                                                  设备状态（ON/OFF）
    std::string                                         status; 
    //                                                  父设备标识（0表示没有父设备，1表示有父设备）
    std::string                                         parental;      
    //                                                  设备制造商        
    std::string                                         manufacturer;  
    //                                                  设备型号
    std::string                                         model;         
    //                                                  设备所属模块（如video）
    std::string                                         block;      
    //                                                  安全方式（0表示无安全认证，1表示有安全认证）
    std::string                                         safetyWay;            
    //                                                  注册方式（1表示SIP注册）
    std::string                                         registerWay;       
    //                                                  保密属性（0表示不保密，1表示保密）
    std::string                                         secrecy;
    //                                                  设备ID
    std::string                                         deviceID;   
    //                                                  父设备ID
    std::string                                         parentID; 
    //                                                  平台ID
    std::string                                         platformID;    
    //                                                  平台IP地址
    std::string                                         platformIp;  
    //                                                  平台端口号
    std::string                                         platformPort;
    //                                                  所属者
    std::string                                         owner;
    //                                                  设备IP地址
    std::string                                         deviceIp;  
    //                                                  设备端口号
    std::string                                         devicePort;
    //                                                  设备通道号
    std::string                                         deviceChannel; 
    //                                                  设备流类型（如main）
    std::string                                         deviceStream; 
    //                                                  设备经度
    std::string                                         longitude;  
    //                                                  设备纬度
    std::string                                         latitude;

    bool operator==(const MySipSubPlatformConfigDataType& other) const {
        if      (status         != other.status)         return false;
        else if (name           != other.name)           return false;
        else if (deviceID       != other.deviceID)       return false;
        else if (parental       != other.parental)       return false;
        else if (manufacturer   != other.manufacturer)   return false;
        else if (model          != other.model)          return false;
        else if (block          != other.block)          return false;
        else if (safetyWay      != other.safetyWay)      return false;
        else if (registerWay    != other.registerWay)    return false;
        else if (secrecy        != other.secrecy)        return false;
        else if (parentID       != other.parentID)       return false;
        else if (platformID     != other.platformID)     return false;
        else if (platformIp     != other.platformIp)     return false;
        else if (platformPort   != other.platformPort)   return false;
        else if (owner          != other.owner)          return false;
        else if (deviceIp       != other.deviceIp)       return false;
        else if (devicePort     != other.devicePort)     return false;
        else if (deviceChannel  != other.deviceChannel)  return false;
        else if (deviceStream   != other.deviceStream)   return false;
        else if (longitude      != other.longitude)      return false; 
        else if (latitude       != other.latitude)       return false;     
        else                                             return true;    
    }

    bool operator!=(const MySipSubPlatformConfigDataType& other) const {
        return !(*this == other);
    }

} MySipCatalogSubPlatCfg_dt;    

/** 
 * @brief SIP平台配置   
 */ 
typedef struct MySipPlatformConfigDataType  
{   
    //                                                  设备名称
    std::string                                         name;        
    //                                                  设备状态（ON/OFF）
    std::string                                         status; 
    //                                                  父设备标识（0表示没有父设备，1表示有父设备）
    std::string                                         parental;      
    //                                                  设备制造商        
    std::string                                         manufacturer;  
    //                                                  设备型号
    std::string                                         model;         
    //                                                  设备所属模块（如video）
    std::string                                         block;      
    //                                                  安全方式（0表示无安全认证，1表示有安全认证）
    std::string                                         safetyWay;            
    //                                                  注册方式（1表示SIP注册）
    std::string                                         registerWay;       
    //                                                  保密属性（0表示不保密，1表示保密）
    std::string                                         secrecy;
    //                                                  设备ID
    std::string                                         deviceID;   
    //                                                  父设备ID
    std::string                                         parentID; 
    //                                                  平台ID
    std::string                                         platformID;    
    //                                                  平台IP地址
    std::string                                         platformIp;  
    //                                                  平台端口号
    std::string                                         platformPort;
    //                                                  所属者
    std::string                                         owner;
    //                                                  设备IP地址
    std::string                                         deviceIp;  
    //                                                  设备端口号
    std::string                                         devicePort;
    //                                                  设备通道号
    std::string                                         deviceChannel; 
    //                                                  设备流类型（如main）
    std::string                                         deviceStream; 
    //                                                  设备经度
    std::string                                         longitude;  
    //                                                  设备纬度
    std::string                                         latitude;

    bool operator==(const MySipPlatformConfigDataType& other) const {
        if      (status         != other.status)         return false;
        else if (name           != other.name)           return false;
        else if (deviceID       != other.deviceID)       return false;
        else if (parental       != other.parental)       return false;
        else if (manufacturer   != other.manufacturer)   return false;
        else if (model          != other.model)          return false;
        else if (block          != other.block)          return false;
        else if (safetyWay      != other.safetyWay)      return false;
        else if (registerWay    != other.registerWay)    return false;
        else if (secrecy        != other.secrecy)        return false;
        else if (parentID       != other.parentID)       return false;
        else if (platformID     != other.platformID)     return false;
        else if (platformIp     != other.platformIp)     return false;
        else if (platformPort   != other.platformPort)   return false;
        else if (owner          != other.owner)          return false;
        else if (deviceIp       != other.deviceIp)       return false;
        else if (devicePort     != other.devicePort)     return false;
        else if (deviceChannel  != other.deviceChannel)  return false;
        else if (deviceStream   != other.deviceStream)   return false;
        else if (longitude      != other.longitude)      return false; 
        else if (latitude       != other.latitude)       return false;     
        else                                             return true;    
    }

    bool operator!=(const MySipPlatformConfigDataType& other) const {
        return !(*this == other);
    }

} MySipCatalogPlatCfg_dt;

/** 
 * @brief SIP设备目录配置   
 */ 
typedef struct MySipCatalogConfigDataType  
{    
    //                                                          SIP设备目录平台配置
    MySipCatalogPlatCfg_dt                                      catalogPlatCfg;
    //                                                          SIP设备目录子平台配置map，key = 子平台ID, value = 子平台配置
    std::map<std::string, MySipCatalogSubPlatCfg_dt>            catalogSubPlatCfgMap;
    //                                                          SIP设备目录虚拟子平台配置map，key = 虚拟子平台ID, value = 虚拟子平台配置
    std::map<std::string, MySipCatalogVirtualSubPlatCfg_dt>     catalogVirtualSubPlatCfgMap;
    //                                                          SIP设备目录设备配置map，key = 设备ID, value = 设备配置
    std::map<std::string, MySipCatalogDeviceCfg_dt>             catalogDeviceCfgMap;

} MySipCatalogCfg_dt;   


}; // MY_COMMON