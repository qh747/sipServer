#include <iostream>
#include "tinyxml2.h"

int main() {
    // XML 字符串
    const char* xmlStr = R"(
        <note>
            <to>Tove</to>
            <from>Jani</from>
            <heading>Reminder</heading>
            <body>Don't forget me this weekend!</body>
        </note>
    )";

    // 创建 tinyxml2::XMLDocument 对象
    tinyxml2::XMLDocument doc;
    doc.Parse(xmlStr);

    // 检查解析是否成功
    if (doc.Error()) {
        std::cout << "XML parsing error: " << doc.ErrorID() << std::endl;
        return 1;
    }

    // 获取根元素
    tinyxml2::XMLElement* root = doc.RootElement();

    // 获取子元素
    tinyxml2::XMLElement* to = root->FirstChildElement("to");
    tinyxml2::XMLElement* from = root->FirstChildElement("from");
    tinyxml2::XMLElement* heading = root->FirstChildElement("heading");
    tinyxml2::XMLElement* body = root->FirstChildElement("body");

    // 输出内容
    std::cout << "To: " << to->GetText() << std::endl;
    std::cout << "From: " << from->GetText() << std::endl;
    std::cout << "Heading: " << heading->GetText() << std::endl;
    std::cout << "Body: " << body->GetText() << std::endl;

    return 0;
}
