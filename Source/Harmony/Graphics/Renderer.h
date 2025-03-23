#pragma once

#include "dx12/Device.h"

#include "CommandListManager.h"
#include "CommandContextPool.h"
#include "CommandContext.h"

#include "DescriptorHeapPool.h"

#include "Utility.h"

namespace graphics
{
    /*
    * A singleton class that manages the entire graphics system.
    * All the other systems will access anything inside the graphics project via Renderer.
    */
    class Renderer
    {
    private:
        static Renderer* msInstance;
        Renderer() = default;

    public:
        static Renderer* getInstance();

    public:
        inline const dx12::Device getDevice() const { return msDevice; }
        inline CommandListManager& getCommandListManager() { return msCommandListManager; }


    private:
        static dx12::Device msDevice;
        static CommandListManager msCommandListManager;
        static std::array<dx12::DescriptorHeap, descriptorHeapTypeCount> msActiveDescriptorHeap;
        static std::array<DescriptorHeapPool, descriptorHeapTypeCount> msDescriptorHeapPool;
    };
}

/*
* Bunch of inline helper functions.
* Keeping these outside of any namespace with Gfx prefix to make the code a bit more clean.
*/

inline graphics::Renderer* GfxRenderer()
{
    return graphics::Renderer::getInstance();
}

inline graphics::dx12::Device GfxDevice()
{
    return graphics::Renderer::getInstance()->getDevice();
}

inline graphics::CommandListManager& GfxCommandListManager()
{
    return graphics::Renderer::getInstance()->getCommandListManager();
}

inline graphics::dx12::DescriptorHandle GfxAllocateDescriptor(const graphics::DescriptorHeapType type = graphics::DescriptorHeapType::View, const size_t count = 1)
{
    return graphics::Renderer::getInstance()->allocateDescriptor(type, count);
}
