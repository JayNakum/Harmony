#pragma once

#include "common.h"

#include "DynamicDescriptorHeap.h"

#include "CommandContext.h"

#include "dx12/RootSignature.h"
#include "dx12/GPUBuffer.h"
#include "dx12/GPUTexture.h"

namespace engine
{
    class ShaderInput
    {
    public:
        enum class ParameterType
        {
            Constant32,
            CBV,
            SRV,
            UAV
        };

        struct Parameters
        {
            const ParameterType type;
            const unsigned int count = 1;
        };

        ShaderInput(const std::vector<Parameters>& layout, graphics::DynamicDescriptorHeap& shaderViewDescriptorHeap, graphics::GraphicContext& context);

        void setResource(const graphics::dx12::GPUBuffer& buffer, const unsigned int layoutIndex, const unsigned int parameterIndex = 0);
        void setResource(const graphics::dx12::GPUTexture& texture, const unsigned int layoutIndex, const unsigned int parameterIndex = 0);

        void finalize();

        ~ShaderInput();

    private:
        graphics::dx12::RootSignature createRootSignature(const std::vector<Parameters>& layout);
        void stageGPUDescriptors(const unsigned int rootIndex, graphics::dx12::DescriptorHandle handle, const size_t count = 0);

    private:
        graphics::DescriptorHeapWithAllocator::Page mCPUDescriptorHeapPage;
        graphics::DynamicDescriptorHeap& mShaderViewDescriptorHeap;
        graphics::GraphicContext& mContext;
    };
}
