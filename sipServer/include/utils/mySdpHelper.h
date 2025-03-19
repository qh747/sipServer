#pragma once
#include <string>
#include "common/mySdpDef.h"
#include "common/myDataDef.h"
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

/**
 * @brief sdp帮助类
 */
class MySdpHelper : public MyNonConstructableAndNonCopyable
{
public:
    /**
     * @brief                       转换sdp传输方向
     * @return                      转换结果
     * @param str                   sdp传输方向字符串
     * @param direction             sdp传输方向
     */
    static MY_COMMON::MyStatus_t    ConvertToSdpDirection(const std::string& str, MY_COMMON::MySdpDirection_t& direction);

    /**
     * @brief                       转换sdp传输方向字符串
     * @return                      转换结果
     * @param direction             sdp传输方向
     * @param str                   sdp传输方向字符串
     */
    static MY_COMMON::MyStatus_t    ConvertToSdpDirectionStr(MY_COMMON::MySdpDirection_t direction, std::string& str);

    /**
     * @brief                       转换sdp角色
     * @return                      转换结果
     * @param str                   sdp角色字符串
     * @param role                  sdp角色
     */
    static MY_COMMON::MyStatus_t    ConvertToSdpRole(const std::string& str, MY_COMMON::MySdpRole_t& role);

    /**
     * @brief                       转换sdp角色字符串
     * @return                      转换结果
     * @param role                  sdp角色
     * @param str                   sdp角色字符串
     */
    static MY_COMMON::MyStatus_t    ConvertToSdpRoleStr(MY_COMMON::MySdpRole_t role, std::string& str);

    /**
     * @brief                       转换sdp类型
     * @return                      转换结果
     * @param str                   sdp类型字符串
     * @param type                  sdp类型
     */
    static MY_COMMON::MyStatus_t    ConvertToSdpType(const std::string& str, MY_COMMON::MySdpType_t& type);

    /**
     * @brief                       转换sdp类型字符串
     * @return                      转换结果
     * @param type                  sdp类型
     * @param str                   sdp类型字符串
     */
    static MY_COMMON::MyStatus_t    ConvertToSdpTypeStr(MY_COMMON::MySdpType_t type, std::string& str);

    /**
     * @brief                       转换sdp轨道类型
     * @return                      转换结果
     * @param str                   sdp轨道类型字符串
     * @param type                  sdp轨道类型
     */
    static MY_COMMON::MyStatus_t    ConvertToSdpTrackType(const std::string& str, MY_COMMON::MySdpTrackType_t& type);

    /**
     * @brief                       转换sdp轨道类型字符串
     * @return                      转换结果
     * @param type                  sdp轨道类型
     * @param str                   sdp轨道类型字符串
     */
    static MY_COMMON::MyStatus_t    ConvertToSdpTrackTypeStr(MY_COMMON::MySdpTrackType_t type, std::string& str);
};

}; // MY_UTILS