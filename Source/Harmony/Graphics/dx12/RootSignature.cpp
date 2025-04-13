#include "RootSignature.h"

#include "Core.h"

#include "types/RootSignatureFlag.h"

#include "Utility.h"

namespace graphics
{
    namespace dx12
    {
        RootSignature::RootSignature(const Description& description)
        {
            D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
            rootSignatureDesc.NumParameters = (UINT)description.rootParameters.size();
            rootSignatureDesc.pParameters = (const D3D12_ROOT_PARAMETER*)description.rootParameters.data();
            rootSignatureDesc.NumStaticSamplers = (UINT)description.staticSamplers.size();
            rootSignatureDesc.pStaticSamplers = (const D3D12_STATIC_SAMPLER_DESC*)description.staticSamplers.data();
            rootSignatureDesc.Flags = toD3DFlags(description.flags);

            Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
            Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

            {// Serialize root signature
                HRESULT hResult = D3D12SerializeRootSignature(
                    &rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                    serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

                HASSERT_HR(hResult, "Failed to serialize root signature %s.\n%s", description.name, errorBlob ? (char*)errorBlob->GetBufferPointer() : "Unknown error");
            }
            {// Create root signature
                HRESULT hResult = GfxDevice()->CreateRootSignature(
                    0,
                    serializedRootSig->GetBufferPointer(),
                    serializedRootSig->GetBufferSize(),
                    IID_PPV_ARGS(&mRootSignature));

                HASSERT_HR(hResult, "Failed to create root signature %s.", description.name);
            }

            setName(mRootSignature.Get(), utility::toWideString(description.name).c_str());
        }
    }
}
