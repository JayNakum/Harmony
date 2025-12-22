#pragma once

#include "common.h"

namespace harmony
{
    /*
    * A generic harmony application.
    * This is an interface to allow multiple apps.
    * There will mainly be two Applications implementing this. Engine and Sandbox.
    */
    class Application
    {
    public:
        virtual void initialize() = 0;
        virtual void run() = 0;
        virtual void deinitialize() = 0;
        virtual ~Application() = default;
    };
}
