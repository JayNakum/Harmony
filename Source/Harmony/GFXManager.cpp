#include "GFXManager.h"

namespace graphics
{
    Manager* Manager::msInstance = nullptr;
    Manager* Manager::getInstance()
    {
        if (msInstance)
        {
            return msInstance;
        }

        msInstance = new Manager();
        return msInstance;
    }
}
