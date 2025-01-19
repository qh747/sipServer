#include <thread>
#include <chrono>
#include <iostream>
#include <boost/core/noncopyable.hpp>
#include <boost/thread/detail/singleton.hpp>

class MySingleton : public boost::noncopyable
{
private:
  friend class boost::detail::thread::singleton<MySingleton>;

private:
    // 私有构造函数和析构函数
    MySingleton() { m_val = 0; }
    ~MySingleton() {}

    // 其他私有成员
public:
    // 禁止复制构造函数和赋值操作符
    MySingleton(const MySingleton&) = delete;
    MySingleton& operator=(const MySingleton&) = delete;

    // 获取单例实例的静态方法
    static MySingleton& getInstance() {
        return boost::detail::thread::singleton<MySingleton>::instance();
    }

    // 公共接口
    void doSomething() {
        m_val = m_val + 10;
        std::cout << "Doing something with value: " << m_val  << " thread id: " << std::this_thread::get_id() << std::endl;
    }

private:
    int m_val;
};

void threadFunc()
{
    for (int i = 0; i < 100; i++)
    {
        // 获取单例实例
        MySingleton& singletonInstance = MySingleton::getInstance();

        // 使用单例实例
        singletonInstance.doSomething();

        // 休眠100毫秒
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() 
{
    std::thread t1(threadFunc);
    std::thread t2(threadFunc);
    std::thread t3(threadFunc);

    t1.join();
    t2.join();
    t3.join();

    return 0;
}
