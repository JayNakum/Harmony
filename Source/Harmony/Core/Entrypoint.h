#pragma once

#include "HarmonyApplication.h"

#undef CREATE_APPLICATION
#define CREATE_APPLICATION(app_class) \
    harmony::Application* createApplication() \
    { \
        return new app_class(); \
    }
