#pragma once

#include "common.h"
#include "directx.h"

#include "RootSignatureEnums.h"

#include "RootParameter.h"

namespace graphics
{
    namespace dx12
    {
        class RootSignature
        {
        public:
            RootSignature(const size_t parametersCount, const size_t staticSamplersCount = 0)
            {
                reset(parametersCount, staticSamplersCount);
            }

            void reset(const size_t parametersCount, const size_t staticSamplersCount = 0)
            {
                mRootParameters.resize(parametersCount);
                mRootParameters.shrink_to_fit();
            }

            void finalize();

            // Returns the raw pointer of ID3D12RootSignature.
            inline ID3D12RootSignature* get() { mRootSignature.Get(); }
            inline const ID3D12RootSignature* get() const { mRootSignature.Get(); }

            // Operator overload for array like indexing
            inline RootParameter& operator[] (size_t index) { return mRootParameters.at(index); }
            inline const RootParameter& operator[] (size_t index) const { return mRootParameters.at(index); }

        private:
            Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;

            std::vector<RootParameter> mRootParameters;

            static const std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> staticSamplers()
            {
                static const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
                    0, // shaderRegister
                    D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
                    D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
                    D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
                    D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

                static const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
                    1, // shaderRegister
                    D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
                    D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressU
                    D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressV
                    D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

                static const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
                    2, // shaderRegister
                    D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
                    D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
                    D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
                    D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

                static const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
                    3, // shaderRegister
                    D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
                    D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressU
                    D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressV
                    D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

                static const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
                    4, // shaderRegister
                    D3D12_FILTER_ANISOTROPIC, // filter
                    D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
                    D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
                    D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressW
                    0.0f, // mipLODBias
                    8); // maxAnisotropy

                static const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
                    5, // shaderRegister
                    D3D12_FILTER_ANISOTROPIC, // filter
                    D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressU
                    D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressV
                    D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressW
                    0.0f, // mipLODBias
                    8); // maxAnisotropy

                return {
                    pointWrap, pointClamp,
                    linearWrap, linearClamp,
                    anisotropicWrap, anisotropicClamp
                };
            }		
        };
    }
}
