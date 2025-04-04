#include <iostream>
#include <sstream>
#include <regex>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include "utils/mySipMsgHelper.h"
using namespace MY_COMMON;

namespace MY_UTILS {

MyStatus_t MySipMsgHelper::GenerateSipMsgURL(const std::string& id, const std::string& ipAddr, uint16_t port, MyTpProto_t type, std::string& url)
{
    std::stringstream ss;
    if (MyTpProto_t::TCP == type) {
        ss << "sip:" << id << "@" << ipAddr << ":" << port << ";transport=tcp";
    }
    else {
        ss << "sip:" << id << "@" << ipAddr << ":" << port << ";transport=udp";
    }

    url = ss.str();
    return MyStatus_t::SUCCESS;
}
    
MyStatus_t MySipMsgHelper::GenerateSipMsgFromHeader(const std::string& id, const std::string& ipAddr, std::string& fromHeader)
{
    std::stringstream ss;
    ss << "<sip:" << id << "@" << ipAddr << ">";

    fromHeader = ss.str();
    return MyStatus_t::SUCCESS;
}
    
MyStatus_t MySipMsgHelper::GenerateSipMsgToHeader(const std::string& id, const std::string& ipAddr, std::string& toHeader)
{
    std::stringstream ss;
    ss << "<sip:" << id << "@" << ipAddr << ">";

    toHeader = ss.str();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipMsgHelper::GenerateSipMsgContactHeader(const std::string& id, const std::string& natIpAddr, uint16_t natPort, std::string& contactHeader)
{
    std::stringstream ss;
    ss << "<sip:" << id << "@" << natIpAddr << ":" << natPort << ">";

    contactHeader = ss.str();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipMsgHelper::ParseSipMsgURL(const std::string& uri, MySipMsgUri_dt& sipUri)
{
    char     id[128];
    char     ipAddr[128];
    uint16_t port;
    char     transport[16];

    if (4 == sscanf(uri.c_str(), "sip:%[^@]@%[^:]:%hu;transport=%s", id, ipAddr, &port, transport)) {
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

    if (3 == sscanf(contactHeader.c_str(), "Contact: <sip:%[^@]@%[^:]:%hu>", id, ipAddr, &port)) {
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
    if (1 == sscanf(expireHeader.c_str(), "Expires: %lf", &expires)) {
        sipExpireHeader = expires;
        return MyStatus_t::SUCCESS;
    } else {
        return MyStatus_t::FAILED;
    }
}

MyStatus_t MySipMsgHelper::ParseSipMsgFromHdr(const std::string& fromHeader, std::string& id, std::string& ipAddr)
{
    // 查找 "sip:" 的起始位置
    std::size_t sipStartPos = fromHeader.find("sip:");
    if (std::string::npos == sipStartPos) {
        return MyStatus_t::FAILED; 
    }
    
    // 调整起始位置到 "sip:" 之后
    std::size_t idStartPos = sipStartPos + 4;
    
    // 查找 '@' 符号
    std::size_t atStartPos = fromHeader.find('@', idStartPos);
    if (std::string::npos == atStartPos) {
        return MyStatus_t::FAILED; 
    }
    
    // 查找域名的结束位置（可能是 '>' 或字符串结尾）
    std::size_t domainEndPos = fromHeader.find('>', atStartPos + 1);
    if (std::string::npos == domainEndPos) {
        domainEndPos = fromHeader.length();
    }

    id     = fromHeader.substr(idStartPos, atStartPos - idStartPos);
    ipAddr = fromHeader.substr(atStartPos + 1, domainEndPos - (atStartPos + 1));
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipMsgHelper::ParseSipMsgToHdr(const std::string& toHeader, std::string& id, std::string& ipAddr)
{
    // 查找 "sip:" 的起始位置
    std::size_t sipStartPos = toHeader.find("sip:");
    if (std::string::npos == sipStartPos) {
        return MyStatus_t::FAILED; 
    }
    
    // 调整起始位置到 "sip:" 之后
    std::size_t idStartPos = sipStartPos + 4;
    
    // 查找 '@' 符号
    std::size_t atStartPos = toHeader.find('@', idStartPos);
    if (std::string::npos == atStartPos) {
        return MyStatus_t::FAILED; 
    }
    
    // 查找域名的结束位置（可能是 '>' 或字符串结尾）
    std::size_t domainEndPos = toHeader.find('>', atStartPos + 1);
    if (std::string::npos == domainEndPos) {
        domainEndPos = toHeader.length();
    }

    id     = toHeader.substr(idStartPos, atStartPos - idStartPos);
    ipAddr = toHeader.substr(atStartPos + 1, domainEndPos - (atStartPos + 1));
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipMsgHelper::PrintSipMsgContactHdr(const MySipMsgContactHdr_dt& sipContactHeader, std::string& str)
{
    std::stringstream ss;
    ss << "id: " << sipContactHeader.id << " ip addr: " << sipContactHeader.ipAddr << " port: " << sipContactHeader.port;

    str = ss.str();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipMsgHelper::PrintSipKeepAliveMsgBody(const MySipKeepAliveMsgBody_dt& keepAliveMsgBody, std::string& str)
{
    std::stringstream ss;
    ss << "cmdType: " << keepAliveMsgBody.cmdType << " DeviceID: " << keepAliveMsgBody.deviceId 
       << " SN: " << keepAliveMsgBody.sn << " Status: " << keepAliveMsgBody.status;

    str = ss.str();
    return MyStatus_t::SUCCESS;
}

MyStatus_t MySipMsgHelper::PrintSipCatalogMsgBody(const MySipCatalogReqMsgBody_dt& catalogMsgBody, std::string& str)
{
    std::stringstream ss;
    ss << "cmdType: " << catalogMsgBody.cmdType << " DeviceID: " << catalogMsgBody.deviceId  << " SN: " << catalogMsgBody.sn;

    str = ss.str();
    return MyStatus_t::SUCCESS;
}
    
} // namespace MY_UTILS