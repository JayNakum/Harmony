#include "GPUTexture.h"

#include "Core.h"

#include "types/GPUResourceDimension.h"

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

        const GPUResource::Description Texture::describeResource() const
        {
            GPUResource::Description createDescription{};

            createDescription.dimension = mDescription.dimension;
            createDescription.width = mDescription.width;
            createDescription.height = mDescription.height;
            createDescription.depth = mDescription.depth;
            createDescription.format = mDescription.format;
            createDescription.sampleCount = mDescription.sampleCount;
            createDescription.sampleQuality = mDescription.sampleQuality;

            return createDescription;
        }

        void Texture::createShaderResourceView(DescriptorHandle& srvHandle, const ShaderResourceViewInfo& viewInfo) const
        {
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
            srvDesc.Format = toDXGIFormat(mDescription.format);
            srvDesc.ViewDimension = toD3DSRVDimension(mDescription.dimension);
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

            switch (mDescription.dimension)
            {
            default:
                HASSERT(false, "Unhandled SRV creation path for resource dimension: %s", toString(mDescription.dimension));
                break;

            case GPUResourceDimension::Texture1D:
                srvDesc.Texture1D.MostDetailedMip = viewInfo.mostDetailedMip;
                srvDesc.Texture1D.MipLevels = (viewInfo.mipLevels == UINT32_MAX)
                    ? (mDescription.mipLevels - viewInfo.mostDetailedMip)
                    : viewInfo.mipLevels;
                srvDesc.Texture1D.ResourceMinLODClamp = viewInfo.resourceMinLODClamp;
                break;

            case GPUResourceDimension::Texture2D:
                srvDesc.Texture2D.MostDetailedMip = viewInfo.mostDetailedMip;
                srvDesc.Texture2D.MipLevels = (viewInfo.mipLevels == UINT32_MAX)
                    ? (mDescription.mipLevels - viewInfo.mostDetailedMip)
                    : viewInfo.mipLevels;
                srvDesc.Texture2D.PlaneSlice = 0;
                srvDesc.Texture2D.ResourceMinLODClamp = viewInfo.resourceMinLODClamp;
                break;

            case GPUResourceDimension::Texture3D:
                srvDesc.Texture3D.MostDetailedMip = viewInfo.mostDetailedMip;
                srvDesc.Texture3D.MipLevels = (viewInfo.mipLevels == UINT32_MAX)
                    ? (mDescription.mipLevels - viewInfo.mostDetailedMip)
                    : viewInfo.mipLevels;
                srvDesc.Texture3D.ResourceMinLODClamp = viewInfo.resourceMinLODClamp;
                break;

            case GPUResourceDimension::Texture1DArray:
                srvDesc.Texture1DArray.MostDetailedMip = viewInfo.mostDetailedMip;
                srvDesc.Texture1DArray.MipLevels = (viewInfo.mipLevels == UINT32_MAX)
                    ? (mDescription.mipLevels - viewInfo.mostDetailedMip)
                    : viewInfo.mipLevels;
                srvDesc.Texture1DArray.FirstArraySlice = viewInfo.firstArraySlice;
                srvDesc.Texture1DArray.ArraySize = viewInfo.arraySize;
                srvDesc.Texture1DArray.ResourceMinLODClamp = viewInfo.resourceMinLODClamp;
                break;

            case GPUResourceDimension::Texture2DArray:
                srvDesc.Texture2DArray.MostDetailedMip = viewInfo.mostDetailedMip;
                srvDesc.Texture2DArray.MipLevels = (viewInfo.mipLevels == UINT32_MAX)
                    ? (mDescription.mipLevels - viewInfo.mostDetailedMip)
                    : viewInfo.mipLevels;
                srvDesc.Texture2DArray.FirstArraySlice = viewInfo.firstArraySlice;
                srvDesc.Texture2DArray.ArraySize = viewInfo.arraySize;
                srvDesc.Texture2DArray.PlaneSlice = 0;
                srvDesc.Texture2DArray.ResourceMinLODClamp = viewInfo.resourceMinLODClamp;
                break;
            }

            GfxDevice()->CreateShaderResourceView(mResource.Get(), &srvDesc, srvHandle);
        }

        void Texture::attachResource(GPUResource& resource)
        {
            mDescription = resource.describeResource();
            mResource.Attach(resource.get());
        }

        void Texture::reset()
        {
            mResource.Reset();
        }
    }

    namespace dx12
    {
        RenderTarget::RenderTarget(const Description& createDescription)
            : mDescription(createDescription), mTexture(createDescription.textureDescription)
        {
            bool isValidColorFlag = hasFlag(mDescription.textureDescription.resourceFlags, GPUResourceFlags::AllowRenderTarget) && (mDescription.type == Type::Color);
            bool isValidDepthFlag = hasFlag(mDescription.textureDescription.resourceFlags, GPUResourceFlags::AllowDepthStencil) && (mDescription.type == Type::Depth);

            HASSERT(isValidColorFlag || isValidDepthFlag, "Invalid GPUResourceFlag %s for render target.", toString(mDescription.textureDescription.resourceFlags));
        }

        void RenderTarget::createFromSwapchain(DescriptorHandle& rtvHandle, GPUResource& baseResource)
        {
            HASSERT(mDescription.type == Type::Color, "Cannot create RTV for a Depth RenderTarget.");

            mTexture.attachResource(baseResource);
            GfxDevice()->CreateRenderTargetView(mTexture.get(), nullptr, rtvHandle);
        }

        void RenderTarget::createRenderTargetView(DescriptorHandle& rtvHandle, uint32_t mipSlice, uint32_t arraySlice)
        {
            HASSERT(mDescription.type == Type::Color, "Cannot create RTV for a Depth RenderTarget.");

            D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
            rtvDesc.Format = toDXGIFormat(mTexture.describeResource().format);
            switch (mTexture.describeResource().dimension)
            {
            case GPUResourceDimension::Texture2D:
                rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                rtvDesc.Texture2D.MipSlice = mipSlice;
                break;

            case GPUResourceDimension::Texture2DArray:
                rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
                rtvDesc.Texture2DArray.MipSlice = mipSlice;
                rtvDesc.Texture2DArray.FirstArraySlice = arraySlice;
                rtvDesc.Texture2DArray.ArraySize = mTexture.describeResource().depth;
                break;

            default:
                HASSERT(false, "Unsupported resource dimension %s for RTV creation.", toString(mTexture.describeResource().dimension));
                return;
            }

            GfxDevice()->CreateRenderTargetView(mTexture.get(), &rtvDesc, rtvHandle);
        }

        void RenderTarget::createDepthStencilView(DescriptorHandle& dsvHandle, uint32_t mipSlice, uint32_t arraySlice, bool readOnly)
        {
            HASSERT(mDescription.type == Type::Depth, "Cannot create DSV for a Color RenderTarget.");

            D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
            dsvDesc.Format = toDXGIFormat(mTexture.describeResource().format);

            dsvDesc.Flags = readOnly ? D3D12_DSV_FLAG_READ_ONLY_DEPTH | D3D12_DSV_FLAG_READ_ONLY_STENCIL : D3D12_DSV_FLAG_NONE;

            switch (mTexture.describeResource().dimension)
            {
            case GPUResourceDimension::Texture2D:
                dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                dsvDesc.Texture2D.MipSlice = mipSlice;
                break;

            case GPUResourceDimension::Texture2DArray:
                dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
                dsvDesc.Texture2DArray.MipSlice = mipSlice;
                dsvDesc.Texture2DArray.FirstArraySlice = arraySlice;
                dsvDesc.Texture2DArray.ArraySize = mTexture.describeResource().depth;
                break;

            default:
                HASSERT(false, "Unsupported resource dimension %s for DSV creation.", toString(mTexture.describeResource().dimension));
                return;
            }

            GfxDevice()->CreateDepthStencilView(mTexture.get(), &dsvDesc, dsvHandle);
        }
    }
}
