#include "ShaderInput.h"

namespace graphics
{
    ShaderInput::ShaderInput(GraphicContext& context, const std::vector<Constants>& constantsLayout, const std::vector<Parameter>& parametersLayout)
        : mContext(context),
        mConstantsLayout(constantsLayout), mParametersLayout(parametersLayout),
        mCPUDescriptorHeapPage(context.getShaderViewHeapPage()),
        mRootSignature(initialize(mContext.getName()))
    {
        mContext.setRootSignature(mRootSignature);
    }

    const void ShaderInput::setConstant(const dx12::DWParam value, const unsigned int layoutIndex, const unsigned int parameterIndex)
    {
        mRootConstants.at(layoutIndex).at(parameterIndex) = value;
    }

    const dx12::DescriptorHandle ShaderInput::setResource(const dx12::DescriptorHandle& cpuHandle, const unsigned int layoutIndex, const unsigned int parameterIndex, const unsigned int rangeIndex)
    {
        mDescriptors.at(layoutIndex).ranges.at(parameterIndex).descriptorRange = cpuHandle;
        return cpuHandle;
    }

    const dx12::DescriptorHandle ShaderInput::setResource(const dx12::ByteAddressBuffer& buffer, const unsigned int layoutIndex, const unsigned int parameterIndex, const unsigned int rangeIndex)
    {
        return setResource(buffer, buffer.describeResource().width, 0, layoutIndex, parameterIndex, rangeIndex);
    }

    const dx12::DescriptorHandle ShaderInput::setResource(const dx12::ByteAddressBuffer& buffer, const uint64_t sizeInBytes, const unsigned int offsetInBytes, const unsigned int layoutIndex, const unsigned int parameterIndex, const unsigned int rangeIndex)
    {
        dx12::DescriptorHandle handle = mDescriptors.at(layoutIndex).ranges.at(parameterIndex).descriptorRange;

        const DescriptorRangeType type = mParametersLayout.at(layoutIndex).ranges.at(parameterIndex).type;

        dx12::Device& device = mContext.getDevice();

        switch (type)
        {
        case DescriptorRangeType::CBV:
            buffer.createConstantBufferView(device, handle, sizeInBytes, offsetInBytes);
            break;
        case DescriptorRangeType::SRV:
            buffer.createShaderResourceView(device, handle, sizeInBytes, offsetInBytes);
            break;
        case DescriptorRangeType::UAV:
            buffer.createUnorderedAccessView(device, handle, sizeInBytes, offsetInBytes);
            break;
        default:
            HASSERT(false, "Invalid DescriptorRangeType(%s) at layoutIndex(%d) paramterIndex(%d) rangeIndex(%d).", toString(type), layoutIndex, parameterIndex, rangeIndex);
            break;
        }

        return handle;
    }


    const dx12::DescriptorHandle ShaderInput::setResource(const dx12::GPUTexture& texture, const dx12::GPUTexture::ShaderResourceViewInfo& viewInfo, const unsigned int layoutIndex, const unsigned int parameterIndex, const unsigned int rangeIndex)
    {
        dx12::DescriptorHandle handle = mDescriptors.at(layoutIndex).ranges.at(parameterIndex).descriptorRange;

        const DescriptorRangeType type = mParametersLayout.at(layoutIndex).ranges.at(parameterIndex).type;
        HASSERT(type == DescriptorRangeType::SRV, "Invalid DescriptorRangeType(%s) at layoutIndex(%d) paramterIndex(%d) rangeIndex(%d).", toString(type), layoutIndex, parameterIndex, rangeIndex);

        texture.createShaderResourceView(mContext.getDevice(), handle, viewInfo);

        return handle;
    }

    const dx12::DescriptorHandle ShaderInput::setResource(const dx12::GPUTexture& texture, const dx12::GPUTexture::UnorderedAccessViewInfo& viewInfo, const unsigned int layoutIndex, const unsigned int parameterIndex, const unsigned int rangeIndex)
    {
        dx12::DescriptorHandle handle = mDescriptors.at(layoutIndex).ranges.at(parameterIndex).descriptorRange;

        const DescriptorRangeType type = mParametersLayout.at(layoutIndex).ranges.at(parameterIndex).type;
        HASSERT(type == DescriptorRangeType::UAV, "Invalid DescriptorRangeType(%s) at layoutIndex(%d) paramterIndex(%d) rangeIndex(%d).", toString(type), layoutIndex, parameterIndex, rangeIndex);

        texture.createUnorderedAccessView(mContext.getDevice(), handle, viewInfo);

        return handle;
    }

