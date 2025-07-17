#pragma once

#include "common.h"
#include "Log.h"
#include "directx.h"

#include "Device.h"
#include "CommandQueue.h"

#include "DescriptorHandle.h"
#include "Format.h"
#include "Texture.h"

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
                unsigned int width;
                unsigned int height;
            };

            // Creates a swapchain based on the createDescription.
            // Needs an array of pre-allocated rtvHandles.
            Swapchain(Device& device, CommandQueue& queue, const Description& createDescription, std::array<DescriptorHandle, BACK_BUFFER_COUNT> rtvHandles);

            const bool isHDRSupported() const { return mHDRSupport; }

            // Resize the swapchain with the new width and height.
            // Since the RTV's are reset, it requires an array of pre-allocated handles.
            // Also checks for HDR support.
            void resize(Device& device, const unsigned int width, const unsigned int height, std::array<DescriptorHandle, BACK_BUFFER_COUNT> rtvHandles);

            // Returns the current buffer index.
            inline const unsigned int getCurrentBackBufferIndex() const { return mSwapChain->GetCurrentBackBufferIndex(); }

            inline Texture& getBackBufferAt(const unsigned int i) { return mBackBuffers.at(i); }
            inline DescriptorHandle getBackBufferViewAt(const unsigned int i) { return mRtvHandles.at(i); }

            // Returns the current render target.
            inline Texture& getCurrentBackBuffer() { return mBackBuffers[getCurrentBackBufferIndex()]; }

            // Returns the current render target.
            inline DescriptorHandle getCurrentBackBufferView() { return mRtvHandles[getCurrentBackBufferIndex()]; }

            // Presents the swapchain.
            void present(const unsigned int interval = 0);

            ~Swapchain() = default;

        private:
            void checkHDRSupport();

        private:
            Microsoft::WRL::ComPtr<IDXGISwapChain3> mSwapChain;
            Texture::Description mRenderTargetDescription;
            std::array<Texture, BACK_BUFFER_COUNT> mBackBuffers;
            std::array<DescriptorHandle, BACK_BUFFER_COUNT> mRtvHandles;

            bool mTearingSupport;
            bool mHDRSupport;
        };
    }
}
