#include "Device.h"

#include "types/DescriptorHeapType.h"

namespace graphics
{
    using namespace Microsoft::WRL;

    Device::Device(const Configuration& config)
        : mDevice(nullptr)
    {
        HRESULT hResult = S_OK;
        bool useDebugLayers = false;
        ComPtr<ID3D12Device> pDevice;

#if defined(HDEBUG)
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
                HASSERT_SOFT(false, "Failed to enable D3D12 debug validation layer.");
            }

#if defined(HDEBUG)
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
        HASSERT(SUCCEEDED(hResult), "Failed to create DXGI Factory.\n", utility::translateErrorCode(hResult).c_str());

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

                HLOG(INFO, "Selected GPU : ", utility::toUTF8(desc.Description), "(", desc.DedicatedVideoMemory >> 20, " MB)");
            }
        }

        if (config.requireRaytracing)
        {
            HASSERT_SOFT(mDevice, "Unable to find a DXR-capable device.");
        }

        if (mDevice == nullptr)
        {
            if (config.useWarpDriver)
            {
                HLOG(INFO, "WARP software adapter requested.");
            }
            else
            {
                HASSERT_SOFT(false, "Failed to find a hardware adapter. Falling back to WARP.");
            }

            hResult = dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pAdapter));
            HASSERT(SUCCEEDED(hResult), "Failed to find warp adapter.\n", utility::translateErrorCode(hResult).c_str());

            hResult = D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice));
            HASSERT(SUCCEEDED(hResult), "Failed to create warp device.\n", utility::translateErrorCode(hResult).c_str());
            mDevice = pDevice.Detach();
        }
#if !defined(HRELEASE)
        else
        {
            bool sysDeveloperModeEnabled = false;

            // Look in the Windows Registry to determine if Developer Mode is enabled
            HKEY hKey;
            LSTATUS result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AppModelUnlock", 0, KEY_READ, &hKey);
            if (result == ERROR_SUCCESS)
            {
                DWORD keyValue = 0, keySize = sizeof(DWORD);
                result = RegQueryValueEx(hKey, L"AllowDevelopmentWithoutDevLicense", nullptr, nullptr, reinterpret_cast<byte*>(&keyValue), &keySize);
                if (result == ERROR_SUCCESS && keyValue == 1)
                {
                    sysDeveloperModeEnabled = true;
                }
                RegCloseKey(hKey);
            }

            HASSERT_SOFT(sysDeveloperModeEnabled, "Enable Developer Mode on Windows 10 to get consistent profiling results.");

            // Prevent the GPU from over-clocking or under-clocking to get consistent timings
            if (sysDeveloperModeEnabled)
            {
                mDevice->SetStablePowerState(TRUE);
            }
        }
#endif

