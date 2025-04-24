#pragma once

#include "common.h"

#include "DynamicDescriptorHeap.h"

#include "CommandContext.h"

#include "dx12/RootSignature.h"
#include "dx12/GPUBuffer.h"
#include "dx12/GPUTexture.h"

namespace graphics
{
    class ShaderInput
    {
    public:
        struct Range
        {
            const DescriptorRangeType type;
            const unsigned int count = 1;
        };

        struct Parameter
        {
            const ShaderVisibility shaderVisibility;
            std::vector<Range> ranges;
        };

        // Scoped shader inputs for management of root signature and descriptors.
        // Allocates a CPUDescriptorHeapPage. Creates and sets a root signature. Allocates necessary descriptors in the page.
        ShaderInput(GraphicContext& context, const std::vector<Parameter>& layout, const RootSignatureFlags flags = RootSignatureFlags::AllowInputAssemblerInputLayout);

        const dx12::DescriptorHandle setResource(const dx12::ByteAddressBuffer& buffer, const unsigned int layoutIndex, const unsigned int parameterIndex = 0, const unsigned int rangeIndex = 0);
        const dx12::DescriptorHandle setResource(const dx12::GPUTexture& texture, const unsigned int layoutIndex, const unsigned int parameterIndex = 0, const unsigned int rangeIndex = 0);

        // Copies descriptors from CPU to GPU and sets descriptor heaps.
        void finalize();

        ~ShaderInput() = default;

    private:
        static const std::array<dx12::StaticSampler, 6> getStaticSamplers();

        dx12::RootSignature initialize(const std::vector<Parameter>& layout, const RootSignatureFlags flags);

    private:
        GraphicContext& mContext;

        const std::vector<Parameter>& mLayout;

        DescriptorHeapWithAllocator::Page mCPUDescriptorHeapPage;
        DynamicDescriptorHeap& mShaderViewDescriptorHeap;

        struct ShaderDescriptorTableLayout // struct Range
        {
            dx12::DescriptorHandle descriptorRange;
            unsigned int count = 1;
        };

        struct ShaderDescriptorLayout // struct Parameter
        {
            std::vector<ShaderDescriptorTableLayout> ranges;
        };

        std::vector<ShaderDescriptorLayout> mDescriptors;
    };
}
