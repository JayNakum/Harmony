#pragma once

#include "common.h"

#include "Maths.h"

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
        static inline void Register(std::function<void()> callback)
        {
            std::lock_guard<std::mutex> scopedLockGuard(msCallbackMutex);
            msRenderCallbacks.push_back(std::move(callback));
        }

        static void matrix(const char* label, mat4f& matrix);
        static void matrixInput(const char* label, mat4f& matrix);

#if !defined(HRELEASE)
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
#else
    public:
        DebugUI(const HWND windowHandle, graphics::Device& device, graphics::CommandQueue& graphicsQueue) {}

        void render(graphics::GraphicContext& context) {}

        ~DebugUI() { msRenderCallbacks.clear(); }
#endif
    private:
        static std::mutex msCallbackMutex;
        static std::vector<std::function<void()>> msRenderCallbacks;
    };
}
