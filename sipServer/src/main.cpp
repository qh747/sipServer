#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include "envir/mySystemEnvir.h"

int main(int argc, char *argv[]) 
{
    MY_ENVIR::MySystemEnvir::Init();

    // 临时代码，之后改用信号量等待
    while (true) {
        LOG(INFO) << "Program is running...";
        sleep(1); // 模拟长时间运行的任务
    }

    return 0;
}