#pragma once

namespace MY_COMMON {
/**
 * @brief 传输方向
 */
typedef enum class MySdpDirectionType
{
    SDP_DIRECTION_INVALID                   = -1,
    //                                      只发送
    SDP_DIRECTION_SENDONLY                  = 0,
    //                                      只接收
    SDP_DIRECTION_RECVONLY                  = 1,
    //                                      同时发送接收
    SDP_DIRECTION_SENDRECV                  = 2,
    //                                      禁止发送数据
    SDP_DIRECTION_INACTIVE                  = 3
} MySdpDirection_t;

/**
 * @brief 传输角色
 */
typedef enum class MySdpRoleType
{
    SDP_ROLE_INVALID                        = -1,
    //                                      客户端
    SDP_ROLE_ACTIVE                         = 0,
    //                                      服务端
    SDP_ROLE_PASSIVE                        = 1
} MySdpRole_t;

typedef enum class MySdpType
{
    SDP_TYPE_INVALID                        = -1,
    //                                      sdp offer
    SDP_TYPE_OFFER                          = 0,
    //                                      sdp answer
    SDP_TYPE_ANSWER                         = 1,
} MySdpType_t;

typedef enum MySdpTrackType
{
    SDP_TRACK_TYPE_INVALID                  = -1,
    //                                      视频
    SDP_TRACK_TYPE_VIDEO                    = 0,
    //                                      音频
    SDP_TRACK_TYPE_AUDIO                    = 1,
    //                                      字幕
    SDP_TRACK_TYPE_TITLE                    = 2,
    //                                      应用
    SDP_TRACK_TYPE_APPLICATION              = 3
} MySdpTrackType_t;

}; // MY_COMMON