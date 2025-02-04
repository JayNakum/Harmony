#pragma once

#include "directx.h"

namespace graphics
{
    namespace dx12
    {
        /*
        * A wrapper over D3D12Device.
        */
        class Device
        {
            friend class Manager;

        public:
            struct Configuration
            {
                bool useWarpDriver = false;
                bool requireRaytracing = false;
            };

            static constexpr uint32_t vendorID_Nvidia = 0x10DE;
            static constexpr uint32_t vendorID_AMD = 0x1002;
            static constexpr uint32_t vendorID_Intel = 0x8086;

        private:
            /*
            * Creates and initializes an ID3D12Device as per the specified Configuration
            * By default, it searches for the adapter with the most memory.
            */
            Device(const Configuration& config);

        public:
            /*
            * Releases the graphics device.
            * In debug mode, it also reports if any resources are currently in use.
            */
            ~Device();

            /*
            * Returns the raw pointer of D3D12Device.
            */
            ID3D12Device* operator->() const
            {
                return mDevice.Get();
            }

            inline bool isDeviceNvidia() const { return Device::getVendorIdFromDevice(mDevice.Get()) == vendorID_Nvidia; }
            inline bool isDeviceAMD() const { return Device::getVendorIdFromDevice(mDevice.Get()) == vendorID_AMD; }
            inline bool isDeviceIntel() const { return Device::getVendorIdFromDevice(mDevice.Get()) == vendorID_Intel; }

        public:
            static uint32_t getVendorIdFromDevice(ID3D12Device* pDevice);

            /*
            * Check adapter support for DirectX Raytracing.
            */
            static bool isRaytracingSupported(ID3D12Device* pDevice);

        private:
            Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
            bool mTypedUAVLoadSupport_R11G11B10_FLOAT = false;
            bool mTypedUAVLoadSupport_R16G16B16A16_FLOAT = false;
        };
    }
}
