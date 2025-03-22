#include "utils/mySdpHelper.h"
using namespace MY_COMMON;

namespace MY_UTILS {

MyStatus_t MySdpHelper::ConvertToSdpDirection(const std::string& str, MySdpDirection_t& direction)
{
    if ("sendonly" == str) {
        direction = MySdpDirection_t::SDP_DIRECTION_SENDONLY;
        return MyStatus_t::SUCCESS;
    }
    if ("recvonly" == str) {
        direction = MySdpDirection_t::SDP_DIRECTION_RECVONLY;
        return MyStatus_t::SUCCESS;
    }
    if ("sendrecv" == str) {
        direction = MySdpDirection_t::SDP_DIRECTION_SENDRECV;
        return MyStatus_t::SUCCESS;
    }
    if ("inactive" == str) {
        direction = MySdpDirection_t::SDP_DIRECTION_INACTIVE;
        return MyStatus_t::SUCCESS;
    }
    return MyStatus_t::FAILED;
}

MyStatus_t MySdpHelper::ConvertToSdpDirectionStr(MySdpDirection_t direction, std::string& str)
{
    if (MySdpDirection_t::SDP_DIRECTION_INACTIVE == direction) {
        str = "inactive";
        return MyStatus_t::SUCCESS;
    }
    if (MySdpDirection_t::SDP_DIRECTION_SENDONLY == direction) {
        str = "sendonly";
        return MyStatus_t::SUCCESS;
    }
    if (MySdpDirection_t::SDP_DIRECTION_RECVONLY == direction) {
        str = "recvonly";
        return MyStatus_t::SUCCESS;
    }
    if (MySdpDirection_t::SDP_DIRECTION_SENDRECV == direction) {
        str = "sendrecv";
        return MyStatus_t::SUCCESS;
    }
    return MyStatus_t::FAILED;
}

MyStatus_t MySdpHelper::MatchSdpDirection(MySdpDirection_t remoteDirection, MySdpDirection_t localDirection,
    MySdpDirection_t &matchDirection)
{
    switch (remoteDirection) {
        case MySdpDirection_t::SDP_DIRECTION_SENDONLY: {
            if (MySdpDirection_t::SDP_DIRECTION_RECVONLY == localDirection ||
                MySdpDirection_t::SDP_DIRECTION_SENDRECV == localDirection) {
                matchDirection = MySdpDirection_t::SDP_DIRECTION_RECVONLY;
                return MyStatus_t::SUCCESS;
            }
        }
        case MySdpDirection_t::SDP_DIRECTION_RECVONLY: {
            if (MySdpDirection_t::SDP_DIRECTION_SENDONLY ==  localDirection ||
                MySdpDirection_t::SDP_DIRECTION_SENDRECV == localDirection) {
                matchDirection = MySdpDirection_t::SDP_DIRECTION_SENDONLY;
                return MyStatus_t::SUCCESS;
            }
        }
        case MySdpDirection_t::SDP_DIRECTION_SENDRECV: {
            if (MySdpDirection_t::SDP_DIRECTION_RECVONLY == localDirection ||
                MySdpDirection_t::SDP_DIRECTION_SENDONLY == localDirection ||
                MySdpDirection_t::SDP_DIRECTION_SENDRECV == localDirection) {
                matchDirection = localDirection;
                return MyStatus_t::SUCCESS;
            }
        }
        case MySdpDirection_t::SDP_DIRECTION_INACTIVE: {
            matchDirection = MySdpDirection_t::SDP_DIRECTION_INACTIVE;
            return MyStatus_t::SUCCESS;
        }
        default: {
            return MyStatus_t::FAILED;
        }
    }
}

MyStatus_t MySdpHelper::ConvertToSdpRole(const std::string& str, MySdpRole_t& role)
{
    if ("active" == str) {
        role = MySdpRole_t::SDP_ROLE_ACTIVE;
        return MyStatus_t::SUCCESS;
    }
    if ("passive" == str) {
        role = MySdpRole_t::SDP_ROLE_PASSIVE;
        return MyStatus_t::SUCCESS;
    }
    return MyStatus_t::FAILED;
}

MyStatus_t MySdpHelper::ConvertToSdpRoleStr(MySdpRole_t role, std::string& str)
{
    if (MySdpRole_t::SDP_ROLE_ACTIVE == role) {
        str = "active";
        return MyStatus_t::SUCCESS;
    }
    if (MySdpRole_t::SDP_ROLE_PASSIVE == role) {
        str = "passive";
        return MyStatus_t::SUCCESS;
    }
    return MyStatus_t::FAILED;
}

MyStatus_t MySdpHelper::ConvertToSdpType(const std::string& str, MySdpType_t& type)
{
    if ("offer" == str) {
        type = MySdpType_t::SDP_TYPE_OFFER;
        return MyStatus_t::SUCCESS;
    }
    if ("answer" == str) {
        type = MySdpType_t::SDP_TYPE_ANSWER;
        return MyStatus_t::SUCCESS;
    }
    return MyStatus_t::FAILED;
}

MyStatus_t MySdpHelper::ConvertToSdpTypeStr(MySdpType_t type, std::string& str)
{
    if (MySdpType_t::SDP_TYPE_OFFER == type) {
        str = "offer";
        return MyStatus_t::SUCCESS;
    }
    if (MySdpType_t::SDP_TYPE_ANSWER == type) {
        str = "answer";
        return MyStatus_t::SUCCESS;
    }
    return MyStatus_t::FAILED;
}

MyStatus_t MySdpHelper::ConvertToSdpTrackType(const std::string& str, MySdpTrackType_t& type)
{
    if ("audio" == str) {
        type = MySdpTrackType_t::SDP_TRACK_TYPE_AUDIO;
        return MyStatus_t::SUCCESS;
    }
    if ("video" == str) {
        type = MySdpTrackType_t::SDP_TRACK_TYPE_VIDEO;
        return MyStatus_t::SUCCESS;
    }
    if ("title" == str) {
        type = MySdpTrackType_t::SDP_TRACK_TYPE_TITLE;
        return MyStatus_t::SUCCESS;
    }
    if ("application" == str) {
        type = MySdpTrackType_t::SDP_TRACK_TYPE_APPLICATION;
        return MyStatus_t::SUCCESS;
    }
    return MyStatus_t::FAILED;
}

MyStatus_t MySdpHelper::ConvertToSdpTrackTypeStr(MySdpTrackType_t type, std::string& str)
{
    if (MySdpTrackType_t::SDP_TRACK_TYPE_AUDIO == type) {
        str = "audio";
        return MyStatus_t::SUCCESS;
    }
    if (MySdpTrackType_t::SDP_TRACK_TYPE_VIDEO == type) {
        str = "video";
        return MyStatus_t::SUCCESS;
    }
    if (MySdpTrackType_t::SDP_TRACK_TYPE_TITLE == type) {
        str = "title";
        return MyStatus_t::SUCCESS;
    }
    if (MySdpTrackType_t::SDP_TRACK_TYPE_APPLICATION == type) {
        str = "application";
        return MyStatus_t::SUCCESS;
    }
    return MyStatus_t::FAILED;
}

} // MY_UTILS