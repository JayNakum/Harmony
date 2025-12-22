#pragma once

#include "common.h"
#include "directx.h"

#include "Device.h"

#include "CommandObject.h"

namespace graphics
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
