#pragma once
#include <string>
#include "common/myDataDef.h"
#include "common/mySipMsgDef.h"
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

class MySipMsgHelper : public MyNonConstructableAndNonCopyable
{
public:
    /**
     * @brief                       生成sip消息URL
     * @return                      生成结果，success-0，fail-非0
     * @param id                    用户id
     * @param ipAddr                用户ip地址
     * @param port                  用户端口
     * @param type                  传输协议类型
     * @param url                   生成的sip url
     */         
    static MY_COMMON::MyStatus_t    GenerateSipMsgURL(const std::string&     id, 
                                                      const std::string&     ipAddr, 
                                                      uint16_t               port, 
                                                      MY_COMMON::MyTpProto_t type,
                                                      std::string&           url);

    /**     
     * @brief                       生成sip消息首部from字段
     * @return                      生成结果，success-0，fail-非0
     * @param id                    用户id
     * @param ipAddr                用户ip地址
     * @param fromHeader            生成的sip from header
     */         
    static MY_COMMON::MyStatus_t    GenerateSipMsgFromHeader(const std::string& id, const std::string& ipAddr, std::string& fromHeader);

    /**         
     * @brief                       生成sip消息首部to字段
     * @return                      生成结果，success-0，fail-非0
     * @param id                    用户id
     * @param ipAddr                用户ip地址
     * @param toHeader              生成的sip to header
     */         
    static MY_COMMON::MyStatus_t    GenerateSipMsgToHeader(const std::string& id, const std::string& ipAddr, std::string& toHeader);

    /**     
     * @brief                       生成sip消息首部contact字段
     * @return                      生成结果，success-0，fail-非0
     * @param id                    用户id
     * @param natIpAddr             用户nat映射后的ip地址
     * @param natPort               用户nat映射后的端口
     * @param contactHeader         生成的sip contact header
     */     
    static MY_COMMON::MyStatus_t    GenerateSipMsgContactHeader(const std::string& id, const std::string& natIpAddr, uint16_t natPort, std::string& contactHeader);

public:
    /**
     * @brief                       解析sip消息URL
     * @return                      解析结果，success-0，fail-非0
     * @param uri                   URL字符串
     * @param sipUri                解析后的sip url结构体
     */     
    static MY_COMMON::MyStatus_t    ParseSipMsgURL(const std::string& uri, MY_COMMON::MySipMsgUri_dt& sipUri);

    /**
     * @brief                       解析sip消息contact首部
     * @return                      解析结果，success-0，fail-非0
     * @param contactHeader         contact字符串
     * @param sipContactHeader      解析后的sip contact header结构体
     */     
    static MY_COMMON::MyStatus_t    ParseSipMsgContactHdr(const std::string& contactHeader, MY_COMMON::MySipMsgContactHdr_dt& sipContactHeader);

    /**
     * @brief                       解析sip消息expire首部
     * @return                      解析结果，success-0，fail-非0
     * @param expireHeader          expire字符串
     * @param sipExpireHeader       解析后的sip expire header值
     */     
    static MY_COMMON::MyStatus_t    ParseSipMsgExpireHdr(const std::string& expireHeader, double& sipExpireHeader);

    /**
     * @brief                       解析sip消息from首部
     * @return                      解析结果，success-0，fail-非0
     * @param fromHeader            from字符串
     * @param id                    解析后的服务id
     * @param ipAddr                解析后的服务ip地址
     */     
    static MY_COMMON::MyStatus_t    ParseSipMsgFromHdr(const std::string& fromHeader, std::string& id, std::string& ipAddr);

public:
    /**
     * @brief                       打印sip消息contact首部
     * @return                      打印结果，0-success, -1-failed
     * @param sipContactHeader      sip contact header结构体
     * @param contactHeader         sip contact header字符串
     */
    static MY_COMMON::MyStatus_t    PrintSipMsgContactHdr(const MY_COMMON::MySipMsgContactHdr_dt& sipContactHeader, std::string& str);

    /**
     * @brief                       打印sip消息keepAlive消息体
     * @return                      打印结果，0-success, -1-failed
     * @param keepAliveMsgBody      sip keepAlive消息体结构体
     * @param str                   sip keepAlive消息体字符串
     */
    static MY_COMMON::MyStatus_t    PrintSipKeepAliveMsgBody(const MY_COMMON::MySipKeepAliveMsgBody_dt& keepAliveMsgBody, std::string& str);

    /**
     * @brief                       打印sip消息catalog消息体
     * @return                      打印结果，0-success, -1-failed
     * @param keepAliveMsgBody      sip catalog消息体结构体
     * @param str                   sip catalog消息体字符串
     */
    static MY_COMMON::MyStatus_t    PrintSipCatalogMsgBody(const MY_COMMON::MySipCatalogReqMsgBody_dt& catalogMsgBody, std::string& str);
};
    
}; // namespace MY_UTILS