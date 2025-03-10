#pragma once

#include <cstdio>

#include <string>
#include <locale>

#include <array>
#include <vector>
#include <list>
#include <queue>
#include <unordered_map>

#include <memory>

#include <mutex>

#include <cmath>
#include <ctime>

/*
* Deletes the copy constructor and = operator.
* Used to make the code clean.
*/
class NonCopyable
{
protected:
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};
