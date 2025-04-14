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
        static Core* msInstance;
        Core() = default;

    public:
        static Core* getInstance();

    public:
        inline const dx12::Device getDevice() const { return msDevice; }
        inline CommandListManager& getCommandListManager() { return msCommandListManager; }

    private:
        static dx12::Device msDevice;
        static CommandListManager msCommandListManager;
        static std::array<dx12::DescriptorHeap, descriptorHeapTypeCount> msActiveDescriptorHeap;
    };
}

/*
* Bunch of inline helper functions.
* Keeping these outside of any namespace with Gfx prefix to make the code a bit more clean.
*/

inline graphics::Core* GfxCore()
{
    return graphics::Core::getInstance();
}

inline graphics::dx12::Device GfxDevice()
{
    return graphics::Core::getInstance()->getDevice();
}

inline graphics::CommandListManager& GfxCommandListManager()
{
    return graphics::Core::getInstance()->getCommandListManager();
}
