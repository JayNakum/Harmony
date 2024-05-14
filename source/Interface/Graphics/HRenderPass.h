#pragma once

namespace harmony
{
    class RenderPass
    {
    public:
        struct Action
        {
            struct Attachment
            {
                float value[4];
            };

            Attachment colors;
        };

        struct Swapchain
        {
            unsigned int width = 0;
            unsigned int height = 0;
            void* target;
        };

        struct Description
        {
            const char* label = "default render pass";
            Action action;
            Swapchain swapchain;
        };

        virtual ~RenderPass() = default;

    public:
        virtual const void* getID() const = 0;
        virtual const Description& getDescription() const = 0;

        virtual void initialize() = 0;
        virtual void begin() = 0;
        virtual void end() const = 0;
        virtual void deinitialize() = 0;
    };
}
