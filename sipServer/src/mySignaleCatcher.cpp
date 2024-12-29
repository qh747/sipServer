#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include "envir/mySignaleCatcher.h"

namespace MY_ENVIR
{
    MyStatus_t MySignalCatcher::Init()
    {
        // 捕获ctrl + c信号
        signal(SIGINT, MySignalCatcher::SignalHandler);
        return MyStatus_t::SUCCESS;
    }

    void MySignalCatcher::SignalHandler(int signum)
    {
        switch (signum) {
            case SIGINT: {
                LOG(INFO) << "Interrupt signal (" << signum << ") received";
                exit(signum);
                break;
            } 
            default:
                LOG(WARNING) << "Other signal (" << signum << ") received";
                break;
        };
    }
    
}; // namespace MY_ENVIR