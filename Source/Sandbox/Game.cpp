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
        static constexpr unsigned int CLIENT_WIDTH = 1280;
        static constexpr unsigned int CLIENT_HEIGHT = 720;

    public:
        Application()
            : mWindow({L"Sandbox", CLIENT_WIDTH, CLIENT_HEIGHT}) ,
            mDevice(),
            mCommandListManager(mDevice)
        {}

        void initialize() override
        {
        }

        void run() override
        {
            while (mWindow.pollEvents())
            {

            }
        }

        void deinitialize() override
        {
            mCommandListManager.getGraphicsQueue().waitForIdle();
        }

        ~Application()
        {
        }

    private:
        windows::Window mWindow;
        Device mDevice;
        CommandListManager mCommandListManager;
    };
}

CREATE_APPLICATION(sandbox::Application);
