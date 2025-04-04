#pragma once

#include "common.h"
#include "directx.h"

#include "RootSignatureEnums.h"

#include "RootParameter.h"

#include "Log.h"

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
            RootSignature(const Description& description);

            // Returns the raw pointer of ID3D12RootSignature.
            inline ID3D12RootSignature* get() { mRootSignature.Get(); }
            inline const ID3D12RootSignature* get() const { mRootSignature.Get(); }

        private:
            Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
        };
    }
}
