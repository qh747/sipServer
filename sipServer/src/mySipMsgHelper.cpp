#include <cstdio>
#include <cstdint>
#include <cstring>
#include "utils/mySipMsgHelper.h"
using MY_COMMON::MyStatus_t;
using MY_COMMON::MyTpProto_t;
using MY_COMMON::MySipMsgUri_dt;
using MY_COMMON::MySipMsgContactHdr_dt;

namespace MY_UTILS {

std::string MySipMsgHelper::GenerateSipMsgURL(const std::string& id, const std::string& ipAddr, uint16_t port, MyTpProto_t type)
{
    std::stringstream ss;
    if (MyTpProto_t::TCP == type) {
        ss << "sip:" << id << "@" << ipAddr << ":" << port << ";transport=tcp" << std::endl;
    }
    else {
        ss << "sip:" << id << "@" << ipAddr << ":" << port << ";transport=udp" << std::endl;
    }
    return ss.str();
}
    
std::string MySipMsgHelper::GenerateSipMsgFromHeader(const std::string& id, const std::string& ipAddr)
{
    std::stringstream ss;
    ss << "<sip:" << id << "@" << ipAddr << ">" << std::endl;
    return ss.str();
}
    
std::string MySipMsgHelper::GenerateSipMsgToHeader(const std::string& id, const std::string& ipAddr)
{
    std::stringstream ss;
    ss << "<sip:" << id << "@" << ipAddr << ">" << std::endl;
    return ss.str();
}

std::string MySipMsgHelper::GenerateSipMsgContactHeader(const std::string& id, const std::string& natIpAddr, uint16_t natPort)
{
    std::stringstream ss;
    ss << "<sip:" << id << "@" << natIpAddr << ":" << natPort << ">" << std::endl;
    return ss.str();
}

MyStatus_t MySipMsgHelper::ParseSipMsgURL(const std::string& uri, MySipMsgUri_dt& sipUri)
{
    char     id[128];
    char     ipAddr[128];
    uint16_t port;
    char     transport[16];

    if (sscanf(uri.c_str(), "sip:%[^@]@%[^:]:%hu;transport=%s", id, ipAddr, &port, transport) == 4) {
        sipUri.id     = id;
        sipUri.ipAddr = ipAddr;
        sipUri.port   = port;
        sipUri.proto  = ((0 == strncmp("udp", transport, 3)) ? MyTpProto_t::UDP : MyTpProto_t::TCP);
        return MyStatus_t::SUCCESS;
    } 
    else {
        return MyStatus_t::FAILED;
    }
}

MyStatus_t MySipMsgHelper::ParseSipMsgContactHdr(const std::string& contactHeader, MySipMsgContactHdr_dt& sipContactHeader)
{
    char     id[128];
    char     ipAddr[128];
    uint16_t port;

    if (sscanf(contactHeader.c_str(), "Contact: <sip:%[^@]@%[^:]:%hu>", id, ipAddr, &port) == 3) {
        sipContactHeader.id     = id;
        sipContactHeader.ipAddr = ipAddr;
        sipContactHeader.port   = port;
        return MyStatus_t::SUCCESS;
    } 
    else {
        return MyStatus_t::FAILED;
    }
}

MyStatus_t MySipMsgHelper::ParseSipMsgExpireHdr(const std::string& expireHeader, double& sipExpireHeader)
{
    double expires = 0.0;
    if (sscanf(expireHeader.c_str(), "Expires: %lf", &expires) == 1) {
        sipExpireHeader = expires;
        return MyStatus_t::SUCCESS;
    } else {
        return MyStatus_t::FAILED;
    }
}
    
}; // namespace MY_UTILS