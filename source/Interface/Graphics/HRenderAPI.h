#pragma once

namespace harmony
{
    class RenderAPI
    {
    public:

        struct Description
        {
			bool useWarpDevice = false;
        };

        virtual ~RenderAPI() = default;
        virtual const Description& getDescription() const = 0;
        virtual void initialize() = 0;
        virtual void frameBegin() = 0;
        virtual void frameEnd() = 0;
        virtual void deinitialize() = 0;
    };
}
