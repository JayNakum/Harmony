#pragma once

#include "common.h"
#include "Log.h"

namespace engine
{
    class Timer
    {
        using Clock = std::chrono::high_resolution_clock;
        using TimePoint = Clock::time_point;

    public:
        Timer();

        // Reset the timer and return elapsed time in seconds
        float restart();

        // Returns the time since construction, in seconds
        float total() const;

        // Returns the time since last restart or construction, in seconds
        float elapsed() const;

        // Resets the timer to now
        void reset();

        ~Timer();

    private:
        TimePoint mStart;
        TimePoint mLast;
    };
}
