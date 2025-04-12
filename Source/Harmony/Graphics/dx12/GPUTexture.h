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
            Texture(const Description& createDescription);

            virtual ~Texture()
            {
                destroy();
            }

        public:
            inline const Description& getDescription() const { return mDescription; }

            // Creates an SRV with custom mip level, mip count, arraySlice and arraySize.
            // Requires a pre allocated descriptorHandle& from a descriptor heap.
            void createShaderResourceView(DescriptorHandle& srvHandle, unsigned int mipLevel, unsigned int mipCount, unsigned int arraySlice, unsigned int arraySize) const;

        protected:
            // Creates a filtered and valid GPUResource::Description based on the initData
            const GPUResource::Description describeResource() const override;

        private:
            const Description mDescription;
        };
    }
}
