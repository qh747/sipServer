#include "utils/myLogHelper.h"

namespace MY_UTILS {

google::LogSeverity MyLogHelper::LogLevelConvert(const std::string& level)
{
    if ("INFO" == level || "info" == level) {
        return google::INFO;
    } 
    else if ("WARNING" == level || "warning" == level) {
        return google::WARNING;
    } 
    else if ("ERROR" == level || "error" == level) {
        return google::ERROR;
    } 
    else if ("FATAL" == level || "fatal" == level) {
        return google::FATAL;
    } 
    else {
        return google::INFO;
    }
}

}; // namespace MY_UTILS