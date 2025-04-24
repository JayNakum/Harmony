#include "ShaderInput.h"

namespace graphics
{
    ShaderInput::ShaderInput(GraphicContext& context, const std::vector<Parameter>& layout, RootSignatureFlags flags)
        : mContext(context), mLayout(layout), mShaderViewDescriptorHeap(context.getShaderViewsHeap()), mCPUDescriptorHeapPage(mShaderViewDescriptorHeap.allocatePage())
    {
        dx12::RootSignature rootSignature = initialize(layout, flags);
        mContext.setRootSignature(rootSignature);
    }

    const dx12::DescriptorHandle ShaderInput::setResource(const dx12::ByteAddressBuffer& buffer, const unsigned int layoutIndex, const unsigned int parameterIndex, const unsigned int rangeIndex)
    {
        return setResource(buffer, buffer.describeResource().width, 0, layoutIndex, parameterIndex, rangeIndex);
    }

    const dx12::DescriptorHandle ShaderInput::setResource(const dx12::ByteAddressBuffer& buffer, const uint64_t sizeInBytes, const unsigned int offsetInBytes, const unsigned int layoutIndex, const unsigned int parameterIndex, const unsigned int rangeIndex)
    {
		dx12::DescriptorHandle handle = mDescriptors.at(layoutIndex).ranges.at(parameterIndex).descriptorRange;

		const DescriptorRangeType type = mLayout.at(layoutIndex).ranges.at(parameterIndex).type;

		dx12::Device& device = mContext.getDevice();

		switch (type)
		{
		case DescriptorRangeType::CBV:
			buffer.createConstantBufferView(device, handle, sizeInBytes, offsetInBytes);
			break;
		case DescriptorRangeType::SRV:
			buffer.createShaderResourceView(device, handle, sizeInBytes, offsetInBytes);
			break;
		default:
			HASSERT(false, "Invalid DescriptorRangeType at layoutIndex(%d) paramterIndex(%d) rangeIndex(%d).", layoutIndex, parameterIndex, rangeIndex);
			break;
		}

		return handle;
    }

    const dx12::DescriptorHandle ShaderInput::setResource(const dx12::GPUTexture& texture, const unsigned int layoutIndex, const unsigned int parameterIndex, const unsigned int rangeIndex)
    {
        return setResource(texture, dx12::GPUTexture::ShaderResourceViewInfo{}, layoutIndex, parameterIndex, rangeIndex);
    }

    const dx12::DescriptorHandle ShaderInput::setResource(const dx12::GPUTexture& texture, const dx12::GPUTexture::ShaderResourceViewInfo& viewInfo, const unsigned int layoutIndex, const unsigned int parameterIndex, const unsigned int rangeIndex)
    {
		dx12::DescriptorHandle handle = mDescriptors.at(layoutIndex).ranges.at(parameterIndex).descriptorRange;

		const DescriptorRangeType type = mLayout.at(layoutIndex).ranges.at(parameterIndex).type;

		dx12::Device& device = mContext.getDevice();

		switch (type)
		{
		case DescriptorRangeType::SRV:
			texture.createShaderResourceView(device, handle, viewInfo);
			break;

		default:
			HASSERT(false, "Invalid DescriptorRangeType at layoutIndex(%d) paramterIndex(%d) rangeIndex(%d).", layoutIndex, parameterIndex, rangeIndex);
			break;
		}

		return handle;
    }

    void ShaderInput::finalize()
    {
        const dx12::DescriptorHandle gpuStartHandle = mShaderViewDescriptorHeap.finalize(mCPUDescriptorHeapPage);

        HASSERT(mLayout.size() == mDescriptors.size(), "Mismatch in layout size (%d) and descriptors size (%s).", mLayout.size(), mDescriptors.size());

        for (unsigned int rootIndex = 0; rootIndex < mDescriptors.size(); ++rootIndex)
        {
            ShaderDescriptorLayout& descriptors = mDescriptors.at(rootIndex);
            ShaderDescriptorTableLayout& tableLayout = descriptors.ranges.at(0);

			uint32_t cpuOffset = mCPUDescriptorHeapPage.getByteOffsetOfHandle(tableLayout.descriptorRange);
			dx12::DescriptorHandle gpuHandle = gpuStartHandle + cpuOffset;
			mContext.commandList()->SetGraphicsRootDescriptorTable(rootIndex, gpuHandle);
        }

        mContext.bindDescriptorHeaps();
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

    dx12::RootSignature ShaderInput::initialize(const std::vector<Parameter>& layout, const RootSignatureFlags flags)
    {
		// TODO: validate layout using reflections
        dx12::Device& device = mContext.getDevice();

        dx12::RootSignature::Description createDescription;
        createDescription.name = mContext.getName() + " root signature";
        createDescription.flags = flags;

        // Auto-assignment counters per descriptor type
        unsigned int nextCBV = 0;
        unsigned int nextSRV = 0;
        unsigned int nextUAV = 0;
        unsigned int nextSampler = 0;

        for (unsigned int rootIndex = 0; rootIndex < layout.size(); ++rootIndex)
        {
            const Parameter& parameter = layout[rootIndex];
            HASSERT(parameter.ranges.size() > 0, "No descriptor ranges for root index %d found.", rootIndex);

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
