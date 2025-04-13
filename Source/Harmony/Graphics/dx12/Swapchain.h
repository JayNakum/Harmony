#pragma once

#include "common.h"
#include "directx.h"

#include "SwapchainEnums.h"

#include "DescriptorHandle.h"
#include "GPUTexture.h"
#include "Format.h"

namespace graphics
{
    namespace dx12
    {
        class Swapchain
        {
            static constexpr unsigned int BACK_BUFFER_COUNT = 3;
        public:
            struct Description
            {
                std::string name;
                unsigned int width;
                unsigned int height;
                HWND windowHandle;

                RenderTarget::Description renderTargetDescription;
            };

            Swapchain(const Description& createDescription, std::array<DescriptorHandle, BACK_BUFFER_COUNT> rtvHandles);

            void resize(unsigned int width, unsigned int height, std::array<DescriptorHandle, BACK_BUFFER_COUNT> rtvHandles);

            inline RenderTarget& getCurrentBackBuffer() { return mBackBuffers[mCurrentBuffer]; }
            void present(unsigned int interval = 0);

            ~Swapchain();

        private:
            Microsoft::WRL::ComPtr<IDXGISwapChain3> mSwapChain3;
            RenderTarget::Description mBackBufferDescription;
            std::array<RenderTarget, BACK_BUFFER_COUNT> mBackBuffers;
            unsigned int mCurrentBuffer = 0;
        };
    }
}
