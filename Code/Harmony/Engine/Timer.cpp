#include "Timer.h"

namespace engine
{
    Timer::Timer()
    {
        reset();
    }

    float Timer::restart()
    {
        TimePoint now = Clock::now();
        std::chrono::duration<float> elapsed = now - mLast;
        mLast = now;
        return elapsed.count(); // in seconds
    }

    float Timer::elapsed() const
    {
        TimePoint now = Clock::now();
        std::chrono::duration<float> elapsed = now - mLast;
        return elapsed.count();
    }

    void Timer::reset()
    {
        mLast = Clock::now();
    }

    Timer::~Timer()
    {
        reset();
    }
}
