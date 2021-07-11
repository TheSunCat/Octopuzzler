#include "jthread.h"

jthread::jthread(ThreadFunc func)
    : t([this, func] {
        using namespace std::chrono_literals;

        while(!this->started)
            std::this_thread::sleep_for(1ms);

        while(this->running)
            func();
    })
{

}

void jthread::start()
{
    started = true;
}

void jthread::stop()
{
    running = false;
    t.join();
}