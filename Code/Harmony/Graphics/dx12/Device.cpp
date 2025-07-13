#include "Device.h"

#include "types/DescriptorHeapType.h"

namespace graphics
{
    using namespace Microsoft::WRL;

    namespace dx12
    {
        Device::Device(const Configuration& config)
            : mDevice(nullptr)
        {
            HRESULT hResult = S_OK;
            bool useDebugLayers = false;
            ComPtr<ID3D12Device> pDevice;

#if defined(DEBUG)
            useDebugLayers = true;
#endif

            DWORD dxgiFactoryFlags = 0;

            if (useDebugLayers)
            {
                ComPtr<ID3D12Debug> debugInterface;
                hResult = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));

                if (SUCCEEDED(hResult))
                {
                    debugInterface->EnableDebugLayer();

                    uint32_t useGPUBasedValidation = true;
                    if (useGPUBasedValidation)
                    {
                        ComPtr<ID3D12Debug1> debugInterface1;
                        hResult = debugInterface->QueryInterface(IID_PPV_ARGS(&debugInterface1));
                        if (SUCCEEDED(hResult))
                        {
                            debugInterface1->SetEnableGPUBasedValidation(true);
                        }
                    }
                }
                else
                {
                    DEBUGLOG("WARNING", "Failed to enable D3D12 debug validation layer.");
                }

#if defined(DEBUG)
                ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
                if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
                {
                    dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

                    dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
                    dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

                    DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
                    {
                        80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
                    };
                    DXGI_INFO_QUEUE_FILTER filter = {};
                    filter.DenyList.NumIDs = _countof(hide);
                    filter.DenyList.pIDList = hide;
                    dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
                }
#endif
            }

            // Obtain the DXGI factory
            ComPtr<IDXGIFactory6> dxgiFactory;
            hResult = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory));
            HASSERT(SUCCEEDED(hResult), "Failed to create DXGI Factory.\n%s", utility::translateErrorCode(hResult));

            // Create the D3D graphics device
            ComPtr<IDXGIAdapter1> pAdapter;

            D3D12EnableExperimentalFeatures(0, nullptr, nullptr, nullptr);

            if (!config.useWarpDriver)
            {
                SIZE_T MaxSize = 0;

                for (uint32_t Idx = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapters1(Idx, &pAdapter); ++Idx)
                {
                    DXGI_ADAPTER_DESC1 desc;
                    pAdapter->GetDesc1(&desc);

                    // Is a software adapter?
                    if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                        continue;

                    // Create a D3D12 device
                    hResult = D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice));
                    if (FAILED(hResult))
                        continue;

                    // Does support DXR if required?
                    if (config.requireRaytracing && !isRaytracingSupported(pDevice.Get()))
                        continue;

                    // By default, search for the adapter with the most memory because that's usually the dGPU.
                    if (desc.DedicatedVideoMemory < MaxSize)
                        continue;

                    MaxSize = desc.DedicatedVideoMemory;

                    if (mDevice != nullptr)
                    {
                        mDevice->Release();
                    }

                    mDevice = pDevice.Detach();

                    DEBUGLOG("INFO", "Selected GPU : %ls(%u MB)", desc.Description, desc.DedicatedVideoMemory >> 20);
                }
            }

            if (config.requireRaytracing)
            {
                DEBUGLOG_IF(mDevice, "WARNING", "Unable to find a DXR-capable device.");
                BREAK_IF(mDevice);
            }

            if (mDevice == nullptr)
            {
                if (config.useWarpDriver)
                {
                    DEBUGLOG("INFO", "WARP software adapter requested.");
                }
                else
                {
                    DEBUGLOG("ERROR", "Failed to find a hardware adapter. Falling back to WARP.");
                    BREAK;
                }
                hResult = dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pAdapter));
                HASSERT(SUCCEEDED(hResult), "Failed to find warp adapter.\n%s", utility::translateErrorCode(hResult));

                hResult = D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice));
                HASSERT(SUCCEEDED(hResult), "Failed to create warp device.\n%s", utility::translateErrorCode(hResult));
                mDevice = pDevice.Detach();
            }
#if !defined(RELEASE)
            else
            {
                bool sysDeveloperModeEnabled = false;

                // Look in the Windows Registry to determine if Developer Mode is enabled
                HKEY hKey;
                LSTATUS result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AppModelUnlock", 0, KEY_READ, &hKey);
                if (result == ERROR_SUCCESS)
                {
                    DWORD keyValue = 0, keySize = sizeof(DWORD);
                    result = RegQueryValueEx(hKey, L"AllowDevelopmentWithoutDevLicense", 0, NULL, (byte*)&keyValue, &keySize);
                    if (result == ERROR_SUCCESS && keyValue == 1)
                        sysDeveloperModeEnabled = true;
                    RegCloseKey(hKey);
                }

                DEBUGLOG_ONCE_IF(!sysDeveloperModeEnabled, "WARNING", "Enable Developer Mode on Windows 10 to get consistent profiling results.");

                // Prevent the GPU from over-clocking or under-clocking to get consistent timings
                if (sysDeveloperModeEnabled)
                    mDevice->SetStablePowerState(TRUE);
            }
#endif

