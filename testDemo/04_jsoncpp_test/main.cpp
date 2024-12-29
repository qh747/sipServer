#include <iostream>
#include <json/json.h>

int main() {
    // JSON 字符串
    std::string jsonStr = "{\"name\":\"John\", \"age\":30, \"isStudent\":false}";

    // 创建 Json::Value 对象
    Json::Value root;
    Json::Reader reader;

    // 解析 JSON 字符串
    bool parsingSuccessful = reader.parse(jsonStr, root);
    if (!parsingSuccessful) {
        std::cout << "Failed to parse JSON" << std::endl;
        return 1;
    }

    // 访问数据
    std::string name = root["name"].asString();
    int age = root["age"].asInt();
    bool isStudent = root["isStudent"].asBool();

    std::cout << "Name: " << name << std::endl;
    std::cout << "Age: " << age << std::endl;
    std::cout << "Is Student: " << (isStudent ? "Yes" : "No") << std::endl;

    return 0;
}
