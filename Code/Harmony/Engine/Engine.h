#pragma once

#include "common.h"
#include "Log.h"

#include "Window.h"
#include "Inputs.h"
#include "Timer.h"

namespace engine
{
    // Forward declaration and heap allocation is needed to prevent sandbox from accessing direct graphics calls.
    class Renderer;
}

namespace engine
{
    /*
    * Core engine class.
    * Creates and manages main game loop, main window and graphics renderer.
    */
    class Engine
    {
    public:
        Engine(windows::Window& mainWindow);

        inline static const uint64_t getFrameIndex() { return msFrameIndex; }

        void run();

        ~Engine();

    private:
        //void update(const float deltaTime);

        void calculateFPS(const float deltaTime);

    private:
        windows::Window& mWindow;
        windows::Inputs mInputs;
        Renderer* mRenderer;

        static uint64_t msFrameIndex;
        Timer mFrameTimer;
    };
}
