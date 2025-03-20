#pragma once
#include <string>
#include "common/myDataDef.h"
#include "common/mySdpDef.h"
#include "sdp/mySdpAttr.h"
#include "sdp/mySdpMedia.h"

namespace MY_SDP {

/**
 * @brief sdp会话类
 */
class MySdpSession
{
public:
    /**
     * @brief                       从sdp文本加载
     * @return                      有效值
     * @param sdp                   sdp文本
     */
    MY_COMMON::MyStatus_t           loadFrom(const std::string& sdp);

    /**
     * @brief                       生成sdp文本
     * @return                      有效值
     * @param sdp                   sdp文本
     */
    MY_COMMON::MyStatus_t           toString(std::string& sdp);

    /**
     * @brief                       检查是否有效
     * @return                      有效值
     */
    MY_COMMON::MyStatus_t           checkValid() const;

    /**
     * @brief                       获取媒体信息
     * @return                      有效值
     * @param type                  媒体类型
     * @param media                 媒体信息
     */
    MY_COMMON::MyStatus_t           getMedia(MY_COMMON::MySdpTrackType_t type, MySdpMedia& media) const;

public:
    uint32_t                        m_version;
    MySdpOrigin                     m_origin;
    std::string                     m_sessionName;
    MySdpTime                       m_time;
    MySdpConnection                 m_connection;
    MySdpMedia::Vec                 m_mediaVec;
};

}; // MY_SDP
