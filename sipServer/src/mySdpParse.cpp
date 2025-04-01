#include <set>
#include <cstdlib>
#include <sstream>
#include <Util/util.h>
#include "sdp/mySdpReg.h"
#include "sdp/mySdpParse.h"
using namespace MY_COMMON;

namespace MY_SDP {

// ------------------------------------ MySdpParse -------------------------------------------

std::string MySdpParse::toString() const
{
    std::stringstream ss;
    for (auto& item : m_itemVec) {
        ss << item->getKey() << "=" << item->toString() << "\r\n";
    }

    return ss.str();
}

void MySdpParse::addItem(MySdpItem::Ptr item)
{
    m_itemVec.push_back(std::move(item));
}

void MySdpParse::addAttr(MySdpItem::Ptr attr)
{
    auto itemPtr = std::make_shared<MySdpAttr>();
    itemPtr->m_detail = std::move(attr);
    m_itemVec.push_back(std::move(itemPtr));
}

MySdpDirection_t MySdpParse::getDirection() const
{
    for (auto& item : m_itemVec) {
        auto attr = std::dynamic_pointer_cast<MySdpAttr>(item);
        if (nullptr == attr) {
            continue;
        }

        auto dir = std::dynamic_pointer_cast<MyDirectInterface>(attr->m_detail);
        if (nullptr == dir) {
            continue;
        }
        return dir->getDirect();
    }
    return MySdpDirection_t::SDP_DIRECTION_INVALID;
}

MySdpItem::Ptr MySdpParse::getItem(char key, const char* attrKey) const
{
    std::string strKey(1, key);
    for (auto item : m_itemVec) {
        if (0 != strcasecmp(item->getKey(), strKey.data())) {
            continue;
        }

        if (nullptr == attrKey) {
            return item;
        }

        auto attr = std::dynamic_pointer_cast<MySdpAttr>(item);
        if (nullptr != attr && 0 == strcasecmp(attr->m_detail->getKey(), attrKey)) {
            return attr->m_detail;
        }
    }
    return nullptr;
}

std::string MySdpParse::getStringItem(char key, const char* attrKey) const
{
    auto item = this->getItem(key, attrKey);
    if (nullptr == item) {
        return "";
    }
    return item->toString();
}

// ------------------------------------ MySdpSessionParse -------------------------------------------

std::string MySdpSessionParse::toString() const
{
    toolkit::_StrPrinter printer;
    printer << MySdpParse::toString();
    for (auto& media : m_sdpMediaVec) {
        printer << media.toString();
    }
    return std::move(printer.data());
}

MyStatus_t MySdpSessionParse::parse(const std::string& str)
{
    static auto flag = MySdpReg::RegisterAllIterm();
    if (!flag) {
        return MyStatus_t::FAILED;
    }

    MySdpParse* media = nullptr;
    auto lines = toolkit::split(str, "\n");
    std::set<std::string> line_set;
    for (auto& line : lines) {
        toolkit::trim(line);
        if (line.size() < 3 || line[1] != '=') {
            continue;
        }

        if (!line_set.emplace(line).second) {
            continue;
        }

        auto key = line.substr(0, 1);
        auto value = line.substr(2);
        if (!strcasecmp(key.data(), "m")) {
            m_sdpMediaVec.emplace_back(MySdpParse());
            media = &m_sdpMediaVec.back();
            line_set.clear();
        }

        MySdpItem::Ptr item;
        auto it = MySdpReg::MySdpItemCreatMap.find(key);
        if (it != MySdpReg::MySdpItemCreatMap.end()) {
            item = it->second(key, value);
        }
        else {
            item = std::make_shared<MySdpCommon>(key);
            item->parse(value);
        }

        if (media) {
            media->addItem(std::move(item));
        }
        else {
            addItem(std::move(item));
        }
    }
    return MyStatus_t::SUCCESS;
}

int MySdpSessionParse::getVersion() const
{
    return std::atoi(this->getStringItem('v').data());
}

MySdpOrigin MySdpSessionParse::getOrigin() const
{
    return this->getItemClass<MySdpOrigin>('o');
}

std::string MySdpSessionParse::getSessionName() const
{
    return this->getStringItem('s');
}

MySdpTime MySdpSessionParse::getSessionTime() const
{
    return this->getItemClass<MySdpTime>('t');
}

MySdpConnection MySdpSessionParse::getConnection() const
{
    return this->getItemClass<MySdpConnection>('c');
}

} // namespace MY_SDP
