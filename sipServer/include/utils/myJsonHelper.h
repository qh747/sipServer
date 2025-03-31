#pragma once
#include <string>
#include "common/myDataDef.h"
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

/**
 * json帮助类
 */
class MyJsonHelper : public MyNonConstructableAndNonCopyable
{
public:
    /** 
     * @brief                       sip注册json文件解析
     * @return                      解析结果，0-success, -1-failed
     * @param filePath              json文件路径
     * @param regCfg                sip注册配置
     */
    static MY_COMMON::MyStatus_t    ParseSipRegJsonFile(const std::string& filePath, MY_COMMON::MySipRegServCfg_dt& regCfg);

    /** 
     * @brief                       sip服务设备目录json文件解析
     * @return                      解析结果，0-success, -1-failed
     * @param filePath              json文件路径
     * @param catalogCfg            设备目录配置
     */
    static MY_COMMON::MyStatus_t    ParseSipCatalogJsonFile(const std::string& filePath, MY_COMMON::MySipCatalogCfg_dt& catalogCfg);

    /**
     * @brief                       请求播放媒体解析
     * @return                      解析结果，0-success, -1-failed
     * @param buf                   请求数据
     * @param reqInfo               解析内容
     */
    static MY_COMMON::MyStatus_t    ParseHttpReqMediaInfo(const std::string& buf, MY_COMMON::MyHttpReqMediaInfo_dt& reqInfo);

public:
    /**
     * @brief                       生成http错误信息
     * @return                      生成结果，0-success, -1-failed
     * @param errInfo               错误信息
     * @param msgBody               错误消息内容
     */
    static MY_COMMON::MyStatus_t    GenerateHttpErrMsgBody(const std::string& errInfo, std::string& msgBody);

    /**
     * @brief                       生成sip目录信息列表
     * @return                      生成结果，0-success, -1-failed
     * @param deviceInfoMap         sip目录信息map
     * @param deviceInfoStr         sip目录信息字符串
     */
    static MY_COMMON::MyStatus_t    GenerateDeviceListInfo(const std::map<std::string, MY_COMMON::MySipCatalogInfo_dt>& deviceInfoMap, std::string& deviceInfoStr);

    /**
     * @brief                       生成sip设备信息
     * @return                      生成结果，0-success, -1-failed
     * @param deviceInfo            sip设备信息
     * @param deviceInfoStr         sip设备信息字符串
     */
    static MY_COMMON::MyStatus_t    GenerateDeviceInfo(const MY_COMMON::MySipCatalogPlatCfg_dt& deviceInfo, std::string& deviceInfoStr);

    /**
     * @brief                       生成sip设备信息
     * @return                      生成结果，0-success, -1-failed
     * @param deviceInfo            sip设备信息
     * @param deviceInfoStr         sip设备信息字符串
     */
    static MY_COMMON::MyStatus_t    GenerateDeviceInfo(const MY_COMMON::MySipCatalogSubPlatCfg_dt& deviceInfo, std::string& deviceInfoStr);

    /**
     * @brief                       生成sip设备信息
     * @return                      生成结果，0-success, -1-failed
     * @param deviceInfo            sip设备信息
     * @param deviceInfoStr         sip设备信息字符串
     */
    static MY_COMMON::MyStatus_t    GenerateDeviceInfo(const MY_COMMON::MySipCatalogVirtualSubPlatCfg_dt& deviceInfo, std::string& deviceInfoStr);

    /**
     * @brief                       生成sip设备信息
     * @return                      生成结果，0-success, -1-failed
     * @param deviceInfo            sip设备信息
     * @param deviceInfoStr         sip设备信息字符串
     */
    static MY_COMMON::MyStatus_t    GenerateDeviceInfo(const MY_COMMON::MySipCatalogDeviceCfg_dt& deviceInfo, std::string& deviceInfoStr);
};

}; // namespace MY_UTILS