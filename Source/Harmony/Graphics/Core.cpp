#include "Core.h"

namespace graphics
{
    Core* Core::msInstance = nullptr;

    Core* Core::getInstance()
    {
        if (msInstance)
        {
            return msInstance;
        }

        msInstance = new Core();
        return msInstance;
    }
}
