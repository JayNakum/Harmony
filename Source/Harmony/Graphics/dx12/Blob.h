#pragma once

#include "common.h"
#include "directx.h"

namespace graphics
{
    namespace dx12
    {
        /*
        * Wrapper over ID3DBlob.
        * This is just to make the project API independent.
        */
        class Blob
        {
        public:
            const Microsoft::WRL::ComPtr<ID3DBlob>& operator()() const { return mBlob; }

            // Operator overload that returns the raw pointer to ID3DBlob
            const ID3DBlob* operator->() const { return mBlob.Get(); }
            ID3DBlob* operator->() { return mBlob.Get(); }

            // Returns the raw pointer to ID3DBlob
            const ID3DBlob* get() const { return mBlob.Get(); }
            ID3DBlob* get() { return mBlob.Get(); }

            ID3DBlob** getAddressOf() { return mBlob.GetAddressOf(); }

            D3D12_SHADER_BYTECODE getBytecode() const
            {
                return {
                    mBlob ? mBlob->GetBufferPointer() : nullptr,
                    mBlob ? mBlob->GetBufferSize() : 0
                };
            }

            // For implicit conversion to shader bytecode
            operator D3D12_SHADER_BYTECODE() const { return getBytecode(); }

            ID3DBlob** operator&() { return mBlob.GetAddressOf(); }
            const ID3DBlob* const* operator&() const { return mBlob.GetAddressOf(); }

        private:
            Microsoft::WRL::ComPtr<ID3DBlob> mBlob;
        };
    }
}
