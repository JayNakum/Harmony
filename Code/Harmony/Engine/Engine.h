#pragma once

#include "common.h"

#include "Window.h"
#include "Inputs.h"
#include "Timer.h"

namespace engine
{
    class Renderer;
}

namespace engine
{
    class Engine
    {
    public:
        Engine(windows::Window& mainWindow);

        static inline const uint64_t getFrameIndex() { return msFrameIndex; }

        void run();

        ~Engine();

    private:
        //void update(const float deltaTime);

        void calculateFPS(const float deltaTime);

    private:
        windows::Window& mWindow;
        windows::Inputs mInputs;

        static uint64_t msFrameIndex;
        Timer mFrameTimer;
        Renderer* mRenderer;
    };
}
