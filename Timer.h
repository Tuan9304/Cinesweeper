#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <thread>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <iostream>

namespace Time {
    struct Timer {
        template<typename Func> void setInterval(Func func, int interval);
        void stop();
    private:
        std::thread t;
        std::atomic<bool> exitThread = 0;
        std::mutex mtxAlert;
        std::condition_variable sigAlert;
    };
}

// ----------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------

template<typename Func> void Time::Timer::setInterval(Func func, int interval) {
    exitThread = 0;
    if(t.joinable()) {
        std::cerr << "Thread error\n";
    }
    else {
        t = std::thread([=]() {
            while(!exitThread) {
                {
                    std::unique_lock<std::mutex> lock(mtxAlert);
                    if(sigAlert.wait_for(lock, std::chrono::milliseconds(interval), [&]() { return exitThread.load(); })) {
                        break;
                    }
                }
                func();
            }
        });
    }
}

inline void Time::Timer::stop() {
    {
        std::lock_guard<std::mutex> guard(mtxAlert);
        exitThread = 1;
    }
    sigAlert.notify_all();
    if(t.joinable()) {
        t.join();
    }
}

#endif // TIMER_H_INCLUDED
