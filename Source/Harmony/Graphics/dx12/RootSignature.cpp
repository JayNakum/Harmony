#include "RootSignature.h"

#include "Renderer.h"

#include "types/RootSignatureFlag.h"
#include "types/Filter.h"
#include "types/TextureAddressMode.h"
#include "types/ComparisionFunction.h"
#include "types/StaticBorderColor.h"
#include "types/ShaderVisibility.h"

#include "Utility.h"

namespace graphics
{
    namespace dx12
    {
        void RootSignature::initStaticSampler(unsigned int shaderRegister, const StaticSamplerDescription& sampler, const ShaderVisibility visibility, unsigned int registerSpace)
        {
            HASSERT(mInitializedSamplers < mStaticSamplers.size(), "Trying to initialize more than initial specified size (%d) samplers.", mStaticSamplers.size());
            D3D12_STATIC_SAMPLER_DESC& staticSampler = mStaticSamplers.at(mInitializedSamplers++);

            staticSampler.ShaderRegister = shaderRegister;
            staticSampler.RegisterSpace = registerSpace;
            staticSampler.ShaderVisibility = shaderVisibility.at(visibility);

            staticSampler.Filter = sampler::dx12::filter.at(sampler.filter);
            staticSampler.AddressU = sampler::dx12::textureAddressMode.at(sampler.addressU);
            staticSampler.AddressV = sampler::dx12::textureAddressMode.at(sampler.addressV);
            staticSampler.AddressW = sampler::dx12::textureAddressMode.at(sampler.addressW);
            staticSampler.MipLODBias = sampler.mipLODBias;
            staticSampler.MaxAnisotropy = sampler.maxAnisotropy;
            staticSampler.ComparisonFunc = sampler::dx12::comparisionFunction.at(sampler.comparisonFunc);
            staticSampler.BorderColor = sampler::dx12::staticBorderColor.at(sampler.borderColor);
            staticSampler.MinLOD = sampler.minLOD;
            staticSampler.MaxLOD = sampler.maxLOD;
        }

        void RootSignature::finalize(const RootSignatureFlags flags, const std::string name)
        {
            HASSERT(mInitializedSamplers == mStaticSamplers.size(), "%d static samplers are not initialized.", (mStaticSamplers.size() - mInitializedSamplers));

            D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
            rootSignatureDesc.NumParameters = (UINT)mRootParameters.size();
            rootSignatureDesc.pParameters = (const D3D12_ROOT_PARAMETER*)mRootParameters.data();
            rootSignatureDesc.NumStaticSamplers = (UINT)mStaticSamplers.size();
            rootSignatureDesc.pStaticSamplers = (const D3D12_STATIC_SAMPLER_DESC*)mStaticSamplers.data();
            rootSignatureDesc.Flags = toD3DFlags(flags);

            Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
            Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

            {// Serialize root signature
                HRESULT hResult = D3D12SerializeRootSignature(
                    &rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                    serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

                HASSERT_HR(hResult, "Failed to serialize root signature %s.\n%s", name, errorBlob ? (char*)errorBlob->GetBufferPointer() : "Unknown error");
            }
            {// Create root signature
                HRESULT hResult = GfxDevice()->CreateRootSignature(
                    0,
                    serializedRootSig->GetBufferPointer(),
                    serializedRootSig->GetBufferSize(),
                    IID_PPV_ARGS(&mRootSignature));

                HASSERT_HR(hResult, "Failed to create root signature %s.", name);
            }

#if defined(RELEASE)
            (void)name;
#else
            mRootSignature->SetName(utility::toWideString(name).c_str());
#endif
        }
    }
}
