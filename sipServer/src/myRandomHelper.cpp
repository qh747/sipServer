#include <string>
#include <ctime>
#include <climits>
#include <algorithm>
#include <uuid/uuid.h>
#include "utils/myRandomHelper.h"
using namespace MY_COMMON;

namespace MY_UTILS {

MyStatus_t MyRandomHelper::Get32BytesLenRandomStr(std::string& str)
{
    uuid_t uuid;
    char uuid_str[37];

    // 生成UUID
    uuid_generate_random(uuid); 

    // 将UUID转换为小写字符串
    uuid_unparse_lower(uuid, uuid_str); 

    // 提取UUID的前32位作为随机数（去掉UUID中的'-'符号）
    str = uuid_str;
    str.erase(std::remove(str.begin(), str.end(), '-'), str.end());
    return MyStatus_t::SUCCESS;
}

MyStatus_t MyRandomHelper::Get4BytesLenRandomStr(std::string& str)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // 随机数范围
    int lower_bound = 1000; 
    int upper_bound = INT_MAX; 

    // 生成 [lower_bound, upper_bound] 范围内的随机数
    int random_number = lower_bound + std::rand() % (upper_bound - lower_bound + 1);
    str = std::to_string(random_number);
    return MyStatus_t::SUCCESS;
}

}; // namespace MY_UTILS