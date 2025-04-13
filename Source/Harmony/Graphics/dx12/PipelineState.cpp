#include "PipelineState.h"

#include "Renderer.h"

#include "types/DefaultFormats.h"
#include "types/InputClassification.h"
#include "types/Blend.h"
#include "types/BlendOperator.h"
#include "types/BlendLogicOperator.h"
#include "types/PrimitiveTopology.h"

namespace graphics
{
    namespace dx12
    {
        GraphicsPipelineState::GraphicsPipelineState(Description& description)
            : PipelineState(description.name)
        {
            D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dDescription{};

            // Root Signature
            d3dDescription.pRootSignature = mRootSignature->get();

            // Shaders
            d3dDescription.VS = description.vertexShader;
            d3dDescription.PS = description.pixelShader;

            // TODO: Stream Output

            // Blend State
            d3dDescription.BlendState = BlendDescription::toD3DBlendDesc(description.blendDescription);

            // TODO: Sample Mask

            // Rasterizer State
            d3dDescription.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

            // Depth Stencil State
            d3dDescription.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

            // Input Layout

            std::vector<D3D12_INPUT_ELEMENT_DESC> d3dInputElements;
            for (const InputElementsDescription& element : (description.inputLayout))
            {
                d3dInputElements.emplace_back(
                    InputElementsDescription::toD3DInputElementDesc(element)
                );
            }
            d3dDescription.InputLayout.NumElements = d3dInputElements.size();
            d3dDescription.InputLayout.pInputElementDescs = d3dInputElements.data();

            // TODO: IB Strip Cut Value

            // Primitive Topology
            d3dDescription.PrimitiveTopologyType = toD3DPrimitiveTopologyType(description.primitiveTopologyType);

            // Render Targets Count
            d3dDescription.NumRenderTargets = description.numRenderTargets;

            // RTV Format
            for (unsigned int i = 0; i < GraphicsPipelineState::MAX_RENDER_TARGETS; ++i)
            {
                d3dDescription.RTVFormats[i] = toDXGIFormat(description.rtvFormats[i]);
            }

            // DSV Format
            HASSERT((d3dDescription.DepthStencilState.DepthEnable != (description.dsvFormat == DefaultFormats::UNKNOWN)),
                    "Depth buffer format is invalid.");
            d3dDescription.DSVFormat = toDXGIFormat(description.dsvFormat);

            // Sample Desc
            d3dDescription.SampleDesc.Count = description.sampleCount;
            d3dDescription.SampleDesc.Quality = description.sampleQuality;

            // TODO: NodeMask

            // TODO: PSO Cache

            // TODO: Pipeline State Flags

            HRESULT hResult = GfxDevice()->CreateGraphicsPipelineState(&d3dDescription, IID_PPV_ARGS(&mPipelineState));
            HASSERT_HR(hResult, "Failed to create PSO %s.", description.name.c_str());

#if defined(RELEASE)
            (void)description.name;
#else
            mPipelineState->SetName(utility::toWideString(description.name).c_str());
#endif
        }

        GraphicsPipelineState::~GraphicsPipelineState() {}

        D3D12_INPUT_ELEMENT_DESC GraphicsPipelineState::InputElementsDescription::toD3DInputElementDesc(const InputElementsDescription& description)
        {
            D3D12_INPUT_ELEMENT_DESC d3dInputElementDesc{};
            d3dInputElementDesc.SemanticName = description.name.c_str();
            d3dInputElementDesc.SemanticIndex = 0;
            d3dInputElementDesc.Format = toDXGIFormat(description.format);
            d3dInputElementDesc.InputSlot = 0;
            d3dInputElementDesc.AlignedByteOffset = description.alignedByteOffset;
            d3dInputElementDesc.InputSlotClass = toD3DInputClassification(description.inputSlotClassification);
            d3dInputElementDesc.InstanceDataStepRate = 0;
            return d3dInputElementDesc;
        }

        D3D12_RENDER_TARGET_BLEND_DESC GraphicsPipelineState::BlendDescription::RenderTargetBlendDescription::toD3DRenderTargetBlendDesc(const RenderTargetBlendDescription& description)
        {
            D3D12_RENDER_TARGET_BLEND_DESC d3dRenderTargetBlendDesc{};

            d3dRenderTargetBlendDesc.BlendEnable = description.blendEnable;
            d3dRenderTargetBlendDesc.LogicOpEnable = description.logicOpEnable;
            d3dRenderTargetBlendDesc.SrcBlend = toD3DBlend(description.srcBlend);
            d3dRenderTargetBlendDesc.DestBlend = toD3DBlend(description.destBlend);
            d3dRenderTargetBlendDesc.BlendOp = toD3DBlendOperator(description.blendOp);
            d3dRenderTargetBlendDesc.SrcBlendAlpha = toD3DBlend(description.srcBlendAlpha);
            d3dRenderTargetBlendDesc.DestBlendAlpha = toD3DBlend(description.destBlendAlpha);
            d3dRenderTargetBlendDesc.BlendOpAlpha = toD3DBlendOperator(description.blendOpAlpha);
            d3dRenderTargetBlendDesc.LogicOp = toD3DBlendLogicOperator(description.logicOp);
            d3dRenderTargetBlendDesc.RenderTargetWriteMask = description.renderTargetWriteMask;

            return d3dRenderTargetBlendDesc;
        }

        D3D12_BLEND_DESC GraphicsPipelineState::BlendDescription::toD3DBlendDesc(const BlendDescription& description)
        {
            D3D12_BLEND_DESC d3dBlendDesc{};

            d3dBlendDesc.AlphaToCoverageEnable = description.alphaToCoverageEnable;
            d3dBlendDesc.IndependentBlendEnable = description.independentBlendEnable;

            for (unsigned int i = 0; i < GraphicsPipelineState::MAX_RENDER_TARGETS; ++i)
            {
                d3dBlendDesc.RenderTarget[i] = RenderTargetBlendDescription::toD3DRenderTargetBlendDesc(description.renderTarget[i]);
            }

            return d3dBlendDesc;
        }
    }
}
