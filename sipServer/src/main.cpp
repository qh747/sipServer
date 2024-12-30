#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include "envir/mySystemEnvir.h"

int main(int argc, char* argv[]) 
{
    MY_ENVIR::MySystemEnvir::Init(argc, argv);
    MY_ENVIR::MySystemEnvir::Run();

    return 0;
}