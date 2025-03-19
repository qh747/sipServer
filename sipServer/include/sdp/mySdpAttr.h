#pragma once
#include <string>
#include <memory>
#include "common/mySdpDef.h"

namespace MY_SDP {

// ------------------------------------ MySdpItem -------------------------------------------

/**
 * @brief sdp属性基类
 */
class MySdpItem
{
public:
    using Ptr = std::shared_ptr<MySdpItem>;

public:
    virtual ~MySdpItem() = default;

public:
    virtual const char* getKey() const = 0;
    virtual void        parse(const std::string& str)   { m_value = str; }
    virtual std::string toString() const                { return m_value; }
    void                reset() const                   { m_value.clear(); }

protected:
    mutable std::string m_value;
};

// ------------------------------------ MySdpString -------------------------------------------

/**
 * @brief sdp字符串属性类
 */
template <char KEY>
class MySdpString : public MySdpItem
{
public:
    MySdpString() = default;
    MySdpString(std::string val) { m_value = std::move(val); }
    ~MySdpString() override = default;

public:
    const char* getKey() const override { static std::string key(1, KEY); return key.data(); }
};

// ------------------------------------ MySdpCommon -------------------------------------------

/**
 * @brief sdp公共属性类
 */
class MySdpCommon : public MySdpItem
{
public:
    MySdpCommon(std::string key) { this->m_key = std::move(key); }
    MySdpCommon(std::string key, std::string val) { this->m_key = std::move(key); this->m_value = std::move(val); }
    ~MySdpCommon() override = default;

public:
    const char *getKey() const override { return m_key.data(); }

public:
    std::string m_key;
};

// ------------------------------------ MySdpTime -------------------------------------------

/**
 * @brief   sdp time
 * @format  t=<start-time> <stop-time>
 * @example t=0 0
 */
class MySdpTime : public MySdpItem
{
public:
    ~MySdpTime() override = default;

public:
    void        parse(const std::string& str) override;
    std::string toString() const override;
    const char* getKey() const override { return "t"; }

public:
    uint64_t m_start { 0 };
    uint64_t m_stop { 0 };
};

// ------------------------------------ MySdpOrigin -------------------------------------------
/**
 * @brief   sdp origin
 * @format  o=<username> <sess-id> <sess-version> <nettype> <addrtype> <unicast-address>
 * @example o=jdoe 2890844526 2890842807 IN IP4 10.47.16.5
 */
class MySdpOrigin : public MySdpItem
{
public:
    ~MySdpOrigin() override = default;

public:
    void        parse(const std::string& str) override;
    std::string toString() const override;
    const char* getKey() const override { return "o"; }

public:
    bool empty() const {
        return m_username.empty() || m_sessionId.empty() || m_sessionVersion.empty() ||
            m_netType.empty() || m_addrType.empty() || m_address.empty();
    }

public:
    std::string m_username          { "-" };
    std::string m_sessionId         { "0" };
    std::string m_sessionVersion    { "0" };
    std::string m_netType           { "IN" };
    std::string m_addrType          { "IP4" };
    std::string m_address           { "0.0.0.0" };
};

// ------------------------------------ MySdpConnection -------------------------------------------
/**
 * @brief   sdp connection
 * @format  c=<nettype> <addrtype> <connection-address>
 * @example c=IN IP4 224.2.17.12/127
 */
class MySdpConnection : public MySdpItem
{
public:
    ~MySdpConnection() override = default;

public:
    void        parse(const std::string& str) override;
    std::string toString() const override;
    const char* getKey() const override { return "c"; }

public:
    bool empty() const { return m_address.empty(); }

public:
    std::string m_netType  { "IN" };
    std::string m_addrType { "IP4" };
    std::string m_address  { "0.0.0.0" };
};

// ------------------------------------ MySdpMediaLine -------------------------------------------
/**
 * @brief   sdp Media Line
 * @format  m=<media> <port> <proto> <fmt> ...
 * @example m=video 5268 RTP/AVP 96
 * @type    udp - RTP/AVP, tcp - TCP/RTP/AVP
 */
class MySdpMediaLine : public MySdpItem
{
public:
    ~MySdpMediaLine() override = default;

public:
    void        parse(const std::string& str) override;
    std::string toString() const override;
    const char* getKey() const override { return "m"; }

public:
    MY_COMMON::MySdpTrackType_t m_type;
    uint16_t                    m_port;
    std::string                 m_proto;
    std::vector<std::string>    m_fmts;
};

// ------------------------------------ MySdpAttr -------------------------------------------
/**
 * @brief   sdp attr
 * @format  a=<attribute>:<value> | a=<attribute>
 * @example a=recvonly
 */
class MySdpAttr : public MySdpItem
{
public:
    using Ptr = std::shared_ptr<MySdpAttr>;

public:
    ~MySdpAttr() override = default;

public:
    void        parse(const std::string& str) override;
    std::string toString() const override;
    const char* getKey() const override { return "a"; }

public:
    MySdpItem::Ptr m_detail;
};

// ------------------------------------ MySdpAttrSetup -------------------------------------------
/**
 * @brief   sdp attr setup
 * @example a=setup:active
 */
class MySdpAttrSetup : public MySdpItem
{
public:
    MySdpAttrSetup() = default;
    ~MySdpAttrSetup() override = default;
    MySdpAttrSetup(MY_COMMON::MySdpRole_t role) { m_role = role; }

public:
    void        parse(const std::string &str) override;
    std::string toString() const override;
    const char* getKey() const override { return "setup"; }

public:
    MY_COMMON::MySdpRole_t m_role { MY_COMMON::MySdpRole_t::SDP_ROLE_INVALID };
};

// ------------------------------------ MySdpAttrRtpMap -------------------------------------------
/**
 * @brief   sdp attr rtpmap
 * @format  a=rtpmap:<payload type> <encoding name>/<clock rate>[/<encoding parameters>]
 * @example a=rtpmap:111 opus/48000/2
 */
class MySdpAttrRtpMap : public MySdpItem
{
public:
    ~MySdpAttrRtpMap() override = default;

public:
    void        parse(const std::string& str) override;
    std::string toString() const override;
    const char* getKey() const override { return "rtpmap"; }

public:
    uint8_t     m_pt;
    std::string m_codec;
    uint32_t    m_sampleRate;
    uint32_t    m_channel;
};

// ------------------------------------ MySdpAttrSSRC -------------------------------------------
/**
 * @brief   sdp attr ssrc
 * @format  a=ssrc:<ssrc-id> <attribute>:<value> | a=ssrc:<ssrc-id> <attribute>
 * @example a=ssrc:3245185839 cname:Cx4i/VTR51etgjT7
 */
class MySdpAttrSSRC : public MySdpItem
{
public:
    ~MySdpAttrSSRC() override = default;

public:
    void        parse(const std::string& str) override;
    std::string toString() const override;
    const char* getKey() const override { return "ssrc"; }

public:
    uint32_t    m_ssrc;
    std::string m_attribute;
    std::string m_attributeValue;
};

// ------------------------------------ MySdpGB28181SSRC -------------------------------------------
/**
 * @brief   sdp GB28181 ssrc
 * @format  y=ssrc
 * @example y=3245185839
 */
class MySdpGB28181SSRC : public MySdpItem
{
public:
    ~MySdpGB28181SSRC() override = default;

public:
    void        parse(const std::string& str) override;
    std::string toString() const override;
    const char* getKey() const override { return "y"; }

public:
    uint32_t    m_ssrc;
};

// ------------------------------------ MySdpDirect -------------------------------------------

class MyDirectInterface
{
public:
    virtual ~MyDirectInterface() = default;
    virtual MY_COMMON::MySdpDirection_t getDirect() const = 0;

protected:
    static const char* GetDirectString(MY_COMMON::MySdpDirection_t val) {
        switch (val) {
            case MY_COMMON::MySdpDirection_t::SDP_DIRECTION_SENDONLY: return "sendonly";
            case MY_COMMON::MySdpDirection_t::SDP_DIRECTION_RECVONLY: return "recvonly";
            case MY_COMMON::MySdpDirection_t::SDP_DIRECTION_SENDRECV: return "sendrecv";
            case MY_COMMON::MySdpDirection_t::SDP_DIRECTION_INACTIVE: return "inactive";
            default: return "invalid";
        }
    }
};

class MyDirectInterfaceImp : public MySdpItem, public MyDirectInterface
{
public:
    MyDirectInterfaceImp(MY_COMMON::MySdpDirection_t direct) { m_direct = direct; }
    ~MyDirectInterfaceImp() override = default;

public:
    const char* getKey() const override { return GetDirectString(getDirect()); }
    MY_COMMON::MySdpDirection_t getDirect() const override { return m_direct; }

private:
    MY_COMMON::MySdpDirection_t m_direct;
};

class MySdpDirectSendonly : public MySdpItem, public MyDirectInterface
{
public:
    ~MySdpDirectSendonly() override = default;
    const char* getKey() const override { return GetDirectString(getDirect()); }
    MY_COMMON::MySdpDirection_t getDirect() const override { return MY_COMMON::MySdpDirection_t::SDP_DIRECTION_SENDONLY; }
};

class MySdpDirectRecvonly : public MySdpItem, public MyDirectInterface
{
public:
    ~MySdpDirectRecvonly() override = default;
    const char* getKey() const override { return GetDirectString(getDirect()); }
    MY_COMMON::MySdpDirection_t getDirect() const override { return MY_COMMON::MySdpDirection_t::SDP_DIRECTION_RECVONLY; }
};

class MySdpDirectSendrecv : public MySdpItem, public MyDirectInterface
{
public:
    ~MySdpDirectSendrecv() override = default;
    const char* getKey() const override { return GetDirectString(getDirect()); }
    MY_COMMON::MySdpDirection_t getDirect() const override { return MY_COMMON::MySdpDirection_t::SDP_DIRECTION_SENDRECV; }
};

class MySdpDirectInactive : public MySdpItem, public MyDirectInterface
{
public:
    ~MySdpDirectInactive() override = default;
    const char* getKey() const override { return GetDirectString(getDirect()); }
    MY_COMMON::MySdpDirection_t getDirect() const override { return MY_COMMON::MySdpDirection_t::SDP_DIRECTION_INACTIVE; }
};

}; // MY_SDP

