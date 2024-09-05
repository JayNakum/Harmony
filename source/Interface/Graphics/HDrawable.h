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

        struct DrawDescription
        {
            int numElements = 0;
            int baseElement = 0;
            int numInstances = 1;
            int baseInstance = 0;
        };

        struct Description
        {
            const char* label = "default drawable";
            harmony::Buffer* vertexBuffer = nullptr; // TODO: allow an array of buffers because it is supported by DX12. We can have multiple buffer views bound at a time when doing IASetVertexBuffers.
            harmony::Buffer* indexBuffer = nullptr;

            StageBindings vs = { 0 };
            StageBindings fs = { 0 };
            DrawDescription drawDescription;
        };

        virtual ~Drawable() = default;

    public:
        virtual const Description& getDescription() const = 0;

        virtual void initialize() = 0;
        virtual void bind(unsigned int slot = 0) const = 0;
        virtual void draw() const = 0;
        virtual void unbind() const = 0;
        virtual void deinitialize() = 0;
    };
}
