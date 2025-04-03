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
                : mInitializedSamplers(0)
            {
                reset(parametersCount, staticSamplersCount);
            }

            // Reset the root parameters list.
            void reset(const size_t parametersCount, const size_t staticSamplersCount = 0)
            {
                mRootParameters.resize(parametersCount);
                mRootParameters.shrink_to_fit();

                mStaticSamplers.resize(staticSamplersCount);
                mStaticSamplers.shrink_to_fit();
                mInitializedSamplers = 0;
            }

            // StaticSamplerDescription with default values.
            // The default values are as per CD3DX12_STATIC_SAMPLER_DESC
            struct StaticSamplerDescription
            {
                sampler::Filter filter = sampler::Filter::Anisotropic;
                sampler::TextureAddressMode addressU = sampler::TextureAddressMode::Wrap;
                sampler::TextureAddressMode addressV = addressU;
                sampler::TextureAddressMode addressW = addressV;
                float mipLODBias = 0;
                unsigned int maxAnisotropy = 16;
                sampler::ComparisionFunction comparisonFunc = sampler::ComparisionFunction::LessEqual;
                sampler::StaticBorderColor borderColor = sampler::StaticBorderColor::OpaqueWhite;
                float minLOD = 0.0f;
                float maxLOD = FLT_MAX;
            };
            void initStaticSampler(unsigned int shaderRegister, const StaticSamplerDescription& sampler = {}, const ShaderVisibility visibility = ShaderVisibility::All, unsigned int registerSpace = 0);

            // create and/or serialize root signature
            void finalize(const RootSignatureFlags flags, const std::string name);

            // Returns the raw pointer of ID3D12RootSignature.
            inline ID3D12RootSignature* get() { mRootSignature.Get(); }
            inline const ID3D12RootSignature* get() const { mRootSignature.Get(); }

            // Operator overload for array like indexing
            inline RootParameter& operator[] (size_t index) { return mRootParameters.at(index); }
            inline const RootParameter& operator[] (size_t index) const { return mRootParameters.at(index); }

        private:
            Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;

            // Array of root parameters
            std::vector<RootParameter> mRootParameters;
            // Array of static samplers
            std::vector<D3D12_STATIC_SAMPLER_DESC> mStaticSamplers;
            unsigned int mInitializedSamplers;
        };
    }
}
