#pragma once

#include "common.h"
#include "directx.h"

#include "PipelineStateEnums.h"

#include "RootSignature.h"

#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        /*
        * Wrapper over ID3D12PipelineState
        * This is the parent class for the graphics and compute pipeline states
        */
        class PipelineState
        {
        protected:
            PipelineState(const std::string& name)
                : mName(name), mRootSignature(nullptr), mPipelineState(nullptr) {}

        public:
            virtual ~PipelineState()
            {
                mPipelineState = nullptr;
            }

            inline void setRootSignature(const RootSignature& bindMappings)
            {
                mRootSignature = &bindMappings;
            }

            inline const RootSignature& getRootSignature() const
            {
                HASSERT(mRootSignature != nullptr, "Root signature is null.");
                return *mRootSignature;
            }

            // Returns the raw pointer of ID3D12PipelineState.
            inline const ID3D12PipelineState* get() const { return mPipelineState.Get(); }
            inline ID3D12PipelineState* get() { return mPipelineState.Get(); }

            // Operator overload that returns the raw pointer of ID3D12PipelineState.
            inline const ID3D12PipelineState* operator->() const { return mPipelineState.Get(); }
            inline ID3D12PipelineState* operator->() { return mPipelineState.Get(); }

        protected:
            const std::string mName;
            const RootSignature* mRootSignature;

            Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;
        };
    }

    namespace dx12
    {
        class GraphicsPipelineState : public PipelineState
        {
        public:

            struct Description
            {
                MemoryBlob vertexShader;
                MemoryBlob pixelShader;
                CullMode cullMode;
                D3D12_DEPTH_STENCIL_DESC DepthStencilState;
                D3D12_INPUT_LAYOUT_DESC InputLayout;
                D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue;
                D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType;
                unsigned int numRenderTargets;
                DXGI_FORMAT RTVFormats[8];
                DXGI_FORMAT DSVFormat;
                DXGI_SAMPLE_DESC SampleDesc;
                unsigned int nodeMask;
                D3D12_CACHED_PIPELINE_STATE CachedPSO;
                D3D12_PIPELINE_STATE_FLAGS Flags;
            };

            GraphicsPipelineState(const std::string& name);

        private:
            D3D12_GRAPHICS_PIPELINE_STATE_DESC mPSODesc;
        };
    }
}
