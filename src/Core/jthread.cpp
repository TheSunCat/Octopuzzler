#include "jthread.h"
#include "Core.h"

jthread::jthread(ThreadFunc func)
    : t([this, func] {
        using namespace std::chrono_literals;

        while(!this->started)
        {
            std::this_thread::sleep_for(1ms);

            if(!running) {
                std::cout << "JTHREAD: ERROR: Thread was stopped before starting!" << std::endl;
                return;
            }
        }

        while(this->running)
            func();

    })
{ }

jthread::~jthread()
{
    if(running)
        stop();
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