#pragma once

#include "common.h"
#include "Log.h"

#include "memory/RingAllocator.h"

#include "dx12/Device.h"
#include "dx12/CommandQueue.h"
#include "dx12/DescriptorHeap.h"

#include "CommandContext.h"

#include "imgui.h"

namespace engine
{
    class DebugUI
    {
    public:
        static inline void Register(std::function<void()> callback) { msRenderCallbacks.push_back(std::move(callback)); }

        DebugUI(const HWND windowHandle, graphics::Device& device, graphics::CommandQueue& graphicsQueue);

        void render(graphics::GraphicContext& context);

        ~DebugUI();

        class DescriptorHeapWithAllocator
        {
            static constexpr size_t MEMORY_BUDGET = memory::KB(2);

        public:
            DescriptorHeapWithAllocator(graphics::Device& device);

            inline graphics::DescriptorHeap& getHeap() { return mDescriptorHeap; }

            graphics::DescriptorHandle allocate();

            ~DescriptorHeapWithAllocator() = default;

        private:
            const size_t mAllocationSize;
            memory::RingAllocator mAllocator;
            graphics::DescriptorHeap mDescriptorHeap;
        };

    private:
        DescriptorHeapWithAllocator mShaderViewsDescriptorHeapGPU;
        static std::vector<std::function<void()>> msRenderCallbacks;
    };
}