#if defined(HDEBUG)
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
        HDEBUG_ONLY(setupDRED());
    }

    Device::~Device()
    {
#if defined(HDEBUG)
        Microsoft::WRL::ComPtr<ID3D12DebugDevice> debugDevice;
        HRESULT hResult = mDevice->QueryInterface(IID_PPV_ARGS(&debugDevice));
        if (SUCCEEDED(hResult))
        {
            //debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY| D3D12_RLDO_IGNORE_INTERNAL);
            debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
        }
#endif
    }

    uint32_t Device::getVendorIdFromDevice(Device& device)
    {
        HRESULT hResult = S_OK;
        LUID luid = device->GetAdapterLuid();

        // Obtain the DXGI factory
        Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
        hResult = CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory));
        HASSERT(SUCCEEDED(hResult), "Failed to create DXGI Factory.\n", utility::translateErrorCode(hResult).c_str());

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

    size_t Device::getDescriptorHandleIncrementSize(const DescriptorHeapType type) const
    {
        return mDevice->GetDescriptorHandleIncrementSize(dx12::toD3DDescriptorHeapType(type));
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

        {// D3D12_FEATURE_DATA_D3D12_OPTIONS3
            D3D12_FEATURE_DATA_D3D12_OPTIONS3 featureData = {};
            if (SUCCEEDED(mDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS3, &featureData, sizeof(featureData))))
            {
                mSupportsCopyQueueTimestampQueries = featureData.CopyQueueTimestampQueriesSupported;
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

    bool Device::isRaytracingSupported(ID3D12Device* device)
    {
        D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupport = {};

        HRESULT hResult = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureSupport, sizeof(featureSupport));
        if (FAILED(hResult))
            return false;

        return featureSupport.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
    }

#if defined(HDEBUG)
    static std::string toString(const D3D12_AUTO_BREADCRUMB_OP operation)
    {
        switch (operation)
        {
        default:
            return "";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_SETMARKER:
            return "SETMARKER";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_BEGINEVENT:
            return "BEGINEVENT";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_ENDEVENT:
            return "ENDEVENT";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_DRAWINSTANCED:
            return "DRAWINSTANCED";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_DRAWINDEXEDINSTANCED:
            return "DRAWINDEXEDINSTANCED";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_EXECUTEINDIRECT:
            return "EXECUTEINDIRECT";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_DISPATCH:
            return "DISPATCH";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_COPYBUFFERREGION:
            return "COPYBUFFERREGION";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_COPYTEXTUREREGION:
            return "COPYTEXTUREREGION";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_COPYRESOURCE:
            return "COPYRESOURCE";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_COPYTILES:
            return "COPYTILES";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_RESOLVESUBRESOURCE:
            return "RESOLVESUBRESOURCE";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_CLEARRENDERTARGETVIEW:
            return "CLEARRENDERTARGETVIEW";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_CLEARUNORDEREDACCESSVIEW:
            return "CLEARUNORDEREDACCESSVIEW";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_CLEARDEPTHSTENCILVIEW:
            return "CLEARDEPTHSTENCILVIEW";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_RESOURCEBARRIER:
            return "RESOURCEBARRIER";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_EXECUTEBUNDLE:
            return "EXECUTEBUNDLE";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_PRESENT:
            return "PRESENT";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_RESOLVEQUERYDATA:
            return "RESOLVEQUERYDATA";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_BEGINSUBMISSION:
            return "BEGINSUBMISSION";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_ENDSUBMISSION:
            return "ENDSUBMISSION";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME:
            return "DECODEFRAME";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_PROCESSFRAMES:
            return "PROCESSFRAMES";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_ATOMICCOPYBUFFERUINT:
            return "ATOMICCOPYBUFFERUINT";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_ATOMICCOPYBUFFERUINT64:
            return "ATOMICCOPYBUFFERUINT64";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_RESOLVESUBRESOURCEREGION:
            return "RESOLVESUBRESOURCEREGION";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_WRITEBUFFERIMMEDIATE:
            return "WRITEBUFFERIMMEDIATE";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME1:
            return "DECODEFRAME1";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_SETPROTECTEDRESOURCESESSION:
            return "SETPROTECTEDRESOURCESESSION";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME2:
            return "DECODEFRAME2";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_PROCESSFRAMES1:
            return "PROCESSFRAMES1";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_BUILDRAYTRACINGACCELERATIONSTRUCTURE:
            return "BUILDRAYTRACINGACCELERATIONSTRUCTURE";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_EMITRAYTRACINGACCELERATIONSTRUCTUREPOSTBUILDINFO:
            return "EMITRAYTRACINGACCELERATIONSTRUCTUREPOSTBUILDINFO";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_COPYRAYTRACINGACCELERATIONSTRUCTURE:
            return "COPYRAYTRACINGACCELERATIONSTRUCTURE";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_DISPATCHRAYS:
            return "DISPATCHRAYS";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_INITIALIZEMETACOMMAND:
            return "INITIALIZEMETACOMMAND";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_EXECUTEMETACOMMAND:
            return "EXECUTEMETACOMMAND";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_ESTIMATEMOTION:
            return "ESTIMATEMOTION";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_RESOLVEMOTIONVECTORHEAP:
            return "RESOLVEMOTIONVECTORHEAP";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_SETPIPELINESTATE1:
            return "SETPIPELINESTATE1";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_INITIALIZEEXTENSIONCOMMAND:
            return "INITIALIZEEXTENSIONCOMMAND";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_EXECUTEEXTENSIONCOMMAND:
            return "EXECUTEEXTENSIONCOMMAND";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_DISPATCHMESH:
            return "DISPATCHMESH";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_ENCODEFRAME:
            return "ENCODEFRAME";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_RESOLVEENCODEROUTPUTMETADATA:
            return "RESOLVEENCODEROUTPUTMETADATA";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_BARRIER:
            return "BARRIER";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_BEGIN_COMMAND_LIST:
            return "BEGIN_COMMAND_LIST";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_DISPATCHGRAPH:
            return "DISPATCHGRAPH";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_SETPROGRAM:
            return "SETPROGRAM";
        case D3D12_AUTO_BREADCRUMB_OP::D3D12_AUTO_BREADCRUMB_OP_PROCESSFRAMES2:
            return "PROCESSFRAMES2";
        }
    }

    static std::string toString(const D3D12_DRED_ALLOCATION_TYPE allocationType)
    {
        switch (allocationType)
        {
        default:
            return "";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_COMMAND_QUEUE:
            return "COMMAND_QUEUE";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_COMMAND_ALLOCATOR:
            return "COMMAND_ALLOCATOR";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_PIPELINE_STATE:
            return "PIPELINE_STATE";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_COMMAND_LIST:
            return "COMMAND_LIST";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_FENCE:
            return "FENCE";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_DESCRIPTOR_HEAP:
            return "DESCRIPTOR_HEAP";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_HEAP:
            return "HEAP";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_QUERY_HEAP:
            return "QUERY_HEAP";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_COMMAND_SIGNATURE:
            return "COMMAND_SIGNATURE";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_PIPELINE_LIBRARY:
            return "PIPELINE_LIBRARY";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_VIDEO_DECODER:
            return "VIDEO_DECODER";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_VIDEO_PROCESSOR:
            return "VIDEO_PROCESSOR";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_RESOURCE:
            return "RESOURCE";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_PASS:
            return "PASS";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_CRYPTOSESSION:
            return "CRYPTOSESSION";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_CRYPTOSESSIONPOLICY:
            return "CRYPTOSESSIONPOLICY";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_PROTECTEDRESOURCESESSION:
            return "PROTECTEDRESOURCESESSION";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_VIDEO_DECODER_HEAP:
            return "VIDEO_DECODER_HEAP";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_COMMAND_POOL:
            return "COMMAND_POOL";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_COMMAND_RECORDER:
            return "COMMAND_RECORDER";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_STATE_OBJECT:
            return "STATE_OBJECT";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_METACOMMAND:
            return "METACOMMAND";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_SCHEDULINGGROUP:
            return "SCHEDULINGGROUP";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_VIDEO_MOTION_ESTIMATOR:
            return "VIDEO_MOTION_ESTIMATOR";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_VIDEO_MOTION_VECTOR_HEAP:
            return "VIDEO_MOTION_VECTOR_HEAP";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_VIDEO_EXTENSION_COMMAND:
            return "VIDEO_EXTENSION_COMMAND";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_VIDEO_ENCODER:
            return "VIDEO_ENCODER";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_VIDEO_ENCODER_HEAP:
            return "VIDEO_ENCODER_HEAP";
        case D3D12_DRED_ALLOCATION_TYPE::D3D12_DRED_ALLOCATION_TYPE_INVALID:
            return "INVALID";
        }
    }

    void Device::setupDRED()
    {
        Microsoft::WRL::ComPtr<ID3D12DeviceRemovedExtendedDataSettings> dredSettings;
        HRESULT hResult = D3D12GetDebugInterface(IID_PPV_ARGS(&dredSettings));
        HASSERT(SUCCEEDED(hResult), "Failed to setup Device Removed Extended Data.\n", utility::translateErrorCode(hResult).c_str());

        // Turn on auto-breadcrumbs and page fault reporting.
        dredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
        dredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
    }
#endif

    void Device::getDeviceRemovedReason() const
    {
#if defined(HDEBUG)
        HRESULT hResult = mDevice->GetDeviceRemovedReason();
        if (hResult == S_OK)
        {
            return;
        }
        HLOG(ERROR, "Device Removed.\n", utility::translateErrorCode(hResult).c_str());

        Microsoft::WRL::ComPtr<ID3D12DeviceRemovedExtendedData> dred;
        hResult = mDevice->QueryInterface(IID_PPV_ARGS(&dred));
        HASSERT(SUCCEEDED(hResult), "Failed to query DRED.\n", utility::translateErrorCode(hResult).c_str());

        Microsoft::WRL::ComPtr<ID3D12DeviceRemovedExtendedData1> dred1;
        dred.As(&dred1);
        D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT1 dredAutoBreadcrumbsOutput;
        hResult = dred1->GetAutoBreadcrumbsOutput1(&dredAutoBreadcrumbsOutput);

        if (SUCCEEDED(hResult))
        {
            const D3D12_AUTO_BREADCRUMB_NODE1* pNode = dredAutoBreadcrumbsOutput.pHeadAutoBreadcrumbNode;
            while (pNode)
            {
                if (pNode->pCommandQueueDebugNameA)
                {
                    HLOG(DRED, "Command Queue: ", pNode->pCommandQueueDebugNameA);
                }
                else if (pNode->pCommandQueueDebugNameW)
                {
                    HLOG(DRED, "Command Queue: ", utility::toUTF8(pNode->pCommandQueueDebugNameW));
                }
                else
                {
                    HLOG(DRED, "Command Queue: <unnamed>");
                }


                if (pNode->pCommandListDebugNameA)
                {
                    HLOG(DRED, "Command List: ", pNode->pCommandListDebugNameA);
                }
                else if (pNode->pCommandListDebugNameW)
                {
                    HLOG(DRED, "Command List: ", utility::toUTF8(pNode->pCommandListDebugNameW));
                }
                else
                {
                    HLOG(DRED, "Command List: <unnamed>");
                }

                UINT lastBreadcrumb = *(pNode->pLastBreadcrumbValue);
                UINT breadcrumbCount = pNode->BreadcrumbCount;

                HASSERT(pNode->pCommandHistory != nullptr, "[DRED] Command list history not found.");

                for (UINT i = 0; i < lastBreadcrumb; ++i)
                {
                    HLOG(DRED, "Breadcrumb Operation", "(", i, " / ", breadcrumbCount, "): ", toString(pNode->pCommandHistory[i]).c_str());
                }

                if (lastBreadcrumb < breadcrumbCount)
                {
                    HLOG(DRED, "UNFINISHED COMMAND LIST AT ", lastBreadcrumb, ": ", toString(pNode->pCommandHistory[lastBreadcrumb]).c_str());
                }
                else
                {
                    HLOG(DRED, "Command list finished successfully.");
                }

                pNode = pNode->pNext;
            }
        }
        else
        {
            HLOG(DRED, "Failed to get breadcrumbs output.\n", utility::translateErrorCode(hResult).c_str());
        }

        Microsoft::WRL::ComPtr<ID3D12DeviceRemovedExtendedData2> dred2;
        dred.As(&dred2);
        D3D12_DRED_PAGE_FAULT_OUTPUT2 dredPageFaultOutput;
        hResult = dred2->GetPageFaultAllocationOutput2(&dredPageFaultOutput);

        if (SUCCEEDED(hResult))
        {
            if (dredPageFaultOutput.PageFaultVA == 0)
            {
                HLOG(DRED, "No page fault occurred.");
                return;
            }

            HLOG(DRED, "Page Fault at GPU Address: ", dredPageFaultOutput.PageFaultVA);

            {
                const D3D12_DRED_ALLOCATION_NODE1* pExistingNode = dredPageFaultOutput.pHeadExistingAllocationNode;

                while (pExistingNode)
                {
                    const char* nameA = pExistingNode->ObjectNameA ? pExistingNode->ObjectNameA : "<unnamed>";
                    const wchar_t* nameW = pExistingNode->ObjectNameW ? pExistingNode->ObjectNameW : L"<unnamed>";

                    HLOG(DRED, "Existing Allocation (%p)",
                        toString(pExistingNode->AllocationType).c_str(), ": ",
                        nameA, " ",
                        utility::toUTF8(nameW), " ",
                        "(", pExistingNode->pObject, ")");

                    pExistingNode = pExistingNode->pNext;
                }
            }

            {
                const D3D12_DRED_ALLOCATION_NODE1* pFreeNode = dredPageFaultOutput.pHeadRecentFreedAllocationNode;

                while (pFreeNode)
                {
                    HLOG(DRED, "Recently Freed Allocation ", toString(pFreeNode->AllocationType).c_str(), ": ", pFreeNode->ObjectNameA, " (", pFreeNode->pObject, ")");
                    HLOG(DRED, "Recently Freed Allocation ", toString(pFreeNode->AllocationType).c_str(), ": ", utility::toUTF8(pFreeNode->ObjectNameW), " (", pFreeNode->pObject, ")");

                    pFreeNode = pFreeNode->pNext;
                }
            }
        }
        else
        {
            HLOG(DRED, "Failed to get pagefault output.\n", utility::translateErrorCode(hResult).c_str());
        }
#endif
    }
}
