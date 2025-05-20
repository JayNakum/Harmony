#include "common.h"

#include "Entrypoint.h"
#include "Window.h"
#include "Inputs.h"
#include "Renderer.h"

namespace sandbox
{
    class Application : public harmony::Application
    {
        static constexpr unsigned int CLIENT_WIDTH = 1080;
        static constexpr unsigned int CLIENT_HEIGHT = 720;
    public:
        Application()
            : mWindow({L"Sandbox", CLIENT_WIDTH, CLIENT_HEIGHT}),
            mInputs(mWindow.getHandle()),
            mRenderer(mWindow.getHandle(), CLIENT_WIDTH, CLIENT_HEIGHT)
        {
        }

        void initialize() override
        {

        }

        void run() override
        {
            while (!(mInputs.isPressed(core::Inputs::Digital::Key_escape)) &&
                   mWindow.pollEvents())
            {
                mInputs.update(0);

                if (mWindow.didResize())
                {
                    mRenderer.resize(mWindow.getDescription().width, mWindow.getDescription().height);
                }

                mRenderer.render();
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
        windows::Inputs mInputs;

        engine::Renderer mRenderer;
    };
}

CREATE_APPLICATION(sandbox::Application);
