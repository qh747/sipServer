#include <thread>
#include <csignal>
#include <iostream>
#include <Util/util.h>
#include <Util/logger.h>
#include <Util/onceToken.h>
#include <Util/TimeTicker.h>
#include <Network/Session.h>
#include <Network/TcpServer.h>
#include <Thread/ThreadPool.h>
#include <Poller/Timer.h>
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

// 定时器测试
void FuncTestTicker()
{
    // 设置日志
    Logger::Instance().add(std::make_shared<ConsoleChannel>());
    Logger::Instance().setWriter(std::make_shared<AsyncLogWriter>());

    // 创建一个重复执行的定时器任务
    Timer::Ptr timer0 = std::make_shared<Timer>(0.5f, []() {
        std::cout << "Timer0: This task repeats every 0.5 seconds." << std::endl;
        return true; // 返回true表示任务重复执行
    }, EventPollerPool::Instance().getFirstPoller());

    // 创建一个只执行一次的定时器任务
    Timer::Ptr timer1 = std::make_shared<Timer>(1.0f, []() {
        std::cout << "Timer1: This task will only run once after 1 second." << std::endl;
        return false; // 返回false表示任务只执行一次
    }, EventPollerPool::Instance().getFirstPoller());

    // 创建一个可能抛出异常的定时器任务
    // Timer::Ptr timer2 = std::make_shared<Timer>(2.0f, []() {
    //     std::cout << "Timer2: This task runs every 2 seconds and may throw an exception." << std::endl;
    //     throw std::runtime_error("Timer2: An exception occurred.");
    // }, EventPollerPool::Instance().getFirstPoller());

    // 等待用户中断程序
    std::cout << "Press Ctrl+C to exit." << std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() 
{
    // 线程池测试
    // FuncTestThreadPool();

    // 事件池测试
    // FuncTestEventPool();

    // 定时器测试
    FuncTestTicker();

    return 0;
}