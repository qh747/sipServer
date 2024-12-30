#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>

int main(int argc, char* argv[]) {
  // 初始化 Google's logging library.
  google::InitGoogleLogging(argv[0]);

  // 设置日志文件的路径
  FLAGS_log_dir = ".";  // 当前目录
  FLAGS_logtostderr = 0;  // 不输出到标准错误，只输出到文件

  // 设置日志文件名前缀，这样就会生成 xx.INFO，xx.WARNING，xx.ERROR 等文件
  FLAGS_log_prefix = true;
  FLAGS_logbuflevel = -1;  // 实时输出日志

  // 设置日志文件名
  google::SetLogDestination(google::INFO, "xx.log");

  // 记录日志
  LOG(INFO) << "This is an info message.";
  LOG(WARNING) << "This is a warning message.";
  LOG(ERROR) << "This is an error message.";

  // 清理 glog
  google::ShutdownGoogleLogging();

  return 0;
}
