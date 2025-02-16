#pragma once

#include "dx12/Device.h"
#include "CommandAllocatorManager.h"
#include "CommandListManager.h"
#include "DescriptorHeapManager.h"
#include "CommandContextManager.h"

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
        inline CommandAllocatorManager& getCommandAllocatorManager() { return mCommandAllocatorManager; }
        inline CommandListManager& getCommandListManager() { return mCommandListManager; }
        inline DescriptorHeapManager& getDescriptorHeapManager() { return mDescriptorHeapManager; }
        inline CommandContextManager& getCommandContextManager() { return mCommandContextManager; }

    private:
        dx12::Device mDevice;
        CommandAllocatorManager mCommandAllocatorManager;
        CommandListManager mCommandListManager;
        DescriptorHeapManager mDescriptorHeapManager;
        CommandContextManager mCommandContextManager;
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

inline graphics::CommandAllocatorManager& GfxCommandAllocatorManager()
{
    return graphics::Renderer::getInstance()->getCommandAllocatorManager();
}

inline graphics::CommandListManager& GfxCommandListManager()
{
    return graphics::Renderer::getInstance()->getCommandListManager();
}

inline graphics::DescriptorHeapManager& GfxDescriptorHeapManager()
{
    return graphics::Renderer::getInstance()->getDescriptorHeapManager();
}

inline graphics::CommandContextManager& GfxCommandContextManager()
{
    return graphics::Renderer::getInstance()->getCommandContextManager();
}
