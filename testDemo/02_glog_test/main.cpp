#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>

int main(int argc, char* argv[]) {
    // 使用glog之前必须先初始化库，仅需执行一次，括号内为程序名
    google::InitGoogleLogging(argv[0]);

    // 是否将日志输出到文件和stderr
    FLAGS_alsologtostderr = true;

    // 是否启用不同颜色显示
	FLAGS_colorlogtostderr = true;

    // 日志输出
    LOG(INFO) << "log info";
    LOG(WARNING) << "log warning";
    LOG(ERROR) << "log error";

    // 当要结束glog时必须关闭库，否则会内存溢出
    google::ShutdownGoogleLogging();
    return 0;
}