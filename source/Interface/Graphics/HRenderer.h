#pragma once

#include <functional>

namespace harmony
{
    class Renderer
    {
    public:
        virtual void initialize() = 0;
        std::function<void()> frame;
        virtual void render() = 0;
        virtual void deinitialize() = 0;
    };
}
