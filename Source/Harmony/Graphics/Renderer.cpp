#include "Renderer.h"

namespace graphics
{
    Renderer* Renderer::msInstance = nullptr;
    Renderer* Renderer::getInstance()
    {
        if (msInstance)
        {
            return msInstance;
        }

        msInstance = new Renderer();
        return msInstance;
    }
}
