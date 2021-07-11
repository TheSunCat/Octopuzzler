#include <thread>
#include <functional>
#include <atomic>

typedef std::function<void()> ThreadFunc;

class jthread
{
public:
    jthread() = default;
    ~jthread();

    jthread(ThreadFunc func);

    void start();
    void stop();
protected:
    std::atomic<bool> running{true};
    std::atomic<bool> started{false};
    std::thread t;
};