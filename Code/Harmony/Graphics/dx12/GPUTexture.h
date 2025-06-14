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

            struct ShaderResourceViewInfo
            {
                uint32_t mostDetailedMip;
                uint32_t mipLevels;
                uint32_t firstArraySlice;
                uint32_t arraySize;
                float resourceMinLODClamp;

                static const ShaderResourceViewInfo getSRVInfo(const GPUTexture::Description& desc)
                {
                    ShaderResourceViewInfo viewInfo{};
                    viewInfo.mostDetailedMip = 0;
                    viewInfo.mipLevels = desc.mipLevels; // Use all mips
                    viewInfo.firstArraySlice = 0;

                    switch (desc.dimension)
                    {
                    case GPUResourceDimension::Texture1DArray:
                    case GPUResourceDimension::Texture2DArray:
                        viewInfo.arraySize = desc.depth; // Full array size
                        break;

                    default:
                        viewInfo.arraySize = 1; // Not an array texture
                        break;
                    }

                    viewInfo.resourceMinLODClamp = 0.0f;
                    return viewInfo;
                }

            };
            // Creates an SRV with custom mip level, mip count, arraySlice and arraySize.
            // Requires a pre allocated descriptorHandle& from a descriptor heap.
            void createShaderResourceView(Device& device, DescriptorHandle& srvHandle, const ShaderResourceViewInfo& viewInfo) const;

            struct UnorderedAccessViewInfo
            {
                uint32_t mipSlice;
                uint32_t firstArraySlice;
                uint32_t arraySize;
                uint32_t firstWSlice;
                uint32_t wSize;

                static const UnorderedAccessViewInfo getUAVInfo(const GPUTexture::Description& desc)
                {
                    UnorderedAccessViewInfo viewInfo{};
                    viewInfo.mipSlice = 0;
                    viewInfo.firstArraySlice = 0;
                    viewInfo.arraySize = (desc.dimension == GPUResourceDimension::Texture1DArray ||
                                          desc.dimension == GPUResourceDimension::Texture2DArray)
                        ? desc.depth
                        : 0;
                    viewInfo.firstWSlice = 0;
                    viewInfo.wSize = (desc.dimension == GPUResourceDimension::Texture3D)
                        ? desc.depth
                        : 0;
                    return viewInfo;
                }
            };

            // Creates an UAV with custom mip level, mip count, arraySlice and arraySize.
            // Requires a pre allocated descriptorHandle& from a descriptor heap.
            void createUnorderedAccessView(Device& device, DescriptorHandle& uavHandle, const UnorderedAccessViewInfo& viewInfo) const;

            // Reset the resource.
            void reset();

            ~GPUTexture() = default;
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
            RenderTarget(const Type type, GPUTexture& texture);

            // baseResource is expected to have called setD3DName for debugging
            void createFromSwapchain(Device& device, DescriptorHandle& rtvHandle, ID3D12Resource* baseResource);

            void createRenderTargetView(Device& device, DescriptorHandle& rtvHandle, const uint32_t mipSlice = 0, const uint32_t arraySlice = 0);
            void createDepthStencilView(Device& device, DescriptorHandle& dsvHandle, const uint32_t mipSlice = 0, const uint32_t arraySlice = 0, const bool readOnlyDepth = false, const bool readOnlyStencil = false);

            inline const Description& getDescription() const { return mDescription; }
            inline const GPUTexture& getTexture() const { return mTexture; }
            inline GPUTexture& getTexture() { return mTexture; }

        private:
            Description mDescription;
            GPUTexture mTexture;
        };
    }
}
