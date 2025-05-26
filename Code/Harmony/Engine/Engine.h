#pragma once

#include "common.h"

#include "Window.h"
#include "Inputs.h"
#include "Timer.h"
#include "Graphics/Renderer.h"

namespace engine
{
    class Engine
    {
    public:
        Engine(windows::Window& mainWindow);

        void run();

        ~Engine() = default;

    private:
        void update(const float deltaTime);

        void calculateFPS(const float deltaTime);

    private:
        windows::Window& mWindow;
        windows::Inputs mInputs;

        Timer mFrameTimer;
        Renderer mRenderer;
    };
}
