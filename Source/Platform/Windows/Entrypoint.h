#pragma once

#include "common.h"
#include "win.h"

#include "HarmonyApplication.h"

/*
* A simple #define to ensure consistency.
* The createApplication() returns a new instance of any app.
* Which is then used inside platform specific entrypoint.cpp to run the app.
*/
#undef CREATE_APPLICATION
#define CREATE_APPLICATION(app_class) \
    harmony::Application* createApplication() \
    { \
        return new app_class(); \
    }


extern harmony::Application* createApplication();

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd)
{
#if defined(DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
        harmony::Application* application = createApplication();

        application->initialize();
        application->run();
        application->deinitialize();

        delete application;
    }
    catch (std::exception& e)
    {
        MessageBox(nullptr, utility::toWideString(e.what()).c_str(), L"Error", MB_OK);
        return -1;
    }

    return 0;
}
