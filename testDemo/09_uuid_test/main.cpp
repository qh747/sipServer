#include <string>
#include <iostream>
#include <algorithm>
#include <uuid/uuid.h>

void generateRandomStr()
{
    uuid_t uuid; // UUID变量
    char uuid_str[37]; // 存储UUID字符串

    // 生成UUID
    uuid_generate_random(uuid); // 基于随机数生成UUID
    uuid_unparse_lower(uuid, uuid_str); // 将UUID转换为小写字符串

    std::cout << "Generated UUID: " << uuid_str << std::endl;

    // 提取UUID的前32位作为随机数（去掉UUID中的'-'符号）
    std::string random_32bit = uuid_str;
    random_32bit.erase(std::remove(random_32bit.begin(), random_32bit.end(), '-'), random_32bit.end());

    std::cout << "32位随机数: " << random_32bit << std::endl;
}

int main() 
{
    generateRandomStr();
    generateRandomStr();

    return 0;
}