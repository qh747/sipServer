#include "utils/myRandomHelper.h"

namespace MY_UTILS {

std::string MyRandomHelper::Get32BitsLenRandomStr()
{
    uuid_t uuid;
    char uuid_str[37];

    // 生成UUID
    uuid_generate_random(uuid); 

    // 将UUID转换为小写字符串
    uuid_unparse_lower(uuid, uuid_str); 

    // 提取UUID的前32位作为随机数（去掉UUID中的'-'符号）
    std::string random_32bit = uuid_str;
    random_32bit.erase(std::remove(random_32bit.begin(), random_32bit.end(), '-'), random_32bit.end());
    return random_32bit;
}

}; // namespace MY_UTILS