#pragma once

namespace harmony
{
    class Application
    {
    public:
        virtual void initialize() = 0;
        virtual void run() = 0;
        virtual void deinitialize() = 0;
    };
}
