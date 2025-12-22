#include "common.h"

#include "Entrypoint.h"
#include "Window.h"
#include "Engine.h"

namespace sandbox
{
    class Application : public harmony::Application
    {
    public:
        Application()
            : mWindow({ L"Sandbox", 1280, 720 }),
            mEngine(mWindow)
        {
        }

        void initialize() override
        {

        }

        void run() override
        {
            mEngine.run();
        }

        void deinitialize() override
        {
        }

        ~Application() override
        {
        }

    private:
        windows::Window mWindow;
        engine::Engine mEngine;
    };
}

CREATE_APPLICATION(sandbox::Application);
