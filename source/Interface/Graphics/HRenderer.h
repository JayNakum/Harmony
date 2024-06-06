#pragma once

namespace harmony
{
    class Renderer
    {
    public:
        virtual void initialize() = 0;
        virtual void deinitialize() = 0;
    };
}
