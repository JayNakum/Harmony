#include "Texture.h"

#include "types/GPUResourceDimension.h"

namespace graphics
{
    Texture::Texture(const Description& createDescription)
        : GPUResource(createDescription)
    {
        HASSERT(createDescription.dimension != GPUResourceDimension::Buffer, "Invalid resource dimension for texture.");
    }

    void Texture::createShaderResourceView(Device& device, DescriptorHandle& srvHandle, const ShaderResourceViewInfo& viewInfo) const
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = dx12::toDXGIFormat(mDescription.format);
        srvDesc.ViewDimension = dx12::toD3DSRVDimension(mDescription.dimension);
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        switch (mDescription.dimension)
        {
        default:
            HASSERT(false, "Unhandled SRV creation path for resource dimension: %s", toString(mDescription.dimension).c_str());
            break;

        case GPUResourceDimension::Texture1D:
            srvDesc.Texture1D.MostDetailedMip = viewInfo.mostDetailedMip;
            srvDesc.Texture1D.MipLevels = viewInfo.mipLevels;
            srvDesc.Texture1D.ResourceMinLODClamp = viewInfo.resourceMinLODClamp;
            break;

        case GPUResourceDimension::Texture2D:
            srvDesc.Texture2D.MostDetailedMip = viewInfo.mostDetailedMip;
            srvDesc.Texture2D.MipLevels = viewInfo.mipLevels;
            srvDesc.Texture2D.PlaneSlice = 0;
            srvDesc.Texture2D.ResourceMinLODClamp = viewInfo.resourceMinLODClamp;
            break;

        case GPUResourceDimension::Texture3D:
            srvDesc.Texture3D.MostDetailedMip = viewInfo.mostDetailedMip;
            srvDesc.Texture3D.MipLevels = viewInfo.mipLevels;
            srvDesc.Texture3D.ResourceMinLODClamp = viewInfo.resourceMinLODClamp;
            break;

        case GPUResourceDimension::Texture1DArray:
            srvDesc.Texture1DArray.MostDetailedMip = viewInfo.mostDetailedMip;
            srvDesc.Texture1DArray.MipLevels = viewInfo.mipLevels;
            srvDesc.Texture1DArray.FirstArraySlice = viewInfo.firstArraySlice;
            srvDesc.Texture1DArray.ArraySize = viewInfo.arraySize;
            srvDesc.Texture1DArray.ResourceMinLODClamp = viewInfo.resourceMinLODClamp;
            break;

        case GPUResourceDimension::Texture2DArray:
            srvDesc.Texture2DArray.MostDetailedMip = viewInfo.mostDetailedMip;
            srvDesc.Texture2DArray.MipLevels = viewInfo.mipLevels;
            srvDesc.Texture2DArray.FirstArraySlice = viewInfo.firstArraySlice;
            srvDesc.Texture2DArray.ArraySize = viewInfo.arraySize;
            srvDesc.Texture2DArray.PlaneSlice = 0;
            srvDesc.Texture2DArray.ResourceMinLODClamp = viewInfo.resourceMinLODClamp;
            break;
        }

        device->CreateShaderResourceView(mResource.Get(), &srvDesc, srvHandle);
    }

    void Texture::createUnorderedAccessView(Device& device, DescriptorHandle& uavHandle, const UnorderedAccessViewInfo& viewInfo) const
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
        uavDesc.Format = dx12::toDXGIFormat(mDescription.format);
        uavDesc.ViewDimension = dx12::toD3DUAVDimension(mDescription.dimension);

        switch (mDescription.dimension)
        {
        default:
            HASSERT(false, "Unhandled UAV creation path for resource dimension: %s", toString(mDescription.dimension).c_str());
            break;

        case GPUResourceDimension::Texture1D:
            uavDesc.Texture1D.MipSlice = viewInfo.mipSlice;
            break;

        case GPUResourceDimension::Texture2D:
            uavDesc.Texture2D.MipSlice = viewInfo.mipSlice;
            uavDesc.Texture2D.PlaneSlice = 0;
            break;

        case GPUResourceDimension::Texture3D:
            uavDesc.Texture3D.MipSlice = viewInfo.mipSlice;
            uavDesc.Texture3D.FirstWSlice = viewInfo.firstWSlice;
            uavDesc.Texture3D.WSize = viewInfo.wSize;
            break;

        case GPUResourceDimension::Texture1DArray:
            uavDesc.Texture1DArray.MipSlice = viewInfo.mipSlice;
            uavDesc.Texture1DArray.FirstArraySlice = viewInfo.firstArraySlice;
            uavDesc.Texture1DArray.ArraySize = viewInfo.arraySize;
            break;

        case GPUResourceDimension::Texture2DArray:
            uavDesc.Texture2DArray.MipSlice = viewInfo.mipSlice;
            uavDesc.Texture2DArray.FirstArraySlice = viewInfo.firstArraySlice;
            uavDesc.Texture2DArray.ArraySize = viewInfo.arraySize;
            uavDesc.Texture2DArray.PlaneSlice = 0;
            break;
        }

        device->CreateUnorderedAccessView(mResource.Get(), nullptr, &uavDesc, uavHandle);
    }

    void Texture::reset()
    {
        mResource.Reset();
    }
}

