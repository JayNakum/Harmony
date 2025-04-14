#pragma once

#include "common.h"
#include "dx12/Device.h"

#include "CommandListManager.h"
#include "CommandContextPool.h"

#include "Utility.h"

namespace graphics
{
    /*
    * A singleton class that manages the entire graphics system.
    * All the other systems will access anything inside the graphics project via Core.
    */
    class Core
    {
    private:
        Core();
    public:
        static Core& getInstance();
        ~Core();

    public:
        inline dx12::Device& getDevice() { return mDevice; }
        inline CommandListManager& getCommandListManager() { return mCommandListManager; }
        inline CommandContextPool& getCommandContextPool() { return mCommandContextPool; }

    private:
        dx12::Device mDevice;
        CommandListManager mCommandListManager;
        CommandContextPool mCommandContextPool;
    };
}

/*
* Bunch of inline helper functions.
* Keeping these outside of any namespace with Gfx prefix to make the code a bit more clean.
*/

inline graphics::dx12::Device& GfxDevice()
{
    return graphics::Core::getInstance()->getDevice();
}

inline graphics::CommandListManager& GfxCommandListManager()
{
    return graphics::Core::getInstance()->getCommandListManager();
}

inline graphics::CommandContextPool& GfxCommandContextPool()
{
    return graphics::Core::getInstance()->getCommandContextPool();
}
