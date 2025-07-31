#include "RootSignature.h"

#include "types/RootSignatureFlag.h"

#include "Utility.h"

namespace graphics
{
    RootSignature::RootSignature(Device& device, const Description& createDescription)
    {
        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.NumParameters = (UINT)createDescription.rootParameters.size();

        std::vector<D3D12_ROOT_PARAMETER> d3dParameters;
        d3dParameters.reserve(createDescription.rootParameters.size());
        for (const RootParameter& param : createDescription.rootParameters)
        {
            d3dParameters.push_back(param.get());
        }
        rootSignatureDesc.pParameters = d3dParameters.data();

        rootSignatureDesc.NumStaticSamplers = (UINT)createDescription.staticSamplers.size();
        rootSignatureDesc.pStaticSamplers = (const D3D12_STATIC_SAMPLER_DESC*)createDescription.staticSamplers.data();

        rootSignatureDesc.Flags = getD3DFlagsFromParameters(createDescription.rootParameters);

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

        setD3DName(mRootSignature.Get(), utility::toWideString(createDescription.name).c_str());
    }

    RootSignature::~RootSignature()
    {
        mRootSignature = nullptr;
    }

    const D3D12_ROOT_SIGNATURE_FLAGS RootSignature::getD3DFlagsFromParameters(const std::vector<RootParameter>& parameters)
    {
        D3D12_ROOT_SIGNATURE_FLAGS flags =
            D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;

        for (const RootParameter& param : parameters)
        {
            if (hasFlag(param.getShaderVisibility(), ShaderVisibility::Vertex))
            {
                flags &= ~(D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS);
            }
            if (hasFlag(param.getShaderVisibility(), ShaderVisibility::Hull))
            {
                flags &= ~(D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS);
            }
            if (hasFlag(param.getShaderVisibility(), ShaderVisibility::Domain))
            {
                flags &= ~(D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS);
            }
            if (hasFlag(param.getShaderVisibility(), ShaderVisibility::Geometry))
            {
                flags &= ~(D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
            }
            if (hasFlag(param.getShaderVisibility(), ShaderVisibility::Pixel))
            {
                flags &= ~(D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);
            }
            if (hasFlag(param.getShaderVisibility(), ShaderVisibility::Amplification))
            {
                flags &= ~(D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS);
            }
            if (hasFlag(param.getShaderVisibility(), ShaderVisibility::Mesh))
            {
                flags &= ~(D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS);
            }
        }

        return flags;
    }
}
