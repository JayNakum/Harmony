#include "Core.h"

namespace graphics
{
    dx12::Device Core::mDevice;
    Core& Core::getInstance()
    {
        static Core instance;
        return instance;
    }
}
