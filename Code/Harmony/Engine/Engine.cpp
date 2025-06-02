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
        float deltaTime = 0.0f;

        while (!(mInputs.isPressed(core::Inputs::Digital::Key_escape)) &&
               mWindow.pollEvents())
        {
            deltaTime = mFrameTimer.restart();
            mInputs.update(deltaTime);

            if (mWindow.didResize())
            {
                mRenderer->resize(mWindow.getDescription().width, mWindow.getDescription().height);
            }

            static double accumulator = 0.0f;
            constexpr double fixedTimeStep = 1.0f / 60.0f;
            accumulator += static_cast<double>(deltaTime);

            while (accumulator >= fixedTimeStep)
            {
                accumulator -= fixedTimeStep;
                // fixedUpdates(fixedTimeStep)
            }

            FrameConstants frame;
            frame.deltaTime = deltaTime;
            frame.totalTime = mFrameTimer.total();

            mRenderer->render(frame);
            ++msFrameIndex;

            calculateFPS(deltaTime);
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
            int fps = frameCount;
            float msPerFrame = 1000.0f / std::max(fps, 1);

            std::string fpsString = " | FPS " + std::to_string(fps) +
                " (" + std::to_string(msPerFrame) + "ms)";

            mWindow.appendWindowText(fpsString);

            fpsTimer = 0.0f;
            frameCount = 0;
        }
    }
}
