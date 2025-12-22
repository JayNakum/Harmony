#include "ShaderInput.h"

namespace graphics
{
    ResourceTable::ResourceTable(const Layout& layout, DescriptorHandle allocationStart)
        : mLayout(layout), mDescriptors(allocationStart), mDescriptorsCount(0)
    {
        for (const Range& range : mLayout.ranges)
        {
            HASSERT(range.count > 0, "Invalid range count for resource table.");
            mDescriptorsCount += range.count;
        }
    }

    void ResourceTable::setResource(Device& device, const DescriptorHandle& descriptor, const unsigned int row, const unsigned int column)
    {
        DescriptorHandle handle = mDescriptors + static_cast<INT>(getDescriptorOffsetAt(row, column) * device.getDescriptorHandleIncrementSize(DescriptorHeapType::ShaderViews));
        device->CopyDescriptorsSimple(1, handle, descriptor, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    /*
        const DescriptorHandle& ResourceTable::setResource(Device& device, const ByteAddressBuffer& buffer, const unsigned int row, const unsigned int column)
        {
            return setResource(device, buffer, buffer.getDescription().width, 0, row, column);
        }

        const DescriptorHandle& ResourceTable::setResource(Device& device, const ByteAddressBuffer& buffer, const uint64_t sizeInBytes, const unsigned int offsetInBytes, const unsigned int row, const unsigned int column)
        {
            DescriptorHandle& handle = mDescriptors + (getDescriptorOffsetAt(row, column) * device.getDescriptorHandleIncrementSize(DescriptorHeapType::ShaderViews));

            DescriptorRangeType type = mLayout.ranges.at(row).type;

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
                HASSERT(false, "Invalid DescriptorRangeType(%s) at row(%d) column(%d).", toString(type), row, column);
                break;
            }

            return handle;
        }

        const DescriptorHandle& ResourceTable::setResource(Device& device, const Texture& texture, const Texture::ShaderResourceViewInfo& viewInfo, const unsigned int row, const unsigned int column)
        {
            DescriptorHandle& handle = mDescriptors + (getDescriptorOffsetAt(row, column) * device.getDescriptorHandleIncrementSize(DescriptorHeapType::ShaderViews));

            DescriptorRangeType type = mLayout.ranges.at(row).type;

            HASSERT(type == DescriptorRangeType::SRV, "Invalid DescriptorRangeType(%s) at row(%d) column(%d).", toString(type), row, column);
            texture.createShaderResourceView(device, handle, viewInfo);

            return handle;
        }

        const DescriptorHandle& ResourceTable::setResource(Device& device, const Texture& texture, const Texture::UnorderedAccessViewInfo& viewInfo, const unsigned int row, const unsigned int column)
        {
            DescriptorHandle& handle = mDescriptors + (getDescriptorOffsetAt(row, column) * device.getDescriptorHandleIncrementSize(DescriptorHeapType::ShaderViews));

            DescriptorRangeType type = mLayout.ranges.at(row).type;

            HASSERT(type == DescriptorRangeType::UAV, "Invalid DescriptorRangeType(%s) at row(%d) column(%d).", toString(type), row, column);
            texture.createUnorderedAccessView(device, handle, viewInfo);

            return handle;
        }
    */

    void ResourceTable::copyDescriptorsFrom(Device& device, const ResourceTable& table)
    {
        HASSERT(mDescriptorsCount == table.getHandlesCount(), "Total descriptor count doesn't match ", mDescriptorsCount);
        device->CopyDescriptorsSimple(mDescriptorsCount, mDescriptors, table.getHandleStart(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    unsigned int ResourceTable::getDescriptorOffsetAt(const unsigned int row, const unsigned int column) const
    {
        unsigned int descriptorOffset = 0;
        for (unsigned int i = 0; i < row; ++i)
        {
            const Range& range = mLayout.ranges.at(i);
            descriptorOffset += range.count;
        }

        descriptorOffset += column;

        HASSERT(descriptorOffset <= mDescriptorsCount, "Row(", row, "), Column(", column, ") out of range.");

        return descriptorOffset;
    }
}

namespace graphics
{
    std::unordered_map<std::string, RootSignature> ShaderInput::msRootSignatures;

    ShaderInput::ShaderInput(const std::vector<Constants>& constantsLayout, const std::vector<ResourceTable::Layout>& gpuResourceTablesLayout)
        : mConstantsLayout(constantsLayout),
        mGPUResourceTablesLayout(gpuResourceTablesLayout)
    {
    }

    RootSignature& ShaderInput::getRootSignature(Device& device)
    {
        std::lock_guard<std::mutex> scopedLockGuard(mRootSignatureMutex);

        const std::string rootSignatureID = createRootSignatureID();

        if (msRootSignatures.find(rootSignatureID) != msRootSignatures.end())
        {
            return msRootSignatures.at(rootSignatureID);
        }

        RootSignature::Description createDescription{};
        createDescription.name = rootSignatureID;

        // Auto-assignment counters per descriptor type for shaderRegister
        unsigned int nextCBV = 0;
        unsigned int nextSRV = 0;
        unsigned int nextUAV = 0;
        unsigned int nextSampler = 0;

        for (const Constants& constants : mConstantsLayout)
        {
            createDescription.rootParameters.emplace_back();
            RootParameter& rootParameter = createDescription.rootParameters.back();
            rootParameter.initAsConstants(nextCBV++, constants.count, constants.shaderVisibility);

            RootConstants param(constants.count);
            mRootConstants.push_back(param);
        }

        for (const ResourceTable::Layout& parameter : mGPUResourceTablesLayout)
        {
            createDescription.rootParameters.emplace_back();
            RootParameter& rootParameter = createDescription.rootParameters.back();

            rootParameter.initAsDescriptorTable(parameter.ranges.size(), parameter.shaderVisibility);
            for (unsigned int rangeIndex = 0; rangeIndex < parameter.ranges.size(); ++rangeIndex)
            {
                const ResourceTable::Range& range = parameter.ranges[rangeIndex];

                HASSERT(range.count > 0, "Trying to set an empty descriptor range.");

                unsigned int shaderRegister = 0;
                switch (range.type)
                {
                default:
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
            }
        }

        createDescription.staticSamplers = getStaticSamplers();

        msRootSignatures.emplace(rootSignatureID, RootSignature(device, createDescription));

        return msRootSignatures.at(rootSignatureID);
    }

    void ShaderInput::setConstants(const RootConstants values, const unsigned int layoutIndex)
    {
        mRootConstants.at(layoutIndex) = values;
    }

    void ShaderInput::updateResourceTable(Device& device, const ResourceTable& resourceTable, const unsigned int layoutIndex)
    {
        mGPUResourceTables.at(layoutIndex).copyDescriptorsFrom(device, resourceTable);
    }

    void ShaderInput::finalize(GraphicContext& context)
    {
        unsigned int rootIndex = 0;

        // Finalize root constants
        HASSERT(mConstantsLayout.size() == mRootConstants.size(), "Mismatch in layout size and root constants size.");

        while (rootIndex < mRootConstants.size())
        {
            RootConstants& constants = mRootConstants.at(rootIndex);
            context.setConstants(rootIndex++, constants.size(), constants.data());
        }

        // Finalize descriptor tables
        HASSERT(mGPUResourceTablesLayout.size() == mGPUResourceTables.size(), "Mismatch in layout size and root parameters size.");
        for (const ResourceTable& table : mGPUResourceTables)
        {
            context.setDescriptorTable(rootIndex++, table.getHandleStart());
        }
    }

    void ShaderInput::finalize(ComputeContext& context)
    {
        unsigned int rootIndex = 0;

        // Finalize root constants
        HASSERT(mConstantsLayout.size() == mRootConstants.size(), "Mismatch in layout size and root constants size.");

        while (rootIndex < mRootConstants.size())
        {
            RootConstants& constants = mRootConstants.at(rootIndex);
            context.setConstants(rootIndex++, constants.size(), constants.data());
        }

        // Finalize descriptor tables
        HASSERT(mGPUResourceTablesLayout.size() == mGPUResourceTables.size(), "Mismatch in layout size and root parameters size.");
        for (const ResourceTable& table : mGPUResourceTables)
        {
            context.setDescriptorTable(rootIndex++, table.getHandleStart());
        }
    }

    std::string ShaderInput::createRootSignatureID()
    {
        std::string rootSignatureID = "";

        for (const Constants& constant : mConstantsLayout)
        {
            rootSignatureID += toString(constant.shaderVisibility);
            rootSignatureID += std::to_string(constant.count);
        }

        for (const ResourceTable::Layout& parameter : mGPUResourceTablesLayout)
        {
            rootSignatureID += toString(parameter.shaderVisibility);
            for (const ResourceTable::Range& range : parameter.ranges)
            {
                rootSignatureID += toString(range.type);
                rootSignatureID += std::to_string(range.count);
            }
        }
        return rootSignatureID;
    }

    const std::array<StaticSampler, 6> ShaderInput::getStaticSamplers()
    {
        static StaticSampler::Description samplerDescription{};

        samplerDescription.filter = sampler::Filter::MinMagMipPoint;
        samplerDescription.addressU = sampler::TextureAddressMode::Wrap;
        static const StaticSampler pointWrap(0, samplerDescription);

        samplerDescription.filter = sampler::Filter::MinMagMipPoint;
        samplerDescription.addressU = sampler::TextureAddressMode::Clamp;
        static const StaticSampler pointClamp(1, samplerDescription);

        samplerDescription.filter = sampler::Filter::MinMagMipLinear;
        samplerDescription.addressU = sampler::TextureAddressMode::Wrap;
        static const StaticSampler linearWrap(2, samplerDescription);

        samplerDescription.filter = sampler::Filter::MinMagMipLinear;
        samplerDescription.addressU = sampler::TextureAddressMode::Clamp;
        static const StaticSampler linearClamp(3, samplerDescription);

        samplerDescription.filter = sampler::Filter::Anisotropic;
        samplerDescription.addressU = sampler::TextureAddressMode::Wrap;
        samplerDescription.maxAnisotropy = 8;
        static const StaticSampler anisotropicWrap(4, samplerDescription);

        samplerDescription.filter = sampler::Filter::Anisotropic;
        samplerDescription.addressU = sampler::TextureAddressMode::Clamp;
        samplerDescription.maxAnisotropy = 8;
        static const StaticSampler anisotropicClamp(5, samplerDescription);

        return {
            pointWrap, pointClamp,
            linearWrap, linearClamp,
            anisotropicWrap, anisotropicClamp
        };
    }
}
