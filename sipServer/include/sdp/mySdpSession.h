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
    using Ptr = std::shared_ptr<MySdpSession>;

public:
    /**
     * @brief                       创建sdp answer
     * @return                      有效值
     * @param remoteSdp             对端sdp
     * @param localSdp              本端sdp
     * @param answerSdp             应答sdp
     */
    static MY_COMMON::MyStatus_t    CreateAnswer(const MySdpSession& remoteSdp, const MySdpSession& localSdp,
                                        MySdpSession& answerSdp);

public:
    /**
     * @brief                       从sdp文本加载
     * @return                      有效值
     * @param sdp                   sdp文本
     */
    MY_COMMON::MyStatus_t           loadFrom(const std::string& sdp);

    /**
     * @brief                       从sdp文件加载
     * @return                      有效值
     * @param fileName              sdp文件名称
     */
    MY_COMMON::MyStatus_t           loadFromFile(const std::string& fileName);

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

public:
    /**
     * @brief                       获取媒体信息
     * @return                      有效值
     * @param type                  媒体类型
     * @param media                 媒体信息
     */
    MY_COMMON::MyStatus_t           getMedia(MY_COMMON::MySdpTrackType_t type, MySdpMedia& media) const;

    /**
     * @brief                       获取端口数量
     * @return                      有效值
     * @param portCount             端口数量
     */
    MY_COMMON::MyStatus_t           getPortCount(unsigned int& portCount) const;

    /**
     * @brief                       设置地址
     * @return                      有效值
     * @param ipAddr                ip地址
     */
    MY_COMMON::MyStatus_t           setAddr(const std::string& ipAddr);

    /**
     * @brief                       设置端口
     * @return                      有效值
     * @param port                  端口
     * @param type                  媒体类型
     */
    MY_COMMON::MyStatus_t           setPort(uint16_t port, MY_COMMON::MySdpTrackType_t type);

    /**
     * @brief                       设置ssrc
     * @return                      有效值
     * @param ssrcVec               ssrc数组
     * @param type                  媒体类型
     */
    MY_COMMON::MyStatus_t           setSSRC(const MySdpMedia::SSRCVec& ssrcVec, MY_COMMON::MySdpTrackType_t type);

public:
    uint32_t                        m_version;
    MySdpOrigin                     m_origin;
    std::string                     m_sessionName;
    MySdpTime                       m_time;
    MySdpConnection                 m_connection;
    MySdpMedia::PtrVec              m_mediaPtrVec;
};

}; // namespace MY_SDP
