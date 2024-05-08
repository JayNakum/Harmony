#pragma once

#include "HBuffer.h"

namespace harmony
{
    class Drawable
    {
    public:
        struct Description
        {
            Buffer*& vertexBuffer;
            Buffer*& indexBuffer;
        };

        virtual ~Drawable() = default;

    public:
        virtual const Description& getDescription() const = 0;
        virtual void setDescription(const Description& description) = 0;

        virtual void initialize() = 0;
        virtual void bind() const = 0;
        virtual void draw(int baseElement, int numElements, int numInstances = 1) const = 0;
        virtual void unbind() const = 0;
        virtual void deinitialize() = 0;
    };
}
