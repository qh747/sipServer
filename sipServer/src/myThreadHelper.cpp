#include "utils/myThreadHelper.h"

namespace MY_UTILS {

std::string MyThreadHelper::ThreadStateToStr(ThreadState_t state)
{
    switch (state) {
        case ThreadState_t::STOPED:
            return "Stopped";
        case ThreadState_t::RUNNING:
            return "Running";
        case ThreadState_t::TERMINATED:
            return "Terminated";
        default:
            return "Unknown";
    }
}
    
}; // namespace MY_UTILS