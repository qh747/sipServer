#pragma once
#include <memory>
#include <string>
#include <vector>
#include "sdp/mySdpAttr.h"
#include "common/mySdpDef.h"

namespace MY_SDP {

// ------------------------------------ MySdpParse -------------------------------------------

/**
 * @brief sdp解析基类
 */
class MySdpParse
{
public:
    virtual ~MySdpParse() = default;

public:
    /**
     * @brief                       转换为字符串格式sdp
     * @return                      字符串格式sdp
     */
    virtual std::string             toString() const;

public:
    template <typename cls>
    cls getItemClass(char key, const char* attrKey = nullptr) const {
        auto item = std::dynamic_pointer_cast<cls>(getItem(key, attrKey));
        if (nullptr == item) {
            return cls();
        }
        return *item;
    }

    template <typename cls>
    std::vector<cls> getAllItem(char key, const char* attrKey = nullptr) const {
        std::vector<cls> ret;
        std::string strKey(1, key);
        for (auto item : m_itemVec) {
            if (0 == strcasecmp(item->getKey(), strKey.data())) {
                if (nullptr == attrKey) {
                    auto c = std::dynamic_pointer_cast<cls>(item);
                    if (nullptr != c) {
                        ret.emplace_back(*c);
                    }
                }
                else {
                    auto attr = std::dynamic_pointer_cast<MySdpAttr>(item);
                    if (nullptr != attr && 0 == strcasecmp(attr->m_detail->getKey(), attrKey)) {
                        auto c = std::dynamic_pointer_cast<cls>(attr->m_detail);
                        if (nullptr != c) {
                            ret.emplace_back(*c);
                        }
                    }
                }
            }
        }
        return ret;
    }

public:
    /**
     * @brief                       添加sdp对象
     * @param item                  sdp对象
     */
    void                            addItem(MySdpItem::Ptr item);

    /**
     * @brief                       添加sdp属性
     * @param attr                  sdp属性
     */
    void                            addAttr(MySdpItem::Ptr attr);

    /**
     * @brief                       获取sdp传输方向
     * @return                      sdp传输方向
     */
    MY_COMMON::MySdpDirection_t     getDirection() const;

    /**
     * @brief                       获取sdp对象
     * @return                      sdp对象
     * @param key                   sdp对象类型key
     * @param attrKey               sdp对象属性key
     */
    MySdpItem::Ptr                  getItem(char key, const char* attrKey = nullptr) const;

    /**
     * @brief                       获取字符串格式sdp对象
     * @return                      字符串格式sdp对象
     * @param key                   sdp对象类型key
     * @param attrKey               sdp对象属性key
     */
    std::string                     getStringItem(char key, const char* attrKey = nullptr) const;

private:
    std::vector<MySdpItem::Ptr>     m_itemVec;
};

// ------------------------------------ MySdpSessionParse -------------------------------------------

/**
 * @brief sdp会话解析类
 */
class MySdpSessionParse : public MySdpParse
{
public:
    using Ptr = std::shared_ptr<MySdpSessionParse>;

public:
    ~MySdpSessionParse() override = default;

public:
    /**
     * @brief                       转换为字符串格式sdp
     * @return                      字符串格式sdp
     */
    std::string                     toString() const override;

public:
    /**
     * @brief                       解析sdp
     * @param str                   sdp字符串
     */
    void                            parse(const std::string& str);

public:
    /**
     * @brief                       获取sdp版本
     * @return                      sdp版本
     */
    int                             getVersion() const;

    /**
     * @brief                       获取sdp发起方信息
     * @return                      sdp发起方信息
     */
    MySdpOrigin                     getOrigin() const;

    /**
     * @brief                       获取sdp会话名称
     * @return                      sdp会话名称
     */
    std::string                     getSessionName() const;

    /**
     * @brief                       获取sdp时间
     * @return                      sdp时间
     */
    MySdpTime                       getSessionTime() const;

    /**
     * @brief                       获取sdp连接信息
     * @return                      sdp连接信息
     */
    MySdpConnection                 getConnection() const;

public:
    std::vector<MySdpParse>         m_sdpMediaVec;
};

}; // namespace MY_SDP