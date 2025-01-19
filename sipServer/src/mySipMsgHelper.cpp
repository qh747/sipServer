#include "utils/mySipMsgHelper.h"
using MY_COMMON::MyTpProto_t;

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
    
}; // namespace MY_UTILS