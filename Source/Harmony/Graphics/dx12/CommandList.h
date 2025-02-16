#pragma once

#include "common.h"
#include "directx.h"

#include "CommandObject.h"

#include "CommandAllocator.h"

namespace graphics
{
    namespace dx12
    {
        /*
        * Wrapper over ID3D12CommandList
        */
        class CommandList : public CommandObject
        {
        public:
            struct Description
            {
                std::string name;
                CommandListType type;
                CommandAllocator allocator;
            };

            // Creates an ID3D12CommandList.
            CommandList(Description& createDesc);

            // Closes and destroys the command list.
            ~CommandList();

            // Returns the raw pointer to ID3D12CommandList required for ExecuteCommandList
            inline const ID3D12CommandList* get() const { return mCommandList.Get(); }
            inline ID3D12CommandList* get() { return mCommandList.Get(); }

            // Returns the raw pointer to ID3D12GraphicsCommandList
            inline const ID3D12GraphicsCommandList* operator->() const { return mCommandList.Get(); }
            inline ID3D12GraphicsCommandList* operator->() { return mCommandList.Get(); }

        private:
            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
        };
    }
}
