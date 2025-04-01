#include <set>
#include <fstream>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "sdp/mySdpParse.h"
#include "utils/mySdpHelper.h"
#include "sdp/mySdpSession.h"
using namespace MY_COMMON;
using namespace MY_UTILS;

namespace MY_SDP {

MyStatus_t MySdpSession::CreateAnswer(const MySdpSession& remoteSdp, const MySdpSession& localSdp, MySdpSession& answerSdp)
{
    answerSdp.m_version     = remoteSdp.m_version;
    answerSdp.m_sessionName = remoteSdp.m_sessionName;
    answerSdp.m_time        = remoteSdp.m_time;

    answerSdp.m_origin                  = localSdp.m_origin;
    answerSdp.m_origin.m_username       = remoteSdp.m_origin.m_username;
    answerSdp.m_origin.m_sessionId      = remoteSdp.m_origin.m_sessionId;
    answerSdp.m_origin.m_sessionVersion = remoteSdp.m_origin.m_sessionVersion;

    if (!localSdp.m_connection.empty()) {
        answerSdp.m_connection  = localSdp.m_connection;
    }

    for (const auto& remoteMedia : remoteSdp.m_mediaPtrVec) {
        MySdpMedia::Ptr localMedia = nullptr;

        // 找到匹配的媒体类型
        for (const auto& mediaPtr : localSdp.m_mediaPtrVec) {
            if (remoteMedia->m_type == mediaPtr->m_type) {
                localMedia = mediaPtr;
            }
        }

        // 匹配失败
        if (nullptr == localMedia) {
            std::string typeStr;
            MySdpHelper::ConvertToSdpTrackTypeStr(remoteMedia->m_type, typeStr);

            LOG(WARNING) << "Match Sdp Answer warning. invalid media type:" << typeStr;
            continue;
        }

        MySdpMedia::Ptr answerMedia = std::make_shared<MySdpMedia>();
        answerMedia->m_type  = remoteMedia->m_type;
        answerMedia->m_proto = remoteMedia->m_proto;
        answerMedia->m_port  = localMedia->m_port;

        if (!localMedia->m_connection.empty()) {
            answerMedia->m_connection  = localMedia->m_connection;
        }

        if (MyStatus_t::SUCCESS != MySdpHelper::MatchSdpDirection(remoteMedia->m_direction,
            localMedia->m_direction, answerMedia->m_direction)) {
            std::string remoteDirectionStr;
            MySdpHelper::ConvertToSdpDirectionStr(remoteMedia->m_direction, remoteDirectionStr);

            std::string localDirectionStr;
            MySdpHelper::ConvertToSdpDirectionStr(localMedia->m_direction, localDirectionStr);

            LOG(WARNING) << "Match Sdp Answer warning. invalid direction. remote: " << remoteDirectionStr
                         << " local: " << localDirectionStr;
            continue;
        }

        if (MySdpRole_t::SDP_ROLE_INVALID != remoteMedia->m_setup.m_role) {
            if (MySdpRole_t::SDP_ROLE_INVALID != localMedia->m_setup.m_role) {
                answerMedia->m_setup.m_role = localMedia->m_setup.m_role;
            }
            else {
                continue;
            }
        }

        for (const auto& remoteCodecPlan : remoteMedia->m_codecPlanVec) {
            for (const auto& localCodecPlan : localMedia->m_codecPlanVec) {
                if (remoteCodecPlan.m_pt == localCodecPlan.m_pt) {
                    answerMedia->m_codecPlanVec.push_back(localCodecPlan);
                }
            }
        }

        for (const auto& ssrc : localMedia->m_ssrcVec) {
            answerMedia->m_ssrcVec.push_back(ssrc);
        }

        answerSdp.m_mediaPtrVec.push_back(answerMedia);
    }
    return answerSdp.checkValid();
}

MyStatus_t MySdpSession::loadFrom(const std::string& sdp)
{
    MySdpSessionParse parser;
    if (MyStatus_t::SUCCESS != parser.parse(sdp)) {
        LOG(ERROR) << "parse sdp failed. sdp: " << sdp << ".";
        return MyStatus_t::FAILED;
    }

    // 会话参数加载
    m_version     = parser.getVersion();
    m_origin      = parser.getOrigin();
    m_sessionName = parser.getSessionName();
    m_time        = parser.getSessionTime();

    auto conn = parser.getConnection();
    if (!conn.empty()) {
        m_connection = conn;
    }

    // 媒体参数加载
    for (auto& mediaParse : parser.m_sdpMediaVec) {
        MySdpMedia::Ptr sdpMedia = std::make_shared<MySdpMedia>();
        auto mediaLine     = mediaParse.getItemClass<MySdpMediaLine>('m');
        sdpMedia->m_type       = mediaLine.m_type;
        sdpMedia->m_port       = mediaLine.m_port;
        sdpMedia->m_proto      = mediaLine.m_proto;
        sdpMedia->m_direction  = mediaParse.getDirection();
        sdpMedia->m_setup      = mediaParse.getItemClass<MySdpAttrSetup>('a', "setup");

        auto mediaConn = mediaParse.getItemClass<MySdpConnection>('c');
        if (!mediaConn.empty()) {
            sdpMedia->m_connection = mediaConn;
        }

        auto ssrcAttrVec = mediaParse.getAllItem<MySdpGB28181SSRC>('y');
        for (const auto& ssrcAttr : ssrcAttrVec) {
            sdpMedia->m_ssrcVec.push_back(ssrcAttr.m_ssrc);
        }

        auto rtpmapAttrVec = mediaParse.getAllItem<MySdpAttrRtpMap>('a', "rtpmap");
        for (const auto& rtpmapAttr : rtpmapAttrVec) {
            MySdpMedia::MySdpCodecPlan_dt codecPlan;
            codecPlan.m_pt         = rtpmapAttr.m_pt;
            codecPlan.m_codec      = rtpmapAttr.m_codec;
            codecPlan.m_sampleRate = rtpmapAttr.m_sampleRate;
            codecPlan.m_channel    = rtpmapAttr.m_channel;
            sdpMedia->m_codecPlanVec.push_back(codecPlan);
        }
        m_mediaPtrVec.push_back(sdpMedia);
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySdpSession::loadFromFile(const std::string& fileName)
{
    std::ifstream file(fileName);
    if (!file.good()) {
        LOG(ERROR) << "parse sdp file failed. file name: " << fileName << ".";
        return MyStatus_t::FAILED;
    }

    std::string str((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
    return this->loadFrom(str);
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
    for (const auto& media : m_mediaPtrVec) {
        MySdpParse mediaParse;
        auto mediaLinePtr = std::make_shared<MySdpMediaLine>();
        mediaLinePtr->m_type       = media->m_type;
        mediaLinePtr->m_port       = media->m_port;
        mediaLinePtr->m_proto      = media->m_proto;

        for (const auto& codecPlan : media->m_codecPlanVec) {
            mediaLinePtr->m_fmts.push_back(std::to_string(codecPlan.m_pt));
        }
        mediaParse.addItem(std::move(mediaLinePtr));

        if (!media->m_connection.empty()) {
            mediaParse.addItem(std::make_shared<MySdpConnection>(media->m_connection));
        }

        if (MySdpDirection_t::SDP_DIRECTION_INVALID != media->m_direction) {
            mediaParse.addAttr(std::make_shared<MyDirectInterfaceImp>(media->m_direction));
        }

        if (MySdpRole_t::SDP_ROLE_INVALID != media->m_setup.m_role) {
            mediaParse.addAttr(std::make_shared<MySdpAttrSetup>(media->m_setup));
        }

        if (MySdpTrackType_t::SDP_TRACK_TYPE_APPLICATION != media->m_type) {
            for (const auto& codecPlan : media->m_codecPlanVec) {
                auto rtpmapPtr = std::make_shared<MySdpAttrRtpMap>();
                rtpmapPtr->m_pt         = codecPlan.m_pt;
                rtpmapPtr->m_codec      = codecPlan.m_codec;
                rtpmapPtr->m_sampleRate = codecPlan.m_sampleRate;
                rtpmapPtr->m_channel    = codecPlan.m_channel;
                mediaParse.addAttr(rtpmapPtr);
            }

            for (const auto& ssrc : media->m_ssrcVec) {
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
    for (const auto& media : m_mediaPtrVec) {
        if (MyStatus_t::SUCCESS != media->checkValid()) {
            return MyStatus_t::FAILED;
        }

        if (MySdpTrackType_t::SDP_TRACK_TYPE_APPLICATION == media->m_type) {
            hasActiveMedia = true;
            break;
        }

        if (MySdpDirection_t::SDP_DIRECTION_SENDRECV == media->m_direction ||
            MySdpDirection_t::SDP_DIRECTION_SENDONLY == media->m_direction ||
            MySdpDirection_t::SDP_DIRECTION_RECVONLY == media->m_direction) {
            hasActiveMedia = true;
            break;
        }
    }
    return hasActiveMedia ? MyStatus_t::SUCCESS : MyStatus_t::FAILED;
}

MyStatus_t MySdpSession::getMedia(MySdpTrackType_t type, MySdpMedia& media) const
{
    for (const auto& item : m_mediaPtrVec) {
        if (type != item->m_type) {
            continue;
        }

        media = *item;
        return MyStatus_t::SUCCESS;
    }
    return MyStatus_t::FAILED;
}

MyStatus_t MySdpSession::getPortCount(unsigned int& portCount) const
{
    std::set<uint16_t> portSet;
    for (const auto& item : m_mediaPtrVec) {
        if (portSet.end() == portSet.find(item->m_port)) {
            portSet.insert(item->m_port);
        }
    }

    if (portSet.empty()) {
        return MyStatus_t::FAILED;
    }

    portCount = portSet.size();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySdpSession::setAddr(const std::string& ipAddr)
{
    if (ipAddr.empty()) {
        return MyStatus_t::FAILED;
    }

    if (m_mediaPtrVec.empty() && m_connection.empty()) {
        return MyStatus_t::FAILED;
    }

    if (!m_connection.empty()) {
        m_connection.m_address = ipAddr;
    }

    for (auto& media : m_mediaPtrVec) {
        if (!media->m_connection.empty()) {
            media->m_connection.m_address = ipAddr;
        }
    }
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySdpSession::setPort(uint16_t port, MySdpTrackType_t type)
{
    if (0 == port || MySdpTrackType_t::SDP_TRACK_TYPE_INVALID == type) {
        return MyStatus_t::FAILED;
    }

    if (m_mediaPtrVec.empty()) {
        return MyStatus_t::FAILED;
    }

    for (auto& media : m_mediaPtrVec) {
        if (type == media->m_type) {
            media->m_port = port;
            return MyStatus_t::SUCCESS;
        }
    }
    return MyStatus_t::FAILED;
}

MyStatus_t MySdpSession::setSSRC(const MySdpMedia::SSRCVec& ssrcVec, MySdpTrackType_t type)
{
    if (MySdpTrackType_t::SDP_TRACK_TYPE_INVALID == type || ssrcVec.empty()) {
        return MyStatus_t::FAILED;
    }

    if (m_mediaPtrVec.empty()) {
        return MyStatus_t::FAILED;
    }

    for (auto& media : m_mediaPtrVec) {
        if (type == media->m_type) {
            media->m_ssrcVec = ssrcVec;
            return MyStatus_t::SUCCESS;
        }
    }
    return MyStatus_t::FAILED;
}

} // namespace MY_SDP