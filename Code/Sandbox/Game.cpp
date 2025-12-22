#include "common.h"
#include "directx.h"
#include "Entrypoint.h"
#include "Window.h"
#include "Inputs.h"
#include "Maths.h"

namespace sandbox
{
    class Application : public harmony::Application
    {
    public:
        Application()
            : mWindow({ L"Sandbox", 1280, 720 })
        {
        }

        void initialize() override
        {

        }

        void run() override
        {
        }

        void deinitialize() override
        {
        }

        ~Application() override
        {
        }

    private:
        windows::Window mWindow;
    };
}

CREATE_APPLICATION(sandbox::Application);
