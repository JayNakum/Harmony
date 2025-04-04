#include "PipelineState.h"

namespace graphics
{
    namespace dx12
    {
        GraphicsPipelineState::GraphicsPipelineState(const std::string& name)
            : PipelineState(name)
        {
            mPSODesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
            mPSODesc.pRootSignature = mRootSignature.Get();
            mPSODesc.VS =
            {
                reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()),
                mShaders["standardVS"]->GetBufferSize()
            };
            mPSODesc.PS =
            {
                reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()),
                mShaders["opaquePS"]->GetBufferSize()
            };
            mPSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            mPSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            mPSODesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
            mPSODesc.SampleMask = UINT_MAX;
            mPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            mPSODesc.NumRenderTargets = 1;
            mPSODesc.RTVFormats[0] = mBackBufferFormat;
            mPSODesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
            mPSODesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
            mPSODesc.DSVFormat = mDepthStencilFormat;
        }
    }
}
