
#include "Engine.h"

#include "Graphics/Renderer.h"

namespace engine
{
    Engine::Engine(windows::Window& mainWindow)
        : mWindow(mainWindow),
        mInputs(mWindow.getHandle()),
        mRenderer(nullptr)
    {
        mRenderer = new Renderer(mWindow.getHandle());
    }

    uint64_t Engine::msFrameIndex = 0;
    void Engine::run()
    {
        FrameConstants frame;

        while (!(mInputs.isPressed(core::Inputs::Digital::Key_escape)) &&
            mWindow.pollEvents())
        {
            frame.deltaTime = mFrameTimer.restart();
            frame.totalTime = mFrameTimer.total();

            mInputs.update(frame.deltaTime);

            if (mWindow.didResize() && (mWindow.getDescription().width != 0 && mWindow.getDescription().height != 0))
            {
                mRenderer->resize(mWindow.getDescription().width, mWindow.getDescription().height);
            }
            else
            {
                static double accumulator = 0.0;
                constexpr double fixedTimeStep = 1.0 / 60.0;
                accumulator += static_cast<double>(frame.deltaTime);

                while (accumulator >= fixedTimeStep)
                {
                    accumulator -= fixedTimeStep;
                    // fixedUpdates(fixedTimeStep)
                    mRenderer->update(fixedTimeStep);
                }

                mRenderer->render(frame);

                ++msFrameIndex;

                calculateFPS(frame.deltaTime);
            }
        }
    }

    Engine::~Engine()
    {
        delete mRenderer;
    }

    void Engine::calculateFPS(const float deltaTime)
    {
        static float fpsTimer = 0.0f;
        static unsigned int frameCount = 0;

        ++frameCount;
        fpsTimer += deltaTime;

        if (fpsTimer >= 1.0f)
        {
            unsigned int fps = frameCount;
            float msPerFrame = 1000.0f / static_cast<float>(std::max<unsigned int>(fps, 1u));

            std::string fpsString = " | FPS " + std::to_string(fps) +
                " (" + std::to_string(msPerFrame) + "ms)";

            mWindow.appendWindowText(fpsString);

            fpsTimer = 0.0f;
            frameCount = 0;
        }
    }
}
