#include "PipelineState.h"

#include "types/DefaultFormats.h"
#include "types/InputClassification.h"
#include "types/Blend.h"
#include "types/BlendOperator.h"
#include "types/BlendLogicOperator.h"
#include "types/CullMode.h"
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
            d3dDescription.pRootSignature = createDescription.rootSignature->get();

            // Shaders
            d3dDescription.VS = createDescription.vertexShader;
            d3dDescription.PS = createDescription.pixelShader;

            // TODO: Stream Output

            // Blend State
            d3dDescription.BlendState = BlendDescription::toD3DBlendDesc(createDescription.blendDescription);

            d3dDescription.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

            // Rasterizer State
            d3dDescription.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            d3dDescription.RasterizerState.CullMode = toD3DCullMode(createDescription.cullMode);

            // Depth Stencil State
            d3dDescription.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

            // Input Layout
            if (createDescription.inputLayout.size() == 0)
            {
                d3dDescription.InputLayout.NumElements = 0;
                d3dDescription.InputLayout.pInputElementDescs = nullptr;
            }
            else
            {
                std::vector<D3D12_INPUT_ELEMENT_DESC> d3dInputElements;
                for (const InputElementsDescription& element : (createDescription.inputLayout))
                {
                    d3dInputElements.emplace_back(
                        InputElementsDescription::toD3DInputElementDesc(element)
                    );
                }
                d3dDescription.InputLayout.NumElements = (UINT)createDescription.inputLayout.size();
                d3dDescription.InputLayout.pInputElementDescs = d3dInputElements.data();
            }

            // TODO: IB Strip Cut Value

            // Primitive Topology
            d3dDescription.PrimitiveTopologyType = toD3DPrimitiveTopologyType(createDescription.primitiveTopologyType);

            // Render Targets Count
            d3dDescription.NumRenderTargets = createDescription.numRenderTargets;

            // RTV Format
            for (unsigned int i = 0; i < createDescription.numRenderTargets; ++i)
            {
                d3dDescription.RTVFormats[i] = toDXGIFormat(createDescription.rtvFormats[i]);
            }

            // DSV Format
            d3dDescription.DepthStencilState.DepthEnable = (BOOL)(createDescription.dsvFormat != DefaultFormats::UNKNOWN);
            d3dDescription.DSVFormat = toDXGIFormat(createDescription.dsvFormat);

            // Sample Desc
            d3dDescription.SampleDesc.Count = createDescription.sampleCount;
            d3dDescription.SampleDesc.Quality = createDescription.sampleQuality;

            // TODO: NodeMask

            // TODO: PSO Cache

            // TODO: Pipeline State Flags

            HRESULT hResult = device->CreateGraphicsPipelineState(&d3dDescription, IID_PPV_ARGS(&mPipelineState));
            HASSERT(SUCCEEDED(hResult), "Failed to create PSO %s.\n%s", createDescription.name.c_str(), utility::translateErrorCode(hResult));
            setD3DName(mPipelineState.Get(), utility::toWideString(createDescription.name).c_str());
        }

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

        const RootSignature& PipelineState::getRootSignature() const
        {
            HASSERT(mRootSignature != nullptr, "Root signature is null.");
            return *mRootSignature;
        }
    }
}
