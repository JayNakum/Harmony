#pragma once

#include "common.h"
#include "Log.h"
#include "directx.h"

#include "Device.h"

#include "CommandObject.h"

namespace graphics
{
    namespace dx12
    {
        /*
        *  A wrapper over ID3D12CommandAllocator
        */
        class CommandAllocator : public CommandObject
        {
        public:
            struct Description
            {
                std::string name;
                CommandListType type;
            };

            // Creates an ID3D12CommandAllocator of type createDescription.type
            CommandAllocator(Device& device, const Description& createDescription);

            // Destroys the ID3D12CommandAllocator
            ~CommandAllocator();

            // Returns the raw pointer on ID3D12CommandAllocator
            inline const ID3D12CommandAllocator* get() const { return mCommandAllocator.Get(); }
            inline ID3D12CommandAllocator* get() { return mCommandAllocator.Get(); }

            // Operator overload that returns the raw pointer of ID3D12CommandAllocator.
            inline const ID3D12CommandAllocator* operator->() const { return mCommandAllocator.Get(); }
            inline ID3D12CommandAllocator* operator->() { return mCommandAllocator.Get(); }

        private:
            Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
        };
    }
}
