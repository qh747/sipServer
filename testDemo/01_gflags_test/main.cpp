#include <iostream>
#include <gflags/gflags.h>

// 定义变量
DEFINE_string(name  , "xiaoming", "user name");
DEFINE_uint32(age   , 18        , "user age");
DEFINE_uint64(id    , 123456    , "user id");
DEFINE_double(weight, 70.1      , "user weight");

int main(int argc, char** argv)
{
    // 命令行参数解析
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // 使用变量
    std::cout << "name: " << FLAGS_name << " id: " << FLAGS_id << " age: " << FLAGS_age << " weight: " << FLAGS_weight << std::endl;
}
