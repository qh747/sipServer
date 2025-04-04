#pragma once
#include <vector>
#include <string>
#include "common/myDataDef.h"
#include "common/mySdpDef.h"
#include "sdp/mySdpAttr.h"

namespace MY_SDP {

/**
 * @brief sdp媒体类
 */
class MySdpMedia
{
public:
    using Ptr    = std::shared_ptr<MySdpMedia>;
    using PtrVec = std::vector<MySdpMedia::Ptr>;

public:
    /**
     * @brief sdp编解码类
     */
    typedef struct MySdpCodecPlanDataType {
        uint8_t     m_pt {0};
        std::string m_codec {""};
        uint32_t    m_sampleRate {0};
        uint32_t    m_channel {0};
    } MySdpCodecPlan_dt;

public:
    using CodecPlanVector  = std::vector<MySdpCodecPlan_dt>;
    using SSRCVec          = std::vector<uint32_t>;

public:
    /**
     * @brief                       检查是否有效
     * @return                      有效值
     */
    MY_COMMON::MyStatus_t           checkValid() const;

    /**
     * @brief                       获取编解码信息
     * @return                      有效值
     * @param pt                    编码类型
     * @param codecPlan             编解码信息
     */
    MY_COMMON::MyStatus_t           getCodecPlan(uint8_t pt, MySdpCodecPlan_dt& codecPlan) const;

    /**
     * @brief                       获取编解码信息
     * @return                      有效值
     * @param codec                 编码类型
     * @param codecPlan             编解码信息
     */
    MY_COMMON::MyStatus_t           getCodecPlan(const char* codec, MySdpCodecPlan_dt& codecPlan) const;

    /**
     * @brief                       获取RTP SSRC
     * @return                      有效值
     * @param ssrc                  RTP SSRC
     */
    MY_COMMON::MyStatus_t           getRtpSSRC(uint32_t& ssrc) const;

public:
    //                              媒体轨道类型
    MY_COMMON::MySdpTrackType_t     m_type {MY_COMMON::MySdpTrackType_t::SDP_TRACK_TYPE_INVALID};

    //                              媒体端口
    uint16_t                        m_port {0};

    //                              媒体连接信息
    MySdpConnection                 m_connection;

    //                              媒体协议
    std::string                     m_proto;

    //                              媒体角色类型
    MySdpAttrSetup                  m_setup;

    //                              媒体传输方向
    MY_COMMON::MySdpDirection_t     m_direction {MY_COMMON::MySdpDirection_t::SDP_DIRECTION_INVALID};

    //                              媒体编解码信息
    CodecPlanVector                 m_codecPlanVec;

    //                              媒体RTP SSRC
    SSRCVec                         m_ssrcVec;
};

}; // namespace MY_SDP