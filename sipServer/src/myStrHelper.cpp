#include <cctype>
#include <algorithm>
#include <Util/util.h>
#include "utils/myStrHelper.h"

namespace MY_UTILS {

std::string MyStrHelper::ConvertToLowStr(const std::string& strToLow)
{
    std::string result = strToLow;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return result;
}
    
std::string MyStrHelper::ConvertToUpStr(const std::string& strToUp)
{
    std::string result = strToUp;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return std::toupper(c);
    });
    return result;
}
    
std::string MyStrHelper::TrimEmptyStr(const std::string& str, const std::string& trimStr)
{
    std::string result = str;
    return toolkit::trim(result, trimStr);
}
    
}; // namespace MY_UTILS