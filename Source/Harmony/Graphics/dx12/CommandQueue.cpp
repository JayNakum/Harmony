#include "CommandQueue.h"

#include "types/CommandListType.h"

#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        CommandQueue::CommandQueue(Device& device, const Description& createDescription)
            : CommandObject(createDescription.type),
            mCommandQueue(nullptr)
        {
            D3D12_COMMAND_QUEUE_DESC createDesc{};
            createDesc.Type = toD3DCommandListType(mType);
            createDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

            HRESULT hResult = device->CreateCommandQueue(&createDesc, IID_PPV_ARGS(&mCommandQueue));
            HASSERT(SUCCEEDED(hResult), "Failed to create command queue %s.\n%s", createDescription.name, utility::translateErrorCode(hResult));

            setName(mCommandQueue.Get(), utility::toWideString(createDescription.name).c_str());
        }

        const uint64_t CommandQueue::executeCommandList(dx12::CommandList& commandList)
        {
            HRESULT hResult = commandList->Close();
            HASSERT(SUCCEEDED(hResult), "Failed to close command list.\n%s", utility::translateErrorCode(hResult));

            ID3D12CommandList* list = commandList.get();
            mCommandQueue->ExecuteCommandLists(1, &list);

            return incrementFenceNoMutex();
        }

        const uint64_t CommandQueue::executeCommandLists(ExecuteCommandListsParams& executeCommandListsParams)
        {

            std::vector<ID3D12CommandList*> d3dLists(count);
            for (unsigned int i = 0; i < count; ++i)
            {
                d3dLists.push_back(commandList[i].get());
            }

            mCommandQueue->ExecuteCommandLists(count, d3dLists.data());
            return incrementFenceNoMutex();
        }

        CommandQueue::~CommandQueue()
        {
            mCommandQueue = nullptr;
        }
    }
}
