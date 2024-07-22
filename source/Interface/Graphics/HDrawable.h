#pragma once

#include "HBuffer.h"

namespace harmony
{
    class Image;
    class Sampler;
}

namespace harmony
{
    class Drawable
    {
    public:
        struct StageBindings
        {
            const harmony::Image* images[12] = { nullptr };
            unsigned int imgCount = 0;

            const harmony::Sampler* samplers[8] = { nullptr };
            unsigned int smpCount = 0;
        };

        struct Description
        {
            const char* label = "default drawable";
            harmony::Buffer::Description vertexBufferDescription = { 0 };
            harmony::Buffer::Description indexBufferDescription = { 0 };
            StageBindings vs = { 0 };
            StageBindings fs = { 0 };
        };

        virtual ~Drawable() = default;

    public:
        virtual const Description& getDescription() const = 0;

        virtual void initialize() = 0;
        virtual void bind() const = 0;
        virtual void draw(int baseElement, int numElements, int numInstances = 1) const = 0;
        virtual void unbind() const = 0;
        virtual void deinitialize() = 0;
    };
}
