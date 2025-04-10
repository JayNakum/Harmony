#pragma once

#include "common.h"

#include "GPUResource.h"
#include "DescriptorHandle.h"
#include "Heap.h"

#include "Format.h"

namespace graphics
{
    namespace dx12
    {
        class Texture : public GPUResource
        {
        public:
            Texture()
                : mWidth(0), mHeight(0), mDepth(0), mTextureType(GPUResourceDimension::Unknown)
            { }

            virtual ~Texture()
            {
                destroy();
            }

            struct Description
            {
                std::string name;

                size_t rowPitchBytes;
                size_t width;
                size_t height;
                Format format;

                GPUResourceDimension textureType = GPUResourceDimension::Texture2D;
            };

            // Create a committed resource.
            void createCommitted(const Description& createDescription);

            // Sub-Allocate a buffer out of a pre-allocated heap.
            void createPlaced(const Description& createDescription, Heap& backingHeap, uint64_t heapOffset);

        public:
            inline const size_t getWidth() const {}
            inline const size_t getHeight() const {}
            inline const size_t getDepth() const {}


            // Creates an SRV with custom mip level, mip count, arraySlice and arraySize.
            // Requires a pre allocated descriptorHandle& from a descriptor heap.
            void createShaderResourceView(DescriptorHandle& srvHandle, unsigned int mipLevel, unsigned int mipCount, unsigned int arraySlice, unsigned int arraySize) const;

        private:
            void initialize(const Description& createDescription);

        private:
            size_t mWidth;
            size_t mHeight;
            size_t mDepth;
            GPUResourceDimension mTextureType;
        };
    }
}
