#include "ShaderInput.h"

namespace graphics
{
    std::unordered_map<std::string, dx12::RootSignature> ShaderInput::msRootSignatures;

    ShaderInput::ShaderInput(GraphicContext& context, const std::vector<Constants>& constantsLayout, const std::vector<Parameter>& parametersLayout)
        : mContext(context),
        mDescriptors(),
        mDescriptorsCount(0),
        mCPUDescriptorHeapPage(context.getShaderViewHeapPage()),
        mConstantsLayout(constantsLayout),
        mParametersLayout(parametersLayout),
        mRootSignature(initialize(createRootSignatureID()))
    {
        mContext.setRootSignature(mRootSignature);
    }

    const void ShaderInput::setConstant(const dx12::DWParam value, const unsigned int layoutIndex, const unsigned int parameterIndex)
    {
        mRootConstants.at(layoutIndex).at(parameterIndex) = value;
    }

    const dx12::DescriptorHandle ShaderInput::setResource(const dx12::DescriptorHandle& cpuHandle, const unsigned int layoutIndex, const unsigned int parameterIndex, const unsigned int rangeIndex)
    {
        dx12::DescriptorHandle handle = getDescriptorHandleAt(layoutIndex, parameterIndex, rangeIndex);

        mContext.copyDescriptorsSimple(cpuHandle, handle);

        return handle;
    }

    const dx12::DescriptorHandle ShaderInput::setResource(const dx12::ByteAddressBuffer& buffer, const unsigned int layoutIndex, const unsigned int parameterIndex, const unsigned int rangeIndex)
    {
        return setResource(buffer, buffer.describeResource().width, 0, layoutIndex, parameterIndex, rangeIndex);
    }

    const dx12::DescriptorHandle ShaderInput::setResource(const dx12::ByteAddressBuffer& buffer, const uint64_t sizeInBytes, const unsigned int offsetInBytes, const unsigned int layoutIndex, const unsigned int parameterIndex, const unsigned int rangeIndex)
    {
        dx12::DescriptorHandle handle = getDescriptorHandleAt(layoutIndex, parameterIndex, rangeIndex);

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
        dx12::DescriptorHandle handle = getDescriptorHandleAt(layoutIndex, parameterIndex, rangeIndex);

        const DescriptorRangeType type = mParametersLayout.at(layoutIndex).ranges.at(parameterIndex).type;
        HASSERT(type == DescriptorRangeType::SRV, "Invalid DescriptorRangeType(%s) at layoutIndex(%d) paramterIndex(%d) rangeIndex(%d).", toString(type), layoutIndex, parameterIndex, rangeIndex);

        texture.createShaderResourceView(mContext.getDevice(), handle, viewInfo);

        return handle;
    }

    const dx12::DescriptorHandle ShaderInput::setResource(const dx12::GPUTexture& texture, const dx12::GPUTexture::UnorderedAccessViewInfo& viewInfo, const unsigned int layoutIndex, const unsigned int parameterIndex, const unsigned int rangeIndex)
    {
        dx12::DescriptorHandle handle = getDescriptorHandleAt(layoutIndex, parameterIndex, rangeIndex);

        const DescriptorRangeType type = mParametersLayout.at(layoutIndex).ranges.at(parameterIndex).type;
        HASSERT(type == DescriptorRangeType::UAV, "Invalid DescriptorRangeType(%s) at layoutIndex(%d) paramterIndex(%d) rangeIndex(%d).", toString(type), layoutIndex, parameterIndex, rangeIndex);

        texture.createUnorderedAccessView(mContext.getDevice(), handle, viewInfo);

        return handle;
    }