    void ShaderInput::finalize()
    {
        unsigned int rootIndex = 0;

        {// Finalize root constants
            HASSERT(mConstantsLayout.size() == mRootConstants.size(), "Mismatch in layout size (%d) and root constants size (%s).", mConstantsLayout.size(), mRootConstants.size());

            while (rootIndex < mRootConstants.size())
            {
                RootConstants& constants = mRootConstants.at(rootIndex);
                mContext.commandList()->SetComputeRoot32BitConstants(rootIndex++, constants.size(), constants.data(), 0);
            }
        }
        {// Finalize descriptor tables
            HASSERT(mParametersLayout.size() == mDescriptors.size(), "Mismatch in layout size (%d) and descriptors size (%s).", mParametersLayout.size(), mDescriptors.size());

            const dx12::DescriptorHandle gpuStartHandle = mContext.copyDescriptorsToGPU();
            while (rootIndex < mDescriptors.size())
            {
                ShaderDescriptorLayout& descriptors = mDescriptors.at(rootIndex);
                ShaderDescriptorTableLayout& tableLayout = descriptors.ranges.at(0);

                uint32_t cpuOffset = mCPUDescriptorHeapPage.getByteOffsetOfHandle(tableLayout.descriptorRange);
                dx12::DescriptorHandle gpuHandle = gpuStartHandle + cpuOffset;
                mContext.commandList()->SetGraphicsRootDescriptorTable(rootIndex++, gpuHandle);
            }

            mContext.bindDescriptorHeaps();
        }
    }

    const std::array<dx12::StaticSampler, 6> ShaderInput::getStaticSamplers()
    {
        static const dx12::StaticSampler pointWrap(0, {
            sampler::Filter::MinMagMipPoint,
            sampler::TextureAddressMode::Wrap,
            sampler::TextureAddressMode::Wrap,
            sampler::TextureAddressMode::Wrap
            });

        static const dx12::StaticSampler pointClamp(1, {
            sampler::Filter::MinMagMipPoint,
            sampler::TextureAddressMode::Clamp,
            sampler::TextureAddressMode::Clamp,
            sampler::TextureAddressMode::Clamp
            });

        static const dx12::StaticSampler linearWrap(2, {
            sampler::Filter::MinMagMipLinear,
            sampler::TextureAddressMode::Wrap,
            sampler::TextureAddressMode::Wrap,
            sampler::TextureAddressMode::Wrap
            });

        static const dx12::StaticSampler linearClamp(3, {
            sampler::Filter::MinMagMipLinear,
            sampler::TextureAddressMode::Clamp,
            sampler::TextureAddressMode::Clamp,
            sampler::TextureAddressMode::Clamp
            });

        static const dx12::StaticSampler anisotropicWrap(4, {
            sampler::Filter::Anisotropic,
            sampler::TextureAddressMode::Wrap,
            sampler::TextureAddressMode::Wrap,
            sampler::TextureAddressMode::Wrap,
            0.0f, 8
            });

        static const dx12::StaticSampler anisotropicClamp(5, {
            sampler::Filter::Anisotropic,
            sampler::TextureAddressMode::Clamp,
            sampler::TextureAddressMode::Clamp,
            sampler::TextureAddressMode::Clamp,
            0.0f, 8
            });

        return {
            pointWrap, pointClamp,
            linearWrap, linearClamp,
            anisotropicWrap, anisotropicClamp
        };
    }

    dx12::RootSignature ShaderInput::initialize(const std::string& name)
    {
        // TODO: validate layout using reflections
        dx12::Device& device = mContext.getDevice();

        dx12::RootSignature::Description createDescription;
        createDescription.name = name + " root signature";

        unsigned int nextConstant32 = 0;
        for (const Constants& constants : mConstantsLayout)
        {
            dx12::RootParameter rootParameter;
            rootParameter.initAsConstants(nextConstant32++, constants.count, constants.shaderVisibility);
            createDescription.rootParameters.emplace_back(rootParameter);

            // Root constant creation
            RootConstants rootConsts;
            rootConsts.reserve(constants.count);
            mRootConstants.emplace_back(rootConsts);
        }

        // Auto-assignment counters per descriptor type for shaderRegister
        unsigned int nextCBV = 0;
        unsigned int nextSRV = 0;
        unsigned int nextUAV = 0;
        unsigned int nextSampler = 0;

        for (const Parameter& parameter : mParametersLayout)
        {
            HASSERT(parameter.ranges.size() > 0, "No descriptor ranges found.");

            dx12::RootParameter rootParameter;

            ShaderDescriptorLayout descriptors; // Descriptor Allocation

            rootParameter.initAsDescriptorTable(parameter.ranges.size(), parameter.shaderVisibility);
            for (unsigned int rangeIndex = 0; rangeIndex < parameter.ranges.size(); ++rangeIndex)
            {
                const Range& range = parameter.ranges[rangeIndex];

                unsigned int shaderRegister = 0;
                switch (range.type)
                {
                case DescriptorRangeType::CBV:
                    shaderRegister = nextCBV++;
                    break;
                case DescriptorRangeType::SRV:
                    shaderRegister = nextSRV++;
                    break;
                case DescriptorRangeType::UAV:
                    shaderRegister = nextUAV++;
                    break;
                case DescriptorRangeType::Sampler:
                    shaderRegister = nextSampler++;
                    break;
                }

                rootParameter.setTableRange(rangeIndex, range.type, shaderRegister, range.count);

                // Descriptor Allocation
                ShaderDescriptorTableLayout tableLayout;
                tableLayout.count = range.count;
                tableLayout.descriptorRange = mCPUDescriptorHeapPage.allocate(tableLayout.count);
                descriptors.ranges.push_back(tableLayout);
            }

            createDescription.rootParameters.emplace_back(rootParameter);

            mDescriptors.push_back(descriptors); // Descriptor Allocation
        }

        createDescription.staticSamplers = getStaticSamplers();

        return dx12::RootSignature(device, createDescription);
    }
}
