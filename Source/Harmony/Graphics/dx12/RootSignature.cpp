#include "RootSignature.h"

#include "types/RootSignatureFlag.h"

#include "Utility.h"

#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        RootSignature::RootSignature(Device& device, const Description& createDescription)
        {
            D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
            rootSignatureDesc.NumParameters = (UINT)createDescription.rootParameters.size();
            rootSignatureDesc.pParameters = (const D3D12_ROOT_PARAMETER*)createDescription.rootParameters.data();
            rootSignatureDesc.NumStaticSamplers = (UINT)createDescription.staticSamplers.size();
            rootSignatureDesc.pStaticSamplers = (const D3D12_STATIC_SAMPLER_DESC*)createDescription.staticSamplers.data();
            rootSignatureDesc.Flags = toD3DRootSignatureFlags(createDescription.flags);

            Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
            Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

            {// Serialize root signature
                HRESULT hResult = D3D12SerializeRootSignature(
                    &rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                    serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

                HASSERT(SUCCEEDED(hResult), "Failed to serialize root signature %s.\n%s\n%s", createDescription.name, errorBlob ? (char*)errorBlob->GetBufferPointer() : "Unknown error", utility::translateErrorCode(hResult));
            }
            {// Create root signature
                HRESULT hResult = device->CreateRootSignature(
                    0,
                    serializedRootSig->GetBufferPointer(),
                    serializedRootSig->GetBufferSize(),
                    IID_PPV_ARGS(&mRootSignature));

                HASSERT(SUCCEEDED(hResult), "Failed to create root signature %s.\n%s", createDescription.name, utility::translateErrorCode(hResult));
            }

            setName(mRootSignature.Get(), utility::toWideString(createDescription.name).c_str());
        }

        RootSignature::~RootSignature()
        {
            mRootSignature = nullptr;
        }
    }
}
