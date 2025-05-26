#pragma once

#include "common.h"

#include "DynamicDescriptorHeap.h"

#include "CommandContext.h"

#include "dx12/RootSignature.h"
#include "dx12/GPUBuffer.h"
#include "dx12/GPUTexture.h"

namespace graphics
{
    /*
    * A management system for shader inputs.
    * The class handles multiple things at a time.
    * - Creation and binding of root signature
    * - Allocation of descriptors.
    * - Management and binding of dynamic descriptor heap.
    *
    * Everything is initialized form a single shader input layout passed in constructor.
    */
    class ShaderInput
    {
    public:
        struct Constants
        {
            const ShaderVisibility shaderVisibility;
            const unsigned int count = 1;
        };

        // Descriptor Table Entry
        struct Range
        {
            const DescriptorRangeType type;
            const unsigned int count = 1;
        };

        // Descriptor Table
        struct Parameter
        {
            const ShaderVisibility shaderVisibility;
            const std::vector<Range> ranges;
        };

        // Scoped shader inputs.
        // 32bit constants are expected at the top of root parameters
        // Layout is expected in order of shader registers.
        ShaderInput(GraphicContext& context, const std::vector<Constants>& constantsLayout, const std::vector<Parameter>& parametersLayout);

        const dx12::RootSignature& getRootSignature() const { return mRootSignature; }

        const void setConstant(const dx12::DWParam value, const unsigned int layoutIndex, const unsigned int parameterIndex = 0);

        // Set a CPU descriptor handle at the specified layoutIndex, parameterIndex and rangeIndex.
        const dx12::DescriptorHandle setResource(const dx12::DescriptorHandle& cpuHandle,
            const unsigned int layoutIndex, const unsigned int parameterIndex = 0, const unsigned int rangeIndex = 0);

        // Set a buffer at the specified layoutIndex, parameterIndex and rangeIndex.
        const dx12::DescriptorHandle setResource(const dx12::ByteAddressBuffer& buffer, const unsigned int layoutIndex, const unsigned int parameterIndex = 0, const unsigned int rangeIndex = 0);
        const dx12::DescriptorHandle setResource(const dx12::ByteAddressBuffer& buffer, const uint64_t sizeInBytes, const unsigned int offsetInBytes,
            const unsigned int layoutIndex, const unsigned int parameterIndex = 0, const unsigned int rangeIndex = 0);

        // Set a texture at the specified layoutIndex, parameterIndex and rangeIndex.
        const dx12::DescriptorHandle setResource(const dx12::GPUTexture& texture, const dx12::GPUTexture::ShaderResourceViewInfo& viewInfo,
            const unsigned int layoutIndex, const unsigned int parameterIndex = 0, const unsigned int rangeIndex = 0);
        const dx12::DescriptorHandle setResource(const dx12::GPUTexture& texture, const dx12::GPUTexture::UnorderedAccessViewInfo& viewInfo,
                                                 const unsigned int layoutIndex, const unsigned int parameterIndex = 0, const unsigned int rangeIndex = 0);
        // Copies descriptors from CPU to GPU and sets descriptor heaps.
        void finalize();

        ~ShaderInput() = default;

        static std::unordered_map<std::string, dx12::RootSignature> msRootSignatures;
    private:
        static const std::array<dx12::StaticSampler, 6> getStaticSamplers();

        std::string createRootSignatureID();

        // Creates the root signature and allocated descriptors in CPU heap.
        dx12::RootSignature& initialize(const std::string& rootSignatureID);

        const unsigned int getDescriptorTableOffsetAt(const unsigned int layoutIndex);
        const unsigned int getDescriptorRangeOffsetAt(const unsigned int layoutIndex, const unsigned int parameterIndex);
        dx12::DescriptorHandle& getDescriptorHandleAt(const unsigned int layoutIndex, const unsigned int parameterIndex, const unsigned int rangeIndex);

    private:
        GraphicContext& mContext;

        dx12::DescriptorHandle mDescriptors;
        unsigned int mDescriptorsCount;
        DescriptorHeapWithAllocator::Page& mCPUDescriptorHeapPage;

        const std::vector<Constants>& mConstantsLayout;
        const std::vector<Parameter>& mParametersLayout;

        dx12::RootSignature& mRootSignature;

        using RootConstants = std::vector<dx12::DWParam>;
        std::vector<RootConstants> mRootConstants;
    };
}
