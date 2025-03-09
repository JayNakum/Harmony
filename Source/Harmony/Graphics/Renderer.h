#pragma once

#include "dx12/Device.h"

#include "CommandListManager.h"
#include "CommandContextPool.h"
#include "CommandContext.h"

#include "DescriptorHeapPool.h"

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
        Renderer();

    public:
        static Renderer* getInstance();

    public:
        inline const dx12::Device getDevice() const { return mDevice; }
        inline CommandListManager& getCommandListManager() { return mCommandListManager; }
        inline CommandContextPool& getCommandContextPool() { return mCommandContextPool; }
        inline dx12::DescriptorHandle allocateDescriptor(const DescriptorHeapType type, const size_t count = 1) { return mDescriptorHeapPool.requestDescriptorHeap(type).allocate(count); }

    private:
        dx12::Device mDevice;
        CommandListManager mCommandListManager;
        CommandContextPool mCommandContextPool;
        DescriptorHeapPool mDescriptorHeapPool;
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

inline graphics::CommandContextPool& GfxCommandContextPool()
{
    return graphics::Renderer::getInstance()->getCommandContextPool();
}

inline graphics::dx12::CommandContext& GfxContextBegin(const std::string& name = "DEFAULT")
{
    graphics::dx12::CommandContext& newContext = graphics::Renderer::getInstance()->getCommandContextPool().requestContext(graphics::CommandListType::Direct);
    newContext.setID(name);
    return newContext;
}

inline graphics::dx12::DescriptorHandle GfxAllocateDescriptor(const graphics::DescriptorHeapType type = graphics::DescriptorHeapType::View, const size_t count = 1)
{
    graphics::Renderer::getInstance()->allocateDescriptor(type, count);
}
