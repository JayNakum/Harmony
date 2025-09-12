#pragma once

#include "common.h"
#include "Log.h"

#include "memory/RingAllocator.h"

#include "dx12/Device.h"
#include "dx12/CommandQueue.h"
#include "dx12/DescriptorHeap.h"

#include "CommandContext.h"

namespace engine
{
    class DebugUI
    {
    public:
        static inline void Register(std::function<void()> callback) { msRenderCallbacks.push_back(std::move(callback)); }

        DebugUI(const HWND windowHandle, graphics::Device& device, graphics::CommandQueue& graphicsQueue);

        void render(graphics::GraphicContext& context);

        ~DebugUI();

    private:
		graphics::DescriptorHeap mShaderViewsDescriptorHeapGPU;
		memory::RingAllocator mDescriptorsAllocator;

        static std::vector<std::function<void()>> msRenderCallbacks;
    };
}
