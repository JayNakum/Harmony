#include "GPUTexture.h"

#include "Renderer.h"

namespace graphics
{
    namespace dx12
    {
        Texture::Texture(const Description& createDescription)
            : mDescription(createDescription)
        {
            destroy();
            mUsageState = GPUResourceState::CopyDest; // Since the resource is created on default heap, it expects some data to be copied to.
        }

        void Texture::createShaderResourceView(DescriptorHandle& srvHandle, unsigned int mipLevel, unsigned int mipCount, unsigned int arraySlice, unsigned int arraySize) const
        {

        }

        const GPUResource::Description Texture::describeResource() const
        {
            GPUResource::Description description{};

            description.dimension = mDescription.dimension;
            description.width = mDescription.width;
            description.height = mDescription.height;
            description.depth = mDescription.depth;
            description.format = mDescription.format;

            return description;
        }
    }
}
