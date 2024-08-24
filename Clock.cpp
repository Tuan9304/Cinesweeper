#include "Clock.h"

void Time::Clock::load(int init) {
    t.stop();
    timer = init;
    t.setInterval([&]() {
        timer++;
    }, 238);
}

void Time::Clock::stop() {
    t.stop();
}

int Time::Clock::restart() {
    int et = timer;
    load(0);
    return et;
}

int Time::Clock::getElapsedTime() {
    return timer >> hs;
}

int Time::Clock::getRawTime() {
    return timer;
}