namespace graphics
{
    RenderTarget::RenderTarget(const Type type, Texture& texture)
        : mType(type), mTexture(texture)
    {
        const Texture::Description& textureDescription = mTexture.getDescription();
        bool isValidColorFlag = hasFlag(textureDescription.resourceFlags, GPUResourceFlags::AllowRenderTarget) && (mType == Type::Color);
        bool isValidDepthFlag = hasFlag(textureDescription.resourceFlags, GPUResourceFlags::AllowDepthStencil) && (mType == Type::Depth);

        HASSERT(isValidColorFlag || isValidDepthFlag, "Invalid GPUResourceFlag %s for render target.", toString(textureDescription.resourceFlags).c_str());
    }

    /*RenderTarget::RenderTarget(const Type type, const Texture::Description& createDescription)
        : mType(type), mTexture(createDescription)
    {
        const Texture::Description& textureDescription = mTexture.getDescription();
        bool isValidColorFlag = hasFlag(textureDescription.resourceFlags, GPUResourceFlags::AllowRenderTarget) && (mType == Type::Color);
        bool isValidDepthFlag = hasFlag(textureDescription.resourceFlags, GPUResourceFlags::AllowDepthStencil) && (mType == Type::Depth);

        HASSERT(isValidColorFlag || isValidDepthFlag, "Invalid GPUResourceFlag %s for render target.", toString(textureDescription.resourceFlags));
    }*/

    void RenderTarget::createFromSwapchain(Device& device, DescriptorHandle& rtvHandle, ID3D12Resource* baseResource)
    {
        HASSERT(mType == Type::Color, "Cannot create RTV for a Depth RenderTarget.");

        mTexture.setResource(baseResource);
        HASSERT(mTexture.get() != nullptr, "mTexture is null.");
        ID3D12Resource* res = mTexture.get();
        device->CreateRenderTargetView(res, nullptr, rtvHandle);
    }

    void RenderTarget::createRenderTargetView(Device& device, DescriptorHandle& rtvHandle, const uint32_t mipSlice, const uint32_t arraySlice)
    {
        HASSERT(mType == Type::Color, "Cannot create RTV for a Depth RenderTarget.");

        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
        rtvDesc.Format = dx12::toDXGIFormat(mTexture.getDescription().format);
        switch (mTexture.getDescription().dimension)
        {
        case GPUResourceDimension::Texture2D:
            rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            rtvDesc.Texture2D.MipSlice = mipSlice;
            break;

        case GPUResourceDimension::Texture2DArray:
            rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
            rtvDesc.Texture2DArray.MipSlice = mipSlice;
            rtvDesc.Texture2DArray.FirstArraySlice = arraySlice;
            rtvDesc.Texture2DArray.ArraySize = mTexture.getDescription().depth;
            break;

        default:
            HASSERT(false, "Unsupported resource dimension %s for RTV creation.", toString(mTexture.getDescription().dimension).c_str());
            return;
        }

        device->CreateRenderTargetView(mTexture.get(), &rtvDesc, rtvHandle);
    }

    void RenderTarget::createDepthStencilView(Device& device, DescriptorHandle& dsvHandle, const uint32_t mipSlice, const uint32_t arraySlice, const bool readOnlyDepth, const bool readOnlyStencil)
    {
        HASSERT(mType == Type::Depth, "Cannot create DSV for a Color RenderTarget.");

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
        dsvDesc.Format = dx12::toDXGIFormat(mTexture.getDescription().format);

        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
        if (readOnlyDepth)
        {
            dsvDesc.Flags |= D3D12_DSV_FLAG_READ_ONLY_DEPTH;
        }
        if (readOnlyStencil)
        {
            dsvDesc.Flags |= D3D12_DSV_FLAG_READ_ONLY_STENCIL;
        }

        switch (mTexture.getDescription().dimension)
        {
        case GPUResourceDimension::Texture2D:
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Texture2D.MipSlice = mipSlice;
            break;

        case GPUResourceDimension::Texture2DArray:
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
            dsvDesc.Texture2DArray.MipSlice = mipSlice;
            dsvDesc.Texture2DArray.FirstArraySlice = arraySlice;
            dsvDesc.Texture2DArray.ArraySize = mTexture.getDescription().depth;
            break;

        default:
            HASSERT(false, "Unsupported resource dimension %s for DSV creation.", toString(mTexture.getDescription().dimension).c_str());
            return;
        }

        device->CreateDepthStencilView(mTexture.get(), &dsvDesc, dsvHandle);
    }
}
