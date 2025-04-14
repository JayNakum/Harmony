#pragma once

#include "common.h"
#include "directx.h"

#include "PipelineStateEnums.h"

#include "Format.h"
#include "RootSignature.h"
#include "Blob.h"

namespace graphics
{
    namespace dx12
    {
        /*
        * Wrapper over ID3D12PipelineState.
        * This is the parent class for the graphics and compute pipeline states.
        */
        class PipelineState
        {
        protected:
            PipelineState(const std::string& name)
                : mName(name), mRootSignature(nullptr), mPipelineState(nullptr) {}

        public:
            inline void setRootSignature(const RootSignature& bindMappings)
            {
                mRootSignature = &bindMappings;
            }

            inline const RootSignature& getRootSignature() const
            {
                HASSERT(mRootSignature != nullptr, "Root signature is null.");
                return *mRootSignature;
            }

            virtual ~PipelineState()
            {
                mPipelineState = nullptr;
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
        /*
        * Graphic pipeline.
        */
        class GraphicsPipelineState : public PipelineState
        {
            static constexpr unsigned int MAX_RENDER_TARGETS = 8; // Same as the maximum amount supported in dx12.

        public:
            // VS_INPUT
            struct InputElementsDescription
            {
                std::string name;
                Format format;
                unsigned int alignedByteOffset;
                InputClassification inputSlotClassification;

                static D3D12_INPUT_ELEMENT_DESC toD3DInputElementDesc(const InputElementsDescription& description);
            };

            // Blend Description with default values.
            // Default values are as per CD3DX12_BLEND_DESC.
            struct BlendDescription
            {
                struct RenderTargetBlendDescription
                {
                    bool blendEnable = false;
                    bool logicOpEnable = false;
                    Blend srcBlend = Blend::One;
                    Blend destBlend = Blend::Zero;
                    BlendOperator blendOp = BlendOperator::Add;
                    Blend srcBlendAlpha = Blend::One;
                    Blend destBlendAlpha = Blend::Zero;
                    BlendOperator blendOpAlpha = BlendOperator::Add;
                    BlendLogicOperator logicOp = BlendLogicOperator::NOOP;
                    uint8_t renderTargetWriteMask = (uint8_t)ColorWriteMask::All; // Note: this works directly because the values of ColorWriteMask are same as D3D12_COLOR_WRITE_ENABLE

                    static D3D12_RENDER_TARGET_BLEND_DESC toD3DRenderTargetBlendDesc(const RenderTargetBlendDescription& description);
                };

                bool alphaToCoverageEnable = false;
                bool independentBlendEnable = false;
                std::array<RenderTargetBlendDescription, MAX_RENDER_TARGETS> renderTarget = {};

                static D3D12_BLEND_DESC toD3DBlendDesc(const BlendDescription& description);
            };

            // Graphics PSO create description.
            struct Description
            {
                std::string name;
                std::vector<InputElementsDescription> inputLayout;
                Blob vertexShader;
                Blob pixelShader;
                // TODO: Add more shaders as needed.

                FillMode fillMode = FillMode::Solid;
                CullMode cullMode = CullMode::None;
                BlendDescription blendDescription{};
                PrimitiveTopology primitiveTopologyType = PrimitiveTopology::Triangle;
                unsigned int numRenderTargets = 1;
                std::array<Format, MAX_RENDER_TARGETS> rtvFormats = { DefaultFormats::R8G8B8A8_UNORM };
                Format dsvFormat = DefaultFormats::D24_UNORM_S8_UINT;
                unsigned int sampleCount = 1;
                unsigned int sampleQuality = 0;
            };

            GraphicsPipelineState(Description& createDescription);
            ~GraphicsPipelineState() = default;
        };
    }
}
