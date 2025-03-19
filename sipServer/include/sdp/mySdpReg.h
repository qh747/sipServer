#pragma once
#include <map>
#include <memory>
#include <string>
#include <functional>
#include <cstdbool>
#include <Util/util.h>
#include "sdp/mySdpAttr.h"
#include "utils/myBaseHelper.h"

namespace MY_SDP {

/**
 * @brief sdp注册类
 */
class MySdpReg : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    using OnCreateSdpItem = std::function<MySdpItem::Ptr(const std::string& key, const std::string& value)>;

public:
    struct MyStrCaseCompare {
        bool operator()(const std::string &__x, const std::string &__y) const { return strcasecmp(__x.data(), __y.data()) < 0; }
    };

public:
    /**
     * @brief 注册sdp属性
     */
    template <typename Item>
    static void RegisterSdpItem() {
        OnCreateSdpItem func = [](const std::string& key, const std::string& value) {
            auto ret = std::make_shared<Item>();
            ret->parse(value);
            return ret;
        };

        Item item;
        MySdpItemCreatMap.emplace(item.getKey(), std::move(func));
    }

public:
    /**
     * @brief   注册所有sdp属性
     * @return  true: 注册成功, false: 注册失败
     */
    static bool RegisterAllIterm();

public:
    static std::map<std::string, OnCreateSdpItem, MyStrCaseCompare> MySdpItemCreatMap;
};

}; // namespace MY_SDP
