#pragma once

#include "common.h"

#include "Utility.h"

inline void hLog(const char* category,
    const char* file,
    const char* func,
    int line,
    const std::string message)
{
    utility::print(
        utility::stringFormat(
            "[", category, "] ",
            utility::timeNow(), " | ",
            file, " | ",
            func, ":", line, " | ",
            message,
            "\n"
        )
    );

    std::fflush(stdout);
}

#define HLOG(category, ...) hLog(#category, __FILE__, __func__, __LINE__, utility::stringFormat(__VA_ARGS__))

#define HASSERT(expression, ...) \
    do \
    { \
        if (!(expression)) \
        { \
            HLOG(ASSERT, "Assertion failed: ", #expression, __VA_OPT__(": ") __VA_ARGS__); \
            std::abort(); \
        } \
    } while (0)

#define HASSERT_SOFT(expression, ...) \
    do \
    { \
        if (!(expression)) \
        { \
            HLOG(ASSERT, "Assertion failed: ", #expression, __VA_OPT__(": ") __VA_ARGS__); \
        } \
    } while (0)

#if defined(HDEBUG)

#define HDEBUG_ONLY(...) __VA_ARGS__

#else // defined(HDEBUG)

#define HDEBUG_ONLY(...)

#endif // defined(HDEBUG)
