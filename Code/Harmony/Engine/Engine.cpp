#include "Engine.h"

namespace engine
{
    Engine::Engine(windows::Window& mainWindow)
        : mWindow(mainWindow),
        mInputs(mWindow.getHandle()),
        mRenderer(mWindow.getHandle(), mWindow.getDescription().width, mWindow.getDescription().height)
    {}

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
                mRenderer.resize(mWindow.getDescription().width, mWindow.getDescription().height);
            }

            static double accumulator = 0.0f;
            constexpr double fixedTimeStep = 1.0f / 60.0f;
            accumulator += static_cast<double>(deltaTime);

            while (accumulator >= fixedTimeStep)
            {
                accumulator -= fixedTimeStep;
                // physics update(fixedTimeStep)
            }

            mRenderer.render();

            calculateFPS(deltaTime);
        }
    }

    void Engine::calculateFPS(const float deltaTime)
    {
        static float fpsTimer = 0.0f;
        static int frameCount = 0;

        frameCount++;
        fpsTimer += deltaTime;

        if (fpsTimer >= 1.0f)
        {
            int fps = frameCount;
            float msPerFrame = 1000.0f / std::max(fps, 1);

            std::ostringstream oss;
            oss << " | FPS " << fps << " (" << msPerFrame << "ms)";
            mWindow.appendWindowText(oss.str());

            frameCount = 0;
            fpsTimer = 0.0f;
        }
    }
}