    void ShaderInput::finalize()
    {
        mContext.bindDescriptorHeaps();

        unsigned int rootIndex = 0;

        // Finalize root constants
        HASSERT(mConstantsLayout.size() == mRootConstants.size(), "Mismatch in layout size (%d) and root constants size (%s).", mConstantsLayout.size(), mRootConstants.size());

        while (rootIndex < mRootConstants.size())
        {
            RootConstants& constants = mRootConstants.at(rootIndex);
            mContext.commandList()->SetComputeRoot32BitConstants(rootIndex++, (UINT)constants.size(), constants.data(), 0);
        }

        // Finalize descriptor tables
        const dx12::DescriptorHandle gpuStartHandle = mContext.copyDescriptorsToGPU(mDescriptors, mDescriptorsCount);

        unsigned int descriptorOffset = 0;

        for (unsigned int i = 0; i < mParametersLayout.size(); ++i)
        {
            // Calculate the GPU Handle
            // Since the all the descriptors are allocated in a single allocation, they are expected to be linear.
            // And since the copyToGPU happens on all the descriptors at once, it is safe to assume that the GPU Heap will have all the descriptors linearly placed.
            // Meaning that we start with offset = 0, and then we add the number of descriptors per table to the offset.
            descriptorOffset += getDescriptorTableOffsetAt(i);

            dx12::DescriptorHandle gpuHandle = gpuStartHandle + (descriptorOffset * mCPUDescriptorHeapPage.getDescriptorSize());

            mContext.commandList()->SetGraphicsRootDescriptorTable(rootIndex++, gpuHandle.getGPUHandle());
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

    std::string ShaderInput::createRootSignatureID()
    {
        std::string rootSignatureID = "";

        for (const Constants& constants : mConstantsLayout)
        {
            rootSignatureID += toString(constants.shaderVisibility);
            rootSignatureID += std::to_string(constants.count);
        }

        for (const Parameter& parameter : mParametersLayout)
        {
            rootSignatureID += toString(parameter.shaderVisibility);
            for (const Range& range : parameter.ranges)
            {
                rootSignatureID += toString(range.type);
                rootSignatureID += std::to_string(range.count);
            }
        }

        return rootSignatureID;
    }

    dx12::RootSignature& ShaderInput::initialize(const std::string& rootSignatureID)
    {
        dx12::Device& device = mContext.getDevice();

        dx12::RootSignature::Description createDescription;
        createDescription.name = rootSignatureID;

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

            createDescription.rootParameters.emplace_back();
            dx12::RootParameter& rootParameter = createDescription.rootParameters.back();

            rootParameter.initAsDescriptorTable(parameter.ranges.size(), parameter.shaderVisibility);
            for (unsigned int rangeIndex = 0; rangeIndex < parameter.ranges.size(); ++rangeIndex)
            {
                const Range& range = parameter.ranges[rangeIndex];

                HASSERT(range.count > 0, "Trying to set an empty descriptor range.");

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

                mDescriptorsCount += range.count; // Descriptor Allocation
            }
        }
        mDescriptors = mCPUDescriptorHeapPage.allocate(mDescriptorsCount); // Descriptor Allocation

        createDescription.staticSamplers = getStaticSamplers();

        if (msRootSignatures.find(rootSignatureID) == msRootSignatures.end())
        {
            msRootSignatures.emplace(rootSignatureID, dx12::RootSignature(device, createDescription));
        }

        return msRootSignatures.at(rootSignatureID);
    }

    const unsigned int ShaderInput::getDescriptorTableOffsetAt(const unsigned int layoutIndex)
    {
        unsigned int tableOffset = 0;

        for (unsigned int i = 0; i < layoutIndex; ++i)
        {
            const Parameter& parameter = mParametersLayout.at(i);
            for (const Range& range : parameter.ranges)
            {
                tableOffset += range.count;
            }
        }

        return tableOffset;
    }

    const unsigned int ShaderInput::getDescriptorRangeOffsetAt(const unsigned int layoutIndex, const unsigned int parameterIndex)
    {
        const unsigned int tableOffset = getDescriptorTableOffsetAt(layoutIndex);

        unsigned int rangeOffset = 0;
        for (unsigned int i = 0; i < parameterIndex; ++i)
        {
            const Range& range = mParametersLayout.at(layoutIndex).ranges.at(i);
            rangeOffset += range.count;
        }

        return tableOffset + rangeOffset;
    }

    dx12::DescriptorHandle ShaderInput::getDescriptorHandleAt(const unsigned int layoutIndex, const unsigned int parameterIndex, const unsigned int rangeIndex)
    {
        const unsigned int rangeOffset = getDescriptorRangeOffsetAt(layoutIndex, parameterIndex);

        dx12::DescriptorHandle handle = mDescriptors;

        handle += ((rangeOffset + rangeIndex) * (unsigned int)mCPUDescriptorHeapPage.getDescriptorSize());

        return handle;
    }
}
