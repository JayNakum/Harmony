#pragma once

#include "HBuffer.h"
#include "HTexture.h"

namespace harmony
{
    class Drawable
    {
    public:
        struct StageBindings
        {
            harmony::Image* images[12] = { nullptr };
            unsigned int imgCount = 0;

            harmony::Sampler* samplers[8] = { nullptr };
            unsigned int smpCount = 0;
        };

        struct Description
        {
            const char* label = "default drawable";
            harmony::Buffer* vertexBuffer = nullptr;
            harmony::Buffer* indexBuffer = nullptr;

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
