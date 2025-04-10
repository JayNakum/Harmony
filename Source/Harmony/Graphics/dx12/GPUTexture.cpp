#include "GPUTexture.h"

namespace graphics
{
    namespace dx12
    {
        void Texture::createCommitted(const Description& createDescription)
        {
            initialize(createDescription);

        }

        void Texture::createPlaced(const Description& createDescription, Heap& backingHeap, uint64_t heapOffset)
        {

        }

        void Texture::initialize(const Description& createDescription)
        {
            mWidth = createDescription.width;
            mHeight = createDescription.height;
            mDepth = createDescription.textureType == GPUResourceDimension::Texture3D ? 6 : 1;

            mTextureType = createDescription.textureType;
            if (createDescription.textureType == GPUResourceDimension::Buffer)
            {
                HASSERT_SOFT(false, "Buffer is an invalid texture type. Using Texture1D instead.");
                mTextureType = GPUResourceDimension::Texture1D;
            }
        }
    }
}
