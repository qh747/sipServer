#include "sdp/mySdpMedia.h"
using namespace MY_COMMON;

namespace MY_SDP {

MyStatus_t MySdpMedia::checkValid() const
{
    if (MySdpTrackType_t::SDP_TRACK_TYPE_INVALID == m_type ||
        0 == m_port ||
        m_proto.empty() ||
        MySdpDirection_t::SDP_DIRECTION_INVALID == m_direction ||
        MySdpDirection_t::SDP_DIRECTION_INACTIVE == m_direction ||
        m_codecPlanVec.empty() ||
        m_ssrcVec.empty()) {
        return MyStatus_t::FAILED;
    }

    if (std::string::npos != m_proto.find("TCP") && MySdpRole_t::SDP_ROLE_INVALID == m_setup.m_role) {
        return MyStatus_t::FAILED;
    }

    return MyStatus_t::SUCCESS;
}

MyStatus_t MySdpMedia::getCodecPlan(uint8_t pt, MySdpCodecPlan_dt& codecPlan) const
{
    for (const auto& item : m_codecPlanVec) {
        if (item.m_pt != pt) {
            continue;
        }

        codecPlan = item;
        return MyStatus_t::SUCCESS;
    }
    return MyStatus_t::FAILED;
}

MyStatus_t MySdpMedia::getCodecPlan(const char* codec, MySdpCodecPlan_dt& codecPlan) const
{
    for (const auto& item : m_codecPlanVec) {
        if (0 != strcasecmp(item.m_codec.data(), codec)) {
            continue;
        }

        codecPlan = item;
        return MyStatus_t::SUCCESS;
    }
    return MyStatus_t::FAILED;
}

MyStatus_t MySdpMedia::getRtpSSRC(uint32_t& ssrc) const
{
    if (m_ssrcVec.empty()) {
        return MyStatus_t::FAILED;
    }

    ssrc = m_ssrcVec[0];
    return MyStatus_t::SUCCESS;
}

} // namespace MY_SDP