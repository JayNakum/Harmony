#pragma once

#include "common.h"

namespace math
{
    template <typename T> inline T alignUpWithMask(T value, size_t mask)
    {
        return (T)(((size_t)value + mask) & ~mask);
    }

    template <typename T> inline T alignDownWithMask(T value, size_t mask)
    {
        return (T)((size_t)value & ~mask);
    }

    template <typename T> inline T alignUp(T value, size_t alignment)
    {
        return alignUpWithMask(value, alignment - 1);
    }

    template <typename T> inline T alignDown(T value, size_t alignment)
    {
        return alignDownWithMask(value, alignment - 1);
    }

    template <typename T> inline bool isAligned(T value, size_t alignment)
    {
        return 0 == ((size_t)value & (alignment - 1));
    }

    template <typename T> inline T divideByMultiple(T value, size_t alignment)
    {
        return (T)((value + alignment - 1) / alignment);
    }

    template <typename T> inline bool isPowerOfTwo(T value)
    {
        return 0 == (value & (value - 1));
    }

    template <typename T> inline bool isDivisible(T value, T divisor)
    {
        return (value / divisor) * divisor == value;
    }
}
