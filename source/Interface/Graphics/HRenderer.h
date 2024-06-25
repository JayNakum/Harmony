#pragma once

namespace harmony
{
    class Renderer
    {
    public:
        struct Description
        {
			bool useWarpDevice = false;
        };

        virtual ~Renderer() = default;
        virtual const Description& getDescription() const = 0;
        virtual void initialize() = 0;
        virtual void render() = 0;
        virtual void deinitialize() = 0;
    };
}
