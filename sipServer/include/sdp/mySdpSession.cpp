#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "sdp/mySdpParse.h"
#include "sdp/mySdpSession.h"
using namespace MY_COMMON;

namespace MY_SDP {

MyStatus_t MySdpSession::loadFrom(const std::string& sdp)
{
    MySdpSessionParse parser;
    if (MyStatus_t::SUCCESS != parser.parse(sdp)) {
        LOG(ERROR) << "parse sdp failed.";
        return MyStatus_t::FAILED;
    }

    // 会话参数加载
    m_version     = parser.getVersion();
    m_origin      = parser.getOrigin();
    m_sessionName = parser.getSessionName();
    m_time        = parser.getSessionTime();
    m_connection  = parser.getConnection();

    // 媒体参数加载
    for (auto& mediaParse : parser.m_sdpMediaVec) {
        MySdpMedia sdpMedia;
        auto mediaLine     = mediaParse.getItemClass<MySdpMediaLine>('m');
        sdpMedia.m_type       = mediaLine.m_type;
        sdpMedia.m_port       = mediaLine.m_port;
        sdpMedia.m_proto      = mediaLine.m_proto;
        sdpMedia.m_connection = mediaParse.getItemClass<MySdpConnection>('c');
        sdpMedia.m_direction  = mediaParse.getDirection();

        auto ssrcAttrVec = mediaParse.getAllItem<MySdpGB28181SSRC>('y');
        for (const auto& ssrcAttr : ssrcAttrVec) {
            sdpMedia.m_ssrcVec.push_back(ssrcAttr.m_ssrc);
        }

        auto rtpmapAttrVec = mediaParse.getAllItem<MySdpAttrRtpMap>('a', "rtpmap");
        for (const auto& rtpmapAttr : rtpmapAttrVec) {
            MySdpMedia::MySdpCodecPlan_dt codecPlan;
            codecPlan.m_pt         = rtpmapAttr.m_pt;
            codecPlan.m_codec      = rtpmapAttr.m_codec;
            codecPlan.m_sampleRate = rtpmapAttr.m_sampleRate;
            codecPlan.m_channel    = rtpmapAttr.m_channel;
            sdpMedia.m_codecPlanVec.push_back(codecPlan);
        }
        m_mediaVec.push_back(sdpMedia);
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySdpSession::toString(std::string& sdp)
{
    MySdpSessionParse sessionParser;

    // 添加会话参数
    sessionParser.addItem(std::make_shared<MySdpString<'v'>>(std::to_string(m_version)));
    sessionParser.addItem(std::make_shared<MySdpOrigin>(m_origin));
    sessionParser.addItem(std::make_shared<MySdpString<'s'>>(m_sessionName));
    sessionParser.addItem(std::make_shared<MySdpTime>(m_time));

    if (!m_connection.empty()) {
        sessionParser.addItem(std::make_shared<MySdpConnection>(m_connection));
    }

    // 添加媒体参数
    for (const auto& media : m_mediaVec) {
        MySdpParse mediaParse;
        auto mediaLinePtr = std::make_shared<MySdpMediaLine>();
        mediaLinePtr->m_type       = media.m_type;
        mediaLinePtr->m_port       = media.m_port;
        mediaLinePtr->m_proto      = media.m_proto;
        for (const auto& codecPlan : media.m_codecPlanVec) {
            mediaLinePtr->m_fmts.push_back(std::to_string(codecPlan.m_pt));
        }
        mediaParse.addItem(std::move(mediaLinePtr));

        if (!media.m_connection.empty()) {
            mediaParse.addItem(std::make_shared<MySdpConnection>(media.m_connection));
        }

        if (MySdpDirection_t::SDP_DIRECTION_INVALID != media.m_direction) {
            mediaParse.addAttr(std::make_shared<MyDirectInterfaceImp>(media.m_direction));
        }

        if (MySdpTrackType_t::SDP_TRACK_TYPE_APPLICATION != media.m_type) {
            for (const auto& codecPlan : media.m_codecPlanVec) {
                auto rtpmapPtr = std::make_shared<MySdpAttrRtpMap>();
                rtpmapPtr->m_pt         = codecPlan.m_pt;
                rtpmapPtr->m_codec      = codecPlan.m_codec;
                rtpmapPtr->m_sampleRate = codecPlan.m_sampleRate;
                rtpmapPtr->m_channel    = codecPlan.m_channel;
                mediaParse.addAttr(rtpmapPtr);
            }

            for (const auto& ssrc : media.m_ssrcVec) {
                auto ssrcPtr = std::make_shared<MySdpGB28181SSRC>();
                ssrcPtr->m_ssrc = ssrc;
                mediaParse.addItem(ssrcPtr);
            }
        }
        sessionParser.m_sdpMediaVec.push_back(mediaParse);
    }

    if (sessionParser.m_sdpMediaVec.empty()) {
        return MyStatus_t::FAILED;
    }

    sdp = sessionParser.toString();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySdpSession::checkValid() const
{
    if (0 != m_version || m_origin.empty() || m_sessionName.empty()) {
        return MyStatus_t::FAILED;
    }

    bool hasActiveMedia = false;
    for (const auto& media : m_mediaVec) {
        if (MyStatus_t::SUCCESS != media.checkValid()) {
            return MyStatus_t::FAILED;
        }

        if (MySdpTrackType_t::SDP_TRACK_TYPE_APPLICATION == media.m_type) {
            hasActiveMedia = true;
            break;
        }

        if (MySdpDirection_t::SDP_DIRECTION_SENDRECV == media.m_direction ||
            MySdpDirection_t::SDP_DIRECTION_SENDONLY == media.m_direction ||
            MySdpDirection_t::SDP_DIRECTION_RECVONLY == media.m_direction) {
            hasActiveMedia = true;
            break;
        }
    }
    return hasActiveMedia ? MyStatus_t::SUCCESS : MyStatus_t::FAILED;
}

MyStatus_t MySdpSession::getMedia(MySdpTrackType_t type, MySdpMedia& media) const
{
    for (const auto& item : m_mediaVec) {
        if (type != item.m_type) {
            continue;
        }

        media = item;
        return MyStatus_t::SUCCESS;
    }
    return MyStatus_t::FAILED;
}

} // MY_SDP