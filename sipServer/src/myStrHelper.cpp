#include <cctype>
#include <algorithm>
#include <Util/util.h>
#include "utils/myStrHelper.h"
using namespace MY_COMMON;

namespace MY_UTILS {

MyStatus_t MyStrHelper::ConvertToLowStr(const std::string& strToLow, std::string& lowStr)
{
    lowStr = strToLow;
    std::transform(lowStr.begin(), lowStr.end(), lowStr.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    return MyStatus_t::SUCCESS;
}
    
MyStatus_t MyStrHelper::ConvertToUpStr(const std::string& strToUp, std::string& upStr)
{
    upStr = strToUp;
    std::transform(upStr.begin(), upStr.end(), upStr.begin(), [](unsigned char c) {
        return std::toupper(c);
    });
    return MyStatus_t::SUCCESS;
}
    
MyStatus_t MyStrHelper::TrimEmptyStr(const std::string& str, std::string& resStr, const std::string& trimStr)
{
    std::string result = str;
    resStr = toolkit::trim(result, trimStr);
    return MyStatus_t::SUCCESS;
}
    
}; // namespace MY_UTILS