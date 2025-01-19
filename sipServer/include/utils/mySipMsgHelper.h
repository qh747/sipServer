#pragma once
#include <string>
#include <sstream>
#include "common/myDataDef.h"
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

class MySipMsgHelper : public MyNonConstructableAndNonCopyable
{
public:
    /**
     * @brief                   生成sip消息URL
     * @param   id              用户id
     * @param   ipAddr          用户ip地址
     * @param   port            用户端口
     * @param   type            传输协议类型
     */     
    static std::string          GenerateSipMsgURL(const std::string& id, const std::string& ipAddr, uint16_t port, MY_COMMON::MyTpProto_t type);

    /**     
     * @brief                   生成sip消息首部from字段
     * @param   id              用户id
     * @param   ipAddr          用户ip地址
     */     
    static std::string          GenerateSipMsgFromHeader(const std::string& id, const std::string& ipAddr);

    /**     
     * @brief                   生成sip消息首部to字段
     * @param   id              用户id
     * @param   ipAddr          用户ip地址
     */     
    static std::string          GenerateSipMsgToHeader(const std::string& id, const std::string& ipAddr);

    /** 
     * @brief                   生成sip消息首部contact字段
     * @param   id              用户id
     * @param   natIpAddr       用户nat映射后的ip地址
     * @param   natPort         用户nat映射后的端口
     */ 
    static std::string          GenerateSipMsgContactHeader(const std::string& id, const std::string& natIpAddr, uint16_t natPort);
};
    
}; // namespace MY_UTILS