#pragma once

#include "HarmonyApplication.h"
#include "win.h"

extern harmony::Application* createApplication();

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPWSTR /*lpCmdLine*/, _In_ int nCmdShow)
{
    harmony::Application* application = createApplication();

    application->initialize();
    application->run();
    application->deinitialize();

    delete application;

    return 0;
}
