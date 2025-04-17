#include "PipelineState.h"

#include "types/DefaultFormats.h"
#include "types/InputClassification.h"
#include "types/Blend.h"
#include "types/BlendOperator.h"
#include "types/BlendLogicOperator.h"
#include "types/PrimitiveTopology.h"

#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        GraphicsPipelineState::GraphicsPipelineState(Device& device, Description& createDescription)
            : PipelineState(createDescription.name)
        {
            D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dDescription{};

            // Root Signature
            d3dDescription.pRootSignature = mRootSignature->get();

            // Shaders
            d3dDescription.VS = createDescription.vertexShader;
            d3dDescription.PS = createDescription.pixelShader;

            // TODO: Stream Output

            // Blend State
            d3dDescription.BlendState = BlendDescription::toD3DBlendDesc(createDescription.blendDescription);

            // TODO: Sample Mask

            // Rasterizer State
            d3dDescription.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

            // Depth Stencil State
            d3dDescription.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

            // Input Layout

            std::vector<D3D12_INPUT_ELEMENT_DESC> d3dInputElements;
            for (const InputElementsDescription& element : (createDescription.inputLayout))
            {
                d3dInputElements.emplace_back(
                    InputElementsDescription::toD3DInputElementDesc(element)
                );
            }
            d3dDescription.InputLayout.NumElements = (UINT)d3dInputElements.size();
            d3dDescription.InputLayout.pInputElementDescs = d3dInputElements.data();

            // TODO: IB Strip Cut Value

            // Primitive Topology
            d3dDescription.PrimitiveTopologyType = toD3DPrimitiveTopologyType(createDescription.primitiveTopologyType);

            // Render Targets Count
            d3dDescription.NumRenderTargets = createDescription.numRenderTargets;

            // RTV Format
            for (unsigned int i = 0; i < GraphicsPipelineState::MAX_RENDER_TARGETS; ++i)
            {
                d3dDescription.RTVFormats[i] = toDXGIFormat(createDescription.rtvFormats[i]);
            }

            // DSV Format
            HASSERT((d3dDescription.DepthStencilState.DepthEnable != (BOOL)(createDescription.dsvFormat == DefaultFormats::UNKNOWN)),
                "Depth buffer format is invalid.");
            d3dDescription.DSVFormat = toDXGIFormat(createDescription.dsvFormat);

            // Sample Desc
            d3dDescription.SampleDesc.Count = createDescription.sampleCount;
            d3dDescription.SampleDesc.Quality = createDescription.sampleQuality;

            // TODO: NodeMask

            // TODO: PSO Cache

            // TODO: Pipeline State Flags

            HRESULT hResult = device->CreateGraphicsPipelineState(&d3dDescription, IID_PPV_ARGS(&mPipelineState));
            HASSERT(SUCCEEDED(hResult), "Failed to create PSO %s.\n%s", createDescription.name.c_str(), utility::translateErrorCode(hResult));
            setName(mPipelineState.Get(), utility::toWideString(createDescription.name).c_str());
        }

        D3D12_INPUT_ELEMENT_DESC GraphicsPipelineState::InputElementsDescription::toD3DInputElementDesc(const InputElementsDescription& createDescription)
        {
            D3D12_INPUT_ELEMENT_DESC d3dInputElementDesc{};
            d3dInputElementDesc.SemanticName = createDescription.name.c_str();
            d3dInputElementDesc.SemanticIndex = 0;
            d3dInputElementDesc.Format = toDXGIFormat(createDescription.format);
            d3dInputElementDesc.InputSlot = 0;
            d3dInputElementDesc.AlignedByteOffset = createDescription.alignedByteOffset;
            d3dInputElementDesc.InputSlotClass = toD3DInputClassification(createDescription.inputSlotClassification);
            d3dInputElementDesc.InstanceDataStepRate = 0;
            return d3dInputElementDesc;
        }

        D3D12_RENDER_TARGET_BLEND_DESC GraphicsPipelineState::BlendDescription::RenderTargetBlendDescription::toD3DRenderTargetBlendDesc(const RenderTargetBlendDescription& createDescription)
        {
            D3D12_RENDER_TARGET_BLEND_DESC d3dRenderTargetBlendDesc{};

            d3dRenderTargetBlendDesc.BlendEnable = createDescription.blendEnable;
            d3dRenderTargetBlendDesc.LogicOpEnable = createDescription.logicOpEnable;
            d3dRenderTargetBlendDesc.SrcBlend = toD3DBlend(createDescription.srcBlend);
            d3dRenderTargetBlendDesc.DestBlend = toD3DBlend(createDescription.destBlend);
            d3dRenderTargetBlendDesc.BlendOp = toD3DBlendOperator(createDescription.blendOp);
            d3dRenderTargetBlendDesc.SrcBlendAlpha = toD3DBlend(createDescription.srcBlendAlpha);
            d3dRenderTargetBlendDesc.DestBlendAlpha = toD3DBlend(createDescription.destBlendAlpha);
            d3dRenderTargetBlendDesc.BlendOpAlpha = toD3DBlendOperator(createDescription.blendOpAlpha);
            d3dRenderTargetBlendDesc.LogicOp = toD3DBlendLogicOperator(createDescription.logicOp);
            d3dRenderTargetBlendDesc.RenderTargetWriteMask = createDescription.renderTargetWriteMask;

            return d3dRenderTargetBlendDesc;
        }

        D3D12_BLEND_DESC GraphicsPipelineState::BlendDescription::toD3DBlendDesc(const BlendDescription& createDescription)
        {
            D3D12_BLEND_DESC d3dBlendDesc{};

            d3dBlendDesc.AlphaToCoverageEnable = createDescription.alphaToCoverageEnable;
            d3dBlendDesc.IndependentBlendEnable = createDescription.independentBlendEnable;

            for (unsigned int i = 0; i < GraphicsPipelineState::MAX_RENDER_TARGETS; ++i)
            {
                d3dBlendDesc.RenderTarget[i] = RenderTargetBlendDescription::toD3DRenderTargetBlendDesc(createDescription.renderTarget[i]);
            }

            return d3dBlendDesc;
        }

        const RootSignature& PipelineState::getRootSignature() const
        {
            HASSERT(mRootSignature != nullptr, "Root signature is null.");
            return *mRootSignature;
        }

    }
}
