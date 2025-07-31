#pragma once

#include "common.h"
#include "Log.h"
#include "directx.h"

#include "Device.h"

#include "RootSignatureEnums.h"

#include "RootParameter.h"

namespace graphics
{
    /*
    * Wrapper over ID3D12RootSignature.
    * It contains a vector of RootParameter
    */
    class RootSignature
    {
        static constexpr unsigned int MAX_STATIC_SAMPLERS = 6;

    public:
        struct Description
        {
            std::string name;
            std::vector<RootParameter> rootParameters;
            std::array<StaticSampler, MAX_STATIC_SAMPLERS> staticSamplers;
        };

        // Creates and serializes root signature
        RootSignature(Device& device, const Description& createDescription);

        ~RootSignature();

        // Returns the raw pointer of ID3D12RootSignature.
        inline ID3D12RootSignature* get() const { return mRootSignature.Get(); }
        inline ID3D12RootSignature* get() { return mRootSignature.Get(); }

        const Microsoft::WRL::ComPtr<ID3D12RootSignature>& operator()() const { return mRootSignature; }

    private:
        const D3D12_ROOT_SIGNATURE_FLAGS getD3DFlagsFromParameters(const std::vector<RootParameter>& parameters);

    private:
        Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
    };
}
