#include <string>
#include <ctime>
#include <climits>
#include <cstdlib>
#include <algorithm>
#include "utils/myRandomHelper.h"

namespace MY_UTILS {

std::string MyRandomHelper::Get32BytesLenRandomStr()
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

std::string MyRandomHelper::Get4BytesLenRandomStr()
{
    std::srand(static_cast<unsigned int>(std::time(0)));

    // 随机数范围
    int lower_bound = 1000; 
    int upper_bound = INT_MAX; 

    // 生成 [lower_bound, upper_bound] 范围内的随机数
    int random_number = lower_bound + std::rand() % (upper_bound - lower_bound + 1);
    return std::to_string(random_number);
}

}; // namespace MY_UTILS