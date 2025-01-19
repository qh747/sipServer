#include "common/myDataDef.h"
#include "envir/mySystemEnvir.h"

int main(int argc, char* argv[]) 
{
    // 初始化sipServer环境
    if (MY_COMMON::MyStatus_t::SUCCESS != MY_ENVIR::MySystemEnvir::Init(argc, argv)) {
        return -1;
    }

    // 运行sipServer环境
    MY_ENVIR::MySystemEnvir::Run();

    // 退出sipServer环境
    MY_ENVIR::MySystemEnvir::Shutdown();

    return 0;
}