#pragma once

#include "common.h"

#include "Utility.h"

template<typename... Args>
inline void hLog(const char* category,
    const char* file,
    const char* func,
    int line,
    Args&&... args)
{
    utility::print(
        utility::stringFormat(
            "[", category, "] ",
            utility::timeNow(), " | ",
            file, " | ",
            func, ":", line, " | ",
            std::forward<Args>(args)...,
            "\n"
        )
    );

    std::fflush(stdout);
}

#define HLOG(category, ...) hLog(#category, __FILE__, __func__, __LINE__, __VA_ARGS__)

#define HASSERT(expression, ...) \
    do \
    { \
        if (!(expression)) \
        { \
            HLOG(ASSERT, \
                 "Assertion failed: %s" __VA_OPT__(": ") __VA_ARGS__, \
                 #expression __VA_OPT__(,) __VA_ARGS__); \
            std::abort(); \
        } \
    } while (0)

#define HASSERT_SOFT(expression, ...) \
    do \
    { \
        if (!(expression)) \
        { \
            HLOG(ASSERT, \
                 "Assertion failed: %s" __VA_OPT__(": ") __VA_ARGS__, \
                 #expression __VA_OPT__(,) __VA_ARGS__); \
        } \
    } while (0)
