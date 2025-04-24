#pragma once

#include "common.h"
#include "directx.h"

#include "Device.h"

#include "DescriptorHandle.h"
#include "GPUResource.h"
#include "Format.h"

namespace graphics
{
    namespace dx12
    {
        /*
        * The class represents a texture on GPU Memory
        */
        class GPUTexture : public GPUResource
        {
        public:
            GPUTexture(const Description& createDescription);

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
            void createShaderResourceView(Device& device, DescriptorHandle& srvHandle, const ShaderResourceViewInfo& viewInfo = {}) const;

            // Reset the resource.
            void reset();

            virtual ~GPUTexture()
            {
                destroy();
            }

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
        * And the GPUTexture::Descripton must have an appropriate format.
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
                Type type;
                GPUTexture::Description textureDescription;
            };

            RenderTarget(const Description& createDescription);

            // baseResource is expected to have called setName for debugging
            void createFromSwapchain(Device& device, DescriptorHandle& rtvHandle, ID3D12Resource* baseResource);

            void createRenderTargetView(Device& device, DescriptorHandle& rtvHandle, uint32_t mipSlice = 0, uint32_t arraySlice = 0);
            void createDepthStencilView(Device& device, DescriptorHandle& dsvHandle, uint32_t mipSlice = 0, uint32_t arraySlice = 0, bool readOnly = false); // TODO: Support multiple read only depth and stencil views

            inline const Description& getDescription() const { return mDescription; }
            inline const GPUTexture& getTexture() const { return mTexture; }
            inline GPUTexture& getTexture() { return mTexture; }

        private:
            Description mDescription;
            GPUTexture mTexture;
        };
    }
}
