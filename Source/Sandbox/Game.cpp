#include "Entrypoint.h"

#include "dx12.h"

#include <d3dcompiler.h>

#include "Core.h"
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
            : mWindow({L"Sandbox", CLIENT_WIDTH, CLIENT_HEIGHT}) {}

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
            GfxCommandListManager().getGraphicsQueue().waitForIdle();
        }

        ~Application()
        {
        }

    private:
        windows::Window mWindow;
    };
}

CREATE_APPLICATION(sandbox::Application);
