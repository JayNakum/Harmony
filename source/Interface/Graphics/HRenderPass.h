#pragma once

namespace harmony
{
    class RenderTarget;
}

namespace harmony
{
    class RenderPass
    {
    public:
        struct Description
        {
            const char* label = "default render pass";
        };

        virtual ~RenderPass() = default;

    public:
        virtual const void* getID() const = 0;
        virtual const Description& getDescription() const = 0;

        virtual void initialize() = 0;
        virtual void render(const harmony::RenderTarget* renderTarget) = 0;
        virtual void deinitialize() = 0;
    };
}
