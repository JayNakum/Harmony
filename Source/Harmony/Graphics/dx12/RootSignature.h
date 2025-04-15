#pragma once

#include "common.h"
#include "directx.h"

#include "Device.h"

#include "RootSignatureEnums.h"

#include "RootParameter.h"

namespace graphics
{
    namespace dx12
    {
        /*
        * Wrapper over ID3D12RootSignature.
        * It contains a vector of dx12::RootParameter
        */
        class RootSignature
        {
        public:
            struct Description
            {
                std::string name;
                RootSignatureFlags flags;
                std::vector<RootParameter> rootParameters;
                std::vector<StaticSampler> staticSamplers;
            };

            // Creates and serializes root signature
            RootSignature(Device& device, const Description& createDescription);

            ~RootSignature();

            // Returns the raw pointer of ID3D12RootSignature.
            inline ID3D12RootSignature* get() const { return mRootSignature.Get(); }
            inline ID3D12RootSignature* get() { return mRootSignature.Get(); }

            const Microsoft::WRL::ComPtr<ID3D12RootSignature>& operator()() const { return mRootSignature; }

        private:
            Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
        };
    }
}
