#pragma once

#include "common.h"
#include "directx.h"

#include "Device.h"
#include "CommandQueue.h"

#include "SwapchainEnums.h"

#include "DescriptorHandle.h"
#include "Format.h"
#include "GPUTexture.h"

namespace graphics
{
    namespace dx12
    {
        /*
        * Wrapper over IDXGISwapChain.
        * With BACK_BUFFER_COUNT back buffers
        */
        class Swapchain
        {
        public:
            static constexpr unsigned int BACK_BUFFER_COUNT = 3;

        public:
            struct Description
            {
                std::string name;
                HWND windowHandle;
                RenderTarget::Description renderTargetDescription;
            };

            // Creates a swapchain based on the createDescription.
            // Needs an array of pre-allocated rtvHandles.
            Swapchain(Device& device, CommandQueue& queue, const Description& createDescription, std::array<DescriptorHandle, BACK_BUFFER_COUNT> rtvHandles);

            // Resize the swapchain with the new width and height.
            // Since the RTV's are reset, it requires an array of pre-allocated handles.
            void resize(Device& device, const unsigned int width, const unsigned int height, std::array<DescriptorHandle, BACK_BUFFER_COUNT> rtvHandles);

            // Returns the current buffer index.
            inline const unsigned int getCurrentBackBufferIndex() const { return mSwapChain->GetCurrentBackBufferIndex();; }

            // Returns the current render target.
            inline RenderTarget& getCurrentBackBuffer() { return mBackBuffers[getCurrentBackBufferIndex()]; }

            // Presents the swapchain and updates the current buffer index.
            void present(const unsigned int interval = 0);

            ~Swapchain();

        private:
            RenderTarget::Description mBackBufferDescription;
            std::array<RenderTarget, BACK_BUFFER_COUNT> mBackBuffers;
            Microsoft::WRL::ComPtr<IDXGISwapChain3> mSwapChain;
        };
    }
}
