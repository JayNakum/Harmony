#pragma once

#include "common.h"
#include "directx.h"

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
            Swapchain(const Description& createDescription, std::array<DescriptorHandle, BACK_BUFFER_COUNT> rtvHandles);

            // Resize the swapchain with the new width and height.
            // Since the RTV's are reset, it requires an array of pre-allocated handles.
            void resize(const unsigned int width, const unsigned int height, std::array<DescriptorHandle, BACK_BUFFER_COUNT> rtvHandles);

            // Returns the current render target.
            inline RenderTarget& getCurrentBackBuffer() { return mBackBuffers[mCurrentBuffer]; }

            // Presents the swapchain and updates the current buffer index.
            void present(const unsigned int interval = 0);

            ~Swapchain();

        private:
            Microsoft::WRL::ComPtr<IDXGISwapChain3> mSwapChain;
            RenderTarget::Description mBackBufferDescription;
            std::array<RenderTarget, BACK_BUFFER_COUNT> mBackBuffers;
            unsigned int mCurrentBuffer = 0;
        };
    }
}