#if defined(DEBUG)
            ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
            hResult = mDevice->QueryInterface(IID_PPV_ARGS(&infoQueue));
            if (SUCCEEDED(hResult))
            {
                // Suppress whole categories of messages
                //D3D12_MESSAGE_CATEGORY Categories[] = {};

                // Suppress messages based on their severity level
                D3D12_MESSAGE_SEVERITY severities[] =
                {
                    D3D12_MESSAGE_SEVERITY_INFO
                };

                // Suppress individual messages by their ID
                D3D12_MESSAGE_ID denyIds[] =
                {
                    // This occurs when there are uninitialized descriptors in a descriptor table, even when a shader does not access the missing descriptors.
                    D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,

                    // Triggered when a shader does not export all color components of a render target,
                    // such as when only writing RGB to an R10G10B10A2 buffer, ignoring alpha.
                    D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_PS_OUTPUT_RT_OUTPUT_MISMATCH,

                    // This occurs when a descriptor table is unbound even when a shader does not access the missing descriptors.
                    // This is common with a root signature shared between disparate shaders that don't all need the same types of resources.
                    D3D12_MESSAGE_ID_COMMAND_LIST_DESCRIPTOR_TABLE_NOT_SET,

                    // RESOURCE_BARRIER_DUPLICATE_SUBRESOURCE_TRANSITIONS
                    D3D12_MESSAGE_ID_RESOURCE_BARRIER_DUPLICATE_SUBRESOURCE_TRANSITIONS,

                    // Suppress errors from calling ResolveQueryData with timestamps that weren't requested on a given frame.
                    D3D12_MESSAGE_ID_RESOLVE_QUERY_INVALID_QUERY_STATE,

                    // Ignoring InitialState D3D12_RESOURCE_STATE_COPY_DEST. Buffers are effectively created in state D3D12_RESOURCE_STATE_COMMON.
                    D3D12_MESSAGE_ID_CREATERESOURCE_STATE_IGNORED,

                    // Ignoring The clear values do not match those passed to resource creation. The clear operation is typically slower as a result; but will still clear to the desired value.
                    D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE
                };

                D3D12_INFO_QUEUE_FILTER newFilter = {};
                //newFilter.DenyList.NumCategories = _countof(Categories);
                //newFilter.DenyList.pCategoryList = Categories;
                newFilter.DenyList.NumSeverities = _countof(severities);
                newFilter.DenyList.pSeverityList = severities;
                newFilter.DenyList.NumIDs = _countof(denyIds);
                newFilter.DenyList.pIDList = denyIds;

                infoQueue->PushStorageFilter(&newFilter);
                infoQueue->Release();
            }
#endif

            checkFeaturesSupport();
        }

        Device::~Device()
        {
#if defined(DEBUG)
            Microsoft::WRL::ComPtr<ID3D12DebugDevice> debugDevice;
            HRESULT hResult = mDevice->QueryInterface(IID_PPV_ARGS(&debugDevice));
            if (SUCCEEDED(hResult))
            {
                //debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY| D3D12_RLDO_IGNORE_INTERNAL);
                debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
            }
#endif
        }

        const uint32_t Device::getVendorIdFromDevice(Device& device)
        {
            HRESULT hResult = S_OK;
            LUID luid = device->GetAdapterLuid();

            // Obtain the DXGI factory
            Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
            hResult = CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory));
            HASSERT(SUCCEEDED(hResult), "Failed to create DXGI Factory.\n%s", utility::translateErrorCode(hResult));

            Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter;

            hResult = dxgiFactory->EnumAdapterByLuid(luid, IID_PPV_ARGS(&pAdapter));
            if (SUCCEEDED(hResult))
            {
                DXGI_ADAPTER_DESC1 desc;
                if (SUCCEEDED(pAdapter->GetDesc1(&desc)))
                {
                    return desc.VendorId;
                }
            }

            return 0;
        }

        const size_t Device::getDescriptorHandleIncrementSize(const DescriptorHeapType type) const
        {
            return mDevice->GetDescriptorHandleIncrementSize(toD3DDescriptorHeapType(type));
        }

        void Device::checkFeaturesSupport()
        {
            {// D3D12_FEATURE_D3D12_OPTIONS
                // To support read-modify-write operations on UAVs during post processing,
                // need to either have the hardware do typed UAV loads of R11G11B10_FLOAT
                // or we need to manually decode an R32_UINT representation of the same buffer.
                // This code determines if we get the hardware load support.
                D3D12_FEATURE_DATA_D3D12_OPTIONS featureData = {};
                if (SUCCEEDED(mDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &featureData, sizeof(featureData))))
                {
                    if (featureData.TypedUAVLoadAdditionalFormats)
                    {
                        D3D12_FEATURE_DATA_FORMAT_SUPPORT support =
                        {
                            DXGI_FORMAT_R11G11B10_FLOAT, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE
                        };

                        if (SUCCEEDED(mDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &support, sizeof(support))) &&
                            (support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
                        {
                            mTypedUAVLoadSupport_R11G11B10_FLOAT = true;
                        }

                        support.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

                        if (SUCCEEDED(mDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &support, sizeof(support))) &&
                            (support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
                        {
                            mTypedUAVLoadSupport_R16G16B16A16_FLOAT = true;
                        }
                    }
                }
            }

            {// D3D12_FEATURE_D3D12_OPTIONS16
                D3D12_FEATURE_DATA_D3D12_OPTIONS16 featureData = {};
                if (SUCCEEDED(mDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS16, &featureData, sizeof(featureData))))
                {
                    mSupportsGPUUploadHeap = featureData.GPUUploadHeapSupported;
                }
            }
        }

        const bool Device::isRaytracingSupported(ID3D12Device* device)
        {
            D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupport = {};

            HRESULT hResult = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureSupport, sizeof(featureSupport));
            if (FAILED(hResult))
                return false;

            return featureSupport.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
        }
    }
}
