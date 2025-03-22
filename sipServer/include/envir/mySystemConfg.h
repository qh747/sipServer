#pragma once
#include <string>
#include <boost/thread/shared_mutex.hpp>
#include "common/myTypeDef.h"
#include "utils/myBaseHelper.h"

namespace MY_ENVIR {

/**
 * 系统配置类
 */
class MySystemConfig : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    /**
     * @brief                                               初始化
     * @return                                              MyStatus_t状态码
     * @param path                                          配置文件路径
     */                                                 
    static MY_COMMON::MyStatus_t                            Init(const std::string& path);

public:                     
    //                                                      获取服务端日志配置
    static MY_COMMON::MyStatus_t                            GetServerLogCfg(MY_COMMON::MyServLogCfg_dt& cfg);

    //                                                      获取服务端线程池配置
    static MY_COMMON::MyStatus_t                            GetServerThdPoolCfg(MY_COMMON::MyServThdPoolCfg_dt& cfg);

    //                                                      获取SIP协议栈配置
    static MY_COMMON::MyStatus_t                            GetSipStackCfg(MY_COMMON::MySipStackCfg_dt& cfg);

    //                                                      获取SIP服务注册有效时间（秒）
    static MY_COMMON::MyStatus_t                            GetSipServRegExpiredTimeInterval(unsigned int& timeInterval);

    //                                                      sip本级服务判断上下级服务超时时间间隔（秒）
    static MY_COMMON::MyStatus_t                            GetSipServRegistJudgeTimeInterval(float& timeInterval);

    //                                                      获取sip事件线程内存配置
    static MY_COMMON::MyStatus_t                            GetSipEvThdMemCfg(MY_COMMON::MySipEvThdMemCfg_dt& cfg);

    //                                                      获取sip服务地址配置
    static MY_COMMON::MyStatus_t                            GetSipServAddrCfg(MY_COMMON::MySipServAddrCfg_dt& cfg);

    //                                                      获取http服务地址配置
    static MY_COMMON::MyStatus_t                            GetHttpServAddrCfg(MY_COMMON::MyHttpServAddrCfg_dt& cfg);
                        
    //                                                      获取SIP上级注册服务配置                 
    static MY_COMMON::MyStatus_t                            GetSipUpRegServCfgMap(MY_COMMON::MySipRegUpServCfgMap& cfg);
                        
    //                                                      获取SIP下级注册服务配置                 
    static MY_COMMON::MyStatus_t                            GetSipLowRegServCfgMap(MY_COMMON::MySipRegLowServCfgMap& cfg);

    //                                                      获取SIP设备目录配置
    static MY_COMMON::MyStatus_t                            GetSipCatalogCfg(MY_COMMON::MySipCatalogCfg_dt& cfg);

    //                                                      获取SIP SDP配置
    static MY_COMMON::MyStatus_t                            GetSipSdpCfg(MY_COMMON::MySipSdpCfg_dt& cfg);

private:
    //                                                      配置文件读写锁
    static boost::shared_mutex                              CfgMutex;

    //                                                      服务端日志配置
    static MY_COMMON::MyServLogCfg_dt                       ServLogCfg;

    //                                                      服务端线程池配置
    static MY_COMMON::MyServThdPoolCfg_dt                   ServThdPoolCfg;

    //                                                      sip协议栈配置
    static MY_COMMON::MySipStackCfg_dt                      SipStackCfg;

    //                                                      sip定时器配置
    static MY_COMMON::MySipTimerCfg_dt                      SipTimerCfg;

    //                                                      sip事件线程内存配置
    static MY_COMMON::MySipEvThdMemCfg_dt                   SipEvThdMemCfg;

    //                                                      sip服务端地址配置
    static MY_COMMON::MySipServAddrCfg_dt                   SipServAddrCfg;

    //                                                      http服务端地址配置
    static MY_COMMON::MyHttpServAddrCfg_dt                  HttpServAddrCfg;

    //                                                      sip注册服务配置
    static MY_COMMON::MySipRegServCfg_dt                    SipRegServCfg;

    //                                                      sip设备目录配置
    static MY_COMMON::MySipCatalogCfg_dt                    SipCatalogCfg;

    //                                                      sip sdp配置
    static MY_COMMON::MySipSdpCfg_dt                        SipSdpCfg;
};

}; // namespace MY_ENVIR