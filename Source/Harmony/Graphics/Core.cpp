#include "Core.h"

namespace graphics
{
    Core& Core::getInstance()
    {
        static Core instance;

        return instance;
    }

    Core::Core()
        : mDevice(), mCommandListManager(), mCommandContextPool()
    {
    }

    Core::~Core()
    {
    }
}
