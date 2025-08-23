#pragma once

#include "common.h"
#include "Log.h"
#include "directx.h"

#include "DescriptorHeapEnums.h"

namespace graphics
{
    /*
    * A wrapper over D3D12Device.
    */
    class Device
    {
    public:
        struct Configuration
        {
            bool useWarpDriver = false;
            bool requireRaytracing = false;

            Configuration() {}
        };

        static constexpr uint32_t vendorID_Nvidia = 0x10DE;
        static constexpr uint32_t vendorID_AMD = 0x1002;
        static constexpr uint32_t vendorID_Intel = 0x8086;

    public:
        // Creates and initializes an ID3D12Device as per the specified Configuration
        // By default, it searches for the adapter with the most memory.
        Device(const Configuration& config = Configuration{});

        // Releases the graphics device.
        // In debug mode, it also reports if any resources are currently in use.
        ~Device();

    public:
        static const uint32_t getVendorIdFromDevice(Device& device);
        inline const bool isDeviceNvidia() { return Device::getVendorIdFromDevice(*this) == vendorID_Nvidia; }
        inline const bool isDeviceAMD() { return Device::getVendorIdFromDevice(*this) == vendorID_AMD; }
        inline const bool isDeviceIntel() { return Device::getVendorIdFromDevice(*this) == vendorID_Intel; }

        inline const bool isGPUUploadHeapSupported() const { return mSupportsGPUUploadHeap; }

        const size_t getDescriptorHandleIncrementSize(const DescriptorHeapType type) const;

        // Returns the raw pointer of D3D12Device.
        inline const ID3D12Device* operator->() const { return mDevice.Get(); }
        inline ID3D12Device* operator->() { return mDevice.Get(); }

    private:
        void checkFeaturesSupport();

        static const bool isRaytracingSupported(ID3D12Device* device);

    private:
        Microsoft::WRL::ComPtr<ID3D12Device> mDevice;

    private:
        bool mTypedUAVLoadSupport_R11G11B10_FLOAT = false;
        bool mTypedUAVLoadSupport_R16G16B16A16_FLOAT = false;

        bool m4xMSAAState = false;
        unsigned int m4xMSAAQuality = 0;

        bool mSupportsGPUUploadHeap = false;
    };
}
