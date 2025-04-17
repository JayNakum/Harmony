#include "common.h"

#include "Entrypoint.h"

#include "dx12.h"

#include <d3dcompiler.h>

#include "CommandListManager.h"
#include "DescriptorHeapWithAllocator.h"

namespace sandbox
{
    using namespace graphics;
    using namespace dx12;

    class Application : public harmony::Application
    {
        static constexpr unsigned int CLIENT_WIDTH = 1920;
        static constexpr unsigned int CLIENT_HEIGHT = 1080;

    public:
        Application()
            : mWindow({L"Sandbox", CLIENT_WIDTH, CLIENT_HEIGHT}),
            mDevice(), mCommandListManager(mDevice)
        {}

        void initialize() override
        {
            mCommandListManager.createNewCommandList(CommandListType::Direct, mCommandList, mCurrentCommandAllocator);
        }

        void run() override
        {
            while (mWindow.pollEvents())
            {

            }
        }

        void deinitialize() override
        {
        }

        ~Application()
        {

        }

    private:
        windows::Window mWindow;
        Device mDevice;

        CommandListManager mCommandListManager;

        inline CommandList& commandList() { return *mCommandList; }
		dx12::CommandList* mCommandList = nullptr;

        inline CommandAllocator& commandAllocator() { return *mCurrentCommandAllocator; }
		dx12::CommandAllocator* mCurrentCommandAllocator = nullptr;

        DescriptorHeapWithAllocator mViewHeap;
        RootSignature mRootSignature;

    };
}

CREATE_APPLICATION(sandbox::Application);
