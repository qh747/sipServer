#include <stdexcept>
#include <cinttypes>
#include <Util/util.h>
#include "utils/mySdpHelper.h"
#include "sdp/mySdpReg.h"
#include "sdp/mySdpAttr.h"
using namespace MY_COMMON;
using namespace MY_UTILS;

namespace MY_SDP {

// ---------------------------------- MySdpTime -----------------------------------------

void MySdpTime::parse(const std::string& str)
{
    if (sscanf(str.data(), "%" SCNu64 " %" SCNu64, &m_start, &m_stop) != 2) {
        throw std::runtime_error("parse sdp time failed");
    }
}

std::string MySdpTime::toString() const
{
    if (m_value.empty()) {
        m_value = std::to_string(m_start) + " " + std::to_string(m_stop);
    }
    return MySdpItem::toString();
}

// ---------------------------------- MySdpOrigin -----------------------------------------

void MySdpOrigin::parse(const std::string& str)
{
    auto vec = toolkit::split(str, " ");
    if (6 != vec.size()) {
        throw std::runtime_error("parse sdp origin failed");
    }

    m_username       = vec[0];
    m_sessionId      = vec[1];
    m_sessionVersion = vec[2];
    m_netType        = vec[3];
    m_addrType       = vec[4];
    m_address        = vec[5];
}

std::string MySdpOrigin::toString() const
{
    if (m_value.empty()) {
        m_value = m_username + " " + m_sessionId + " " + m_sessionVersion + " " + m_netType + " " + m_addrType + " " + m_address;
    }
    return MySdpItem::toString();
}

// ---------------------------------- MySdpConnection -----------------------------------------

void MySdpConnection::parse(const std::string& str)
{
    auto vec = toolkit::split(str, " ");
    if (3 != vec.size()) {
        throw std::runtime_error("parse sdp connection failed");
    }

    m_netType  = vec[0];
    m_addrType = vec[1];
    m_address  = vec[2];
}

std::string MySdpConnection::toString() const
{
    if (m_value.empty()) {
        m_value = m_netType + " " + m_addrType + " " + m_address;
    }
    return MySdpItem::toString();
}

// ---------------------------------- MySdpMediaLine -----------------------------------------

void MySdpMediaLine::parse(const std::string& str)
{
    auto vec = toolkit::split(str, " ");
    if (4 != vec.size()) {
        throw std::runtime_error("parse sdp media line failed");
    }

    if (MyStatus_t::SUCCESS != MySdpHelper::ConvertToSdpTrackType(vec[0], m_type)) {
        throw std::runtime_error("parse sdp media line track type failed");
    }

    m_port  = std::atoi(vec[1].data());
    m_proto = vec[2];

    for (size_t i = 3; i < vec.size(); ++i) {
        m_fmts.emplace_back(vec[i]);
    }
}

std::string MySdpMediaLine::toString() const
{
    std::string trackTypeStr;
    if (MyStatus_t::SUCCESS!= MySdpHelper::ConvertToSdpTrackTypeStr(m_type, trackTypeStr)) {
        throw std::runtime_error("convert sdp media line track type to string failed");
    }

    if (m_value.empty()) {
        m_value = trackTypeStr + " " + std::to_string(m_port) + " " + m_proto;
        for (auto fmt : m_fmts) {
            m_value += ' ';
            m_value += fmt;
        }
    }
    return MySdpItem::toString();
}

// ------------------------------------ MySdpAttr -------------------------------------------

void MySdpAttr::parse(const std::string& str)
{
    auto pos     = str.find(':');
    auto key = pos == std::string::npos ? str : str.substr(0, pos);
    auto value  = pos == std::string::npos ? std::string() : str.substr(pos + 1);

    auto iter = MySdpReg::MySdpItemCreatMap.find(key);
    if (MySdpReg::MySdpItemCreatMap.end() != iter) {
        m_detail = iter->second(key, value);
    }
    else {
        m_detail = std::make_shared<MySdpCommon>(key);
        m_detail->parse(value);
    }
}

std::string MySdpAttr::toString() const
{
    if (m_value.empty()) {
        auto detailValue = m_detail->toString();
        if (detailValue.empty()) {
            m_value = m_detail->getKey();
        }
        else {
            m_value = std::string(m_detail->getKey()) + ":" + detailValue;
        }
    }
    return MySdpItem::toString();
}

// ------------------------------------ MySdpAttrSetup -------------------------------------------

void MySdpAttrSetup::parse(const std::string& str)
{
    if (MyStatus_t::SUCCESS != MySdpHelper::ConvertToSdpRole(str, m_role)) {
        throw std::runtime_error("parse sdp attr setup failed");
    }
}

std::string MySdpAttrSetup::toString() const
{
    if (m_value.empty()) {
        MySdpHelper::ConvertToSdpRoleStr(m_role, m_value);
    }
    return MySdpItem::toString();
}

// ------------------------------------ MySdpAttrRtpMap -------------------------------------------

void MySdpAttrRtpMap::parse(const std::string& str)
{
    char buf[32] = { 0 };
    if (sscanf(str.data(), "%" SCNu8 " %31[^/]/%" SCNd32 "/%" SCNd32, &m_pt, buf, &m_sampleRate, &m_channel) != 4) {
        if (sscanf(str.data(), "%" SCNu8 " %31[^/]/%" SCNd32, &m_pt, buf, &m_sampleRate) != 3) {
            throw std::runtime_error("parse sdp attr rtpmap failed");
        }
    }

    MySdpTrackType_t trackType = MySdpTrackType_t::SDP_TRACK_TYPE_INVALID;
    if (MyStatus_t::SUCCESS != MySdpHelper::ConvertToSdpTrackType(buf, trackType)) {
        // 未指定通道数时，且为音频时，那么通道数默认为1
        m_channel = 1;
    }
    else {
        m_channel = 0;
    }

    m_codec = buf;
}

std::string MySdpAttrRtpMap::toString() const
{
    if (m_value.empty()) {
        m_value = std::to_string((int)m_pt) + " " + m_codec + "/" + std::to_string(m_sampleRate);
        if (m_channel) {
            m_value += '/';
            m_value += std::to_string(m_channel);
        }
    }
    return MySdpItem::toString();
}

// ------------------------------------ MySdpAttrSSRC -------------------------------------------

void MySdpAttrSSRC::parse(const std::string& str_in)
{
    auto str = str_in + '\n';
    char attr_buf[32] = { 0 };
    char attr_val_buf[128] = { 0 };

    if (3 == sscanf(str.data(), "%" SCNu32 " %31[^:]:%127[^\n]", &m_ssrc, attr_buf, attr_val_buf)) {
        m_attribute = attr_buf;
        m_attributeValue = attr_val_buf;
    }
    else if (2 == sscanf(str.data(), "%" SCNu32 " %31s[^\n]", &m_ssrc, attr_buf)) {
        m_attribute = attr_buf;
    }
    else {
        throw std::runtime_error("parse sdp attr ssrc failed");
    }
}

std::string MySdpAttrSSRC::toString() const
{
    if (m_value.empty()) {
        m_value = std::to_string(m_ssrc) + ' ';
        m_value += m_attribute;
        if (!m_attributeValue.empty()) {
            m_value += ':';
            m_value += m_attributeValue;
        }
    }
    return MySdpItem::toString();
}

// ------------------------------------ MySdpGB28181SSRC -------------------------------------------

void MySdpGB28181SSRC::parse(const std::string& str_in)
{
    size_t equal_pos = str_in.find('=');
    if (std::string::npos == equal_pos) {
        throw std::runtime_error("parse sdp gb28181 ssrc failed");
    }

    std::string num_str = str_in.substr(equal_pos + 1);
    m_ssrc = std::stoul(num_str);
}

std::string MySdpGB28181SSRC::toString() const
{
    if (m_value.empty()) {
        m_value = std::to_string(m_ssrc);
    }
    return MySdpItem::toString();
}

}; // MY_SDP