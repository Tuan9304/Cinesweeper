#ifndef CLOCK_H_INCLUDED
#define CLOCK_H_INCLUDED

#include "Timer.h"

namespace Time {
    struct Clock {
        const int hs = 2;
        void load(int init);
        void stop();
        int restart();
        int getElapsedTime();
        int getRawTime();
    private:
        int timer = 0;
        Time::Timer t;
    };
}

#endif // CLOCK_H_INCLUDED
