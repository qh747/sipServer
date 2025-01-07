#include <chrono>
#include <csignal>
#include <iostream>
#include <Util/util.h>
#include <Util/logger.h>
#include <Util/onceToken.h>
#include <Util/TimeTicker.h>
#include <Network/Session.h>
#include <Network/TcpServer.h>
#include <Thread/ThreadPool.h>
#include <Poller/EventPoller.h>
using namespace std;
using namespace toolkit;

// 线程池测试
void FuncTestThreadPool()
{
    //初始化日志系统  
    Logger::Instance().add(std::make_shared<ConsoleChannel>());
    Logger::Instance().setWriter(std::make_shared<AsyncLogWriter>());

    ThreadPool pool(thread::hardware_concurrency(), ThreadPool::PRIORITY_HIGHEST, true);

    //每个任务耗时3秒 
    auto task_second = 3;
    //每个线程平均执行4次任务，总耗时应该为12秒
    auto task_count = thread::hardware_concurrency() * 4;

    semaphore sem;
    vector<int> vec;
    vec.resize(task_count);
    Ticker ticker;
    {
        //放在作用域中确保token引用次数减1 
        auto token = std::make_shared<onceToken>(nullptr, [&]() {
            sem.post();
        });

        for (auto i = 0; i < task_count; ++i) {
            pool.async([token, i, task_second, &vec]() {
                setThreadName(("thread pool " + to_string(i)).data());
                std::this_thread::sleep_for(std::chrono::seconds(task_second)); //休眠三秒
                InfoL << "task " << i << " done!";
                vec[i] = i;
            });
        }
    }

    sem.wait();
    InfoL << "all task done, used milliseconds:" << ticker.elapsedTime();

    //打印执行结果 
    for (auto i = 0; i < task_count; ++i) {
        InfoL << vec[i];
    }
}

// 事件池测试
void FuncTestEventPool()
{
    static bool exit_flag = false;
    signal(SIGINT, [](int) { exit_flag = true; });
    //设置日志
    Logger::Instance().add(std::make_shared<ConsoleChannel>());

    Ticker ticker;
    while(!exit_flag) {
        if(ticker.elapsedTime() > 1000){
            auto vec = EventPollerPool::Instance().getExecutorLoad();
            _StrPrinter printer;
            for(auto load : vec){
                printer << load << "-";
            }
            DebugL << "cpu负载:" << printer;

            EventPollerPool::Instance().getExecutorDelay([](const vector<int> &vec){
                _StrPrinter printer;
                for(auto delay : vec){
                    printer << delay << "-";
                }
                DebugL << "cpu任务执行延时:" << printer;
            });
            ticker.resetTime();
        }

        EventPollerPool::Instance().getExecutor()->async([](){
            auto usec = rand() % 4000;
            //DebugL << usec;
            usleep(usec);
        });

        usleep(2000);
    }
}

// TcpServer测试
class EchoSession: public Session {
public:
    EchoSession(const Socket::Ptr &sock) : Session(sock) { DebugL; }
    ~EchoSession() { DebugL; }

    void onRecv(const Buffer::Ptr &buf) override{
        //处理客户端发送过来的数据 
        TraceL << buf->data() <<  " from port:" << get_local_port();
        send(buf);
    }

    void onError(const SockException &err) override{
        //客户端断开连接或其他原因导致该对象脱离TCPServer管理
        WarnL << err;
    }

    void onManager() override{
        //定时管理该对象，譬如会话超时检查
        DebugL;
    }

private:
    Ticker _ticker;
};

void FuncTestTcpEchoServer()
{
    // 初始化日志模块 
    Logger::Instance().add(std::make_shared<ConsoleChannel>());
    Logger::Instance().setWriter(std::make_shared<AsyncLogWriter>());

    // 监听9000端口
    TcpServer::Ptr server(new TcpServer());
    server->start<EchoSession>(9000);

    // 退出程序事件处理
    static semaphore sem;
    signal(SIGINT, [](int) { sem.post(); });
    sem.wait();
}

int main() 
{
    // 线程池测试
    // FuncTestThreadPool();

    // 事件池测试
    // FuncTestEventPool();

    // TcpServer测试
    FuncTestTcpEchoServer();

    return 0;
}