#pragma once

#include "common.h"
#include "Log.h"

namespace engine
{
    class Data
    {
    protected:
        virtual void renderDebugUI() = 0;
        virtual void reset() = 0;
        virtual ~Data() = default;
    };
}
