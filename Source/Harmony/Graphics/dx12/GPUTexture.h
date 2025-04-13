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
        /*
        * The class represents a texture on GPU Memory
        */
        class Texture : public GPUResource
        {
        public:
            Texture(const Description& createDescription);

            virtual ~Texture()
            {
                destroy();
            }

        public:
            // Creates a filtered and valid GPUResource::Description based on the initData
            const GPUResource::Description describeResource() const override;

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

            void attachResource(GPUResource& resource);

        private:
            Description mDescription;
        };
    }

    namespace dx12
    {
        /*
        * RenderTarget is an extension to the texture class with ability to create RTV and DSV.
        * It owns an instance of texture.
        * A rendertarget can be of either two types. Color or Depth.
        */
        class RenderTarget
        {
        public:
            enum class Type
            {
                Color,
                Depth
            };

            struct Description
            {
                const Type type;
                Texture::Description textureDescription;
            };

            RenderTarget(const Description& description);

            void createFromSwapchain(DescriptorHandle& rtvHandle, GPUResource& baseResource);

            void createRTV(DescriptorHandle& rtvHandle, uint32_t mipSlice = 0, uint32_t arraySlice = 0);
            void createDSV(DescriptorHandle& dsvHandle, uint32_t mipSlice = 0, uint32_t arraySlice = 0, bool readOnly = false); // TODO: Support multiple read only depth and stencil views

            inline const Description& getDescription() const { return mDescription; }
            inline const Texture& getTexture() const { return mTexture; }

        private:
            Description mDescription;
            Texture mTexture;
        };
    }
}
