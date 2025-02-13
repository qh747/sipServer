#pragma once
#include <string>
#include <tinyxml2.h>
#include "common/myDataDef.h"
#include "common/mySipMsgDef.h"
#include "utils/myBaseHelper.h"

namespace MY_UTILS {

/**
 * xml帮助类
 */
class MyXmlHelper : public MyNonConstructableAndNonCopyable
{
public:
    /**
     * @note                        消息格式：
     *                              <?xml version="1.0" encoding="UTF-8"?>                           
     *                              <Notify>\r\n
     *                              <CmdType>Keepalive</CmdType>\r\n
     *                              <SN>1234</SN>\r\n
     *                              <DeviceID>12345678909876543210</DeviceID>\r\n
     *                              <Status>OK</Status>\r\n
     *                              </Notify>\r\n
     */

    /** 
     * @brief                       sip keepalive消息生成
     * @return                      sip keepalive消消息体
     * @param idx                   消息索引编号
     * @param id                    本级服务id
     */
    static std::string              GenerateSipKeepAliveMsgBody(const std::string& idx, const std::string& localServId);

    /**
     * @brief                       sip keepalive消息解析
     * @return                      解析结果
     * @param xmlStr                sip keepalive消息体
     * @param type                  消息类型CmdType
     * @param id                    下级服务id DeviceID
     * @param idx                   消息索引编号SN
     * @param status                消息状态Status
     */
    static MY_COMMON::MyStatus_t    ParseSipKeepAliveMsgBody(const std::string& xmlStr, MY_COMMON::MySipKeepAliveMsgBidy_dt& keepAliveMsgBody);
};

}; // namespace MY_UTILS