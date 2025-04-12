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

            struct ShaderResourceViewInfo
            {
                uint32_t mostDetailedMip = 0;
                uint32_t mipLevels = UINT32_MAX; // If UINT32_MAX, use all remaining mips
                uint32_t firstArraySlice = 0;
                uint32_t arraySize = 1;
                float resourceMinLODClamp = 0.0f;
            };
            // Creates an SRV with custom mip level, mip count, arraySlice and arraySize.
            // Requires a pre allocated descriptorHandle& from a descriptor heap.
            void createShaderResourceView(DescriptorHandle& srvHandle, const ShaderResourceViewInfo& viewInfo) const;

        protected:
            // Creates a filtered and valid GPUResource::Description based on the initData
            const GPUResource::Description describeResource() const override;

        private:
            const Description mDescription;
        };
    }

    namespace dx12
    {
        class RenderTarget
        {
        public:
            enum class Type
            {
                Color,
                Depth
            };

            RenderTarget(const Type type, const Texture::Description& description);

            void createRTV(DescriptorHandle rtvHandle, uint32_t mipSlice = 0, uint32_t arraySlice = 0);
            void createDSV(DescriptorHandle dsvHandle, uint32_t mipSlice = 0, uint32_t arraySlice = 0, bool readOnly = false);

            inline const Type getType() const { return mType; }
            inline const Texture& getTexture() const { return mTexture; }
            inline Texture& getTexture() { return mTexture; }

        private:
            const Type mType;
            Texture mTexture;
        };
    }
}
