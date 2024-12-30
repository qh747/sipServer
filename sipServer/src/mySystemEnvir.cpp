#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include "envir/mySignaleCatch.h"
#include "envir/mySystemEnvir.h"

namespace MY_ENVIR
{
    MyStatus_t MySystemEnvir::Init()
    {
        // 日志初始化
        google::InitGoogleLogging("sipServer");
        // 是否将日志输出到文件和stderr
        FLAGS_alsologtostderr = true;
        // 是否启用不同颜色显示
	    FLAGS_colorlogtostderr = true;

        return MyStatus_t::SUCCESS;
    }

    MyStatus_t MySystemEnvir::Run()
    {
        // 信号处理阻塞程序退出
        MyStatus_t status = MySignalCatcher::Run();

        // 资源销毁，后续补充
        status = MySystemEnvir::Shutdown();
        return status;
    }

    MyStatus_t MySystemEnvir::Shutdown()
    {
        // 当要结束glog时必须关闭库，否则会内存溢出
        google::ShutdownGoogleLogging();
        return MyStatus_t::SUCCESS;
    }

}; // namespace MY_ENVIR