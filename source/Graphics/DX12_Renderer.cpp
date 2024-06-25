#include "DX12_Renderer.h"

#include "Helpers/HLog.h"

static void printHRError(const HRESULT hResult)
{
	// _com_error err(hResult);
	/*HLOG_ERROR("%s", err.Error());
	HLOG_ERROR("Message: %s", err.ErrorMessage());
	HLOG_ERROR("Info: %s", err.ErrorInfo());
	HLOG_ERROR("Description: %s", err.Description());*/
}

namespace dx12
{
	Renderer::Renderer(const harmony::Renderer::Description& description)
		: mDescription(description)
	{
		HLOG_INFO("Created.");
	}

	Renderer::~Renderer()
	{
		HLOG_INFO("Destroyed.");
	}

	const harmony::Renderer::Description& Renderer::getDescription() const
	{
		return mDescription;
	}

	void Renderer::initialize()
	{
		loadPipeline();
		loadAssets();
		HLOG_DEBUG("Initialized.");
	}

	void Renderer::render()
	{
		populateCommandList();

		ID3D12CommandList* ppCommandList[] = {mCommandList.Get()};
		mCommandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);

		HRESULT hResult = mSwapChain->Present(1, 0);
		if (FAILED(hResult))
		{
			printHRError(hResult);
			HASSERT(false, "Failed to present swapchain.");
		}

		waitForPreviousFrame();
	}

	void Renderer::deinitialize()
	{
		waitForPreviousFrame();

		CloseHandle(mFenceEvent);

		HLOG_DEBUG("Deinitialized.");
	}

	void Renderer::getHardwareAdapter(_In_ IDXGIFactory1* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter) const
	{
		*ppAdapter = nullptr;

		ComPtr<IDXGIAdapter1> adapter;

		ComPtr<IDXGIFactory6> factory6;
		HRESULT hResult = pFactory->QueryInterface(IID_PPV_ARGS(&factory6));
		if (SUCCEEDED(hResult))
		{
			for (
				UINT adapterIndex = 0;
				SUCCEEDED(factory6->EnumAdapterByGpuPreference(
					adapterIndex,
					requestHighPerformanceAdapter ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
					IID_PPV_ARGS(&adapter)));
					++adapterIndex
				)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// skip the warp renderer
					continue;
				}

				{
					HRESULT hResult = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
					if (SUCCEEDED(hResult))
					{
						HLOG_INFO("Adapter: %ls(%zu)", desc.Description, desc.DeviceId);
						// HLOG_INFO("Dedicated System Memory: %zu", desc.DedicatedSystemMemory);
						HLOG_INFO("Dedicated Video Memory: %zu bytes", desc.DedicatedVideoMemory);
						HLOG_INFO("Shared System Memory: %zu bytes", desc.SharedSystemMemory);
						break;
					}
				}
				HLOG_DEBUG("%ls", desc.Description);
			}
		}
		else
		{
			printHRError(hResult);
			HSOFT_ASSERT(false, "Failed to fetch adapter by GPU preference. Failed to get factory6.");
		}

		if (adapter.Get() == nullptr)
		{
			for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);
				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// skip the warp renderer
					continue;
				}

				HRESULT hResult = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
				if (SUCCEEDED(hResult))
				{
					HLOG_INFO("Adapter: %ls(%zu)", desc.Description, desc.DeviceId);
					// HLOG_INFO("Dedicated System Memory: %zu", desc.DedicatedSystemMemory);
					HLOG_INFO("Dedicated Video Memory: %zu bytes", desc.DedicatedVideoMemory);
					HLOG_INFO("Shared System Memory: %zu bytes", desc.SharedSystemMemory);
					break;
				}
				HLOG_DEBUG("%ls", desc.Description);
			}
		}

		*ppAdapter = adapter.Detach();
	}

	void Renderer::loadPipeline()
	{
		UINT dxgiFactoryFlags = 0;
#ifdef _DEBUG
		{
			ComPtr<ID3D12Debug> debugController;
			HRESULT hResult = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
			
			if (SUCCEEDED(hResult))
			{
				debugController->EnableDebugLayer();
				dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
			else
			{
				printHRError(hResult);
				HSOFT_ASSERT(false, "Failed to get d3d12 debug interface.");
			}
		}
#endif

		ComPtr<IDXGIFactory4> factory;
		{
			HRESULT hResult = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
			if (FAILED(hResult))
			{
				printHRError(hResult);
				HASSERT(false, "Failed to create DXGI factory.");
			}
		}

		if (mDescription.useWarpDevice)
		{
			ComPtr<IDXGIAdapter> warpAdapter;
			{
				HRESULT hResult = factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
				if (FAILED(hResult))
				{
					printHRError(hResult);
					HASSERT(false, "Failed to get warp adapter.");
				}
			}
			{
				HRESULT hResult = D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice));
				if (FAILED(hResult))
				{
					printHRError(hResult);
					HASSERT(false, "Failed to create warp device.");
				}

				DXGI_ADAPTER_DESC desc;
				warpAdapter->GetDesc(&desc);
				HLOG_INFO("Adapter: %ls(%zu)", desc.Description, desc.DeviceId);
				// HLOG_INFO("Dedicated System Memory: %zu", desc.DedicatedSystemMemory);
				HLOG_INFO("Dedicated Video Memory: %zu bytes", desc.DedicatedVideoMemory);
				HLOG_INFO("Shared System Memory: %zu bytes", desc.SharedSystemMemory);
			}
		}
		else
		{
			ComPtr<IDXGIAdapter1> hardwareAdapter;
			getHardwareAdapter(factory.Get(), &hardwareAdapter);
			{
				HRESULT hResult = D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice));
				if (FAILED(hResult))
				{
					printHRError(hResult);
					HASSERT(false, "Failed to create hardware device.");
				}
			}
		}

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		{
			HRESULT hResult = mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue));
			if (FAILED(hResult))
			{
				printHRError(hResult);
				HASSERT(false, "Failed to create command queue.");
			}
		}

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = mFrameCount;

		auto& windowDesc = harmony::getWindow()->getDescription();
		swapChainDesc.Width = windowDesc.width;
		swapChainDesc.Height = windowDesc.height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> swapChain;
		{
			HRESULT hResult = factory->CreateSwapChainForHwnd(
				mCommandQueue.Get(),
				*((HWND*)(harmony::getWindow()->getHandle())),
				&swapChainDesc,
				nullptr, nullptr,
				&swapChain
			);
			if (FAILED(hResult))
			{
				printHRError(hResult);
				HASSERT(false, "Failed to create the default swapchain.");
			}
		}
		{
			HRESULT hResult = swapChain.As(&mSwapChain);
			if (FAILED(hResult))
			{
				printHRError(hResult);
				HASSERT(false, "Failed to set the default swapchain.");
			}
		}
		mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

		{
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = mFrameCount;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			HRESULT hResult = mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRTVHeap));
			if (FAILED(hResult))
			{
				printHRError(hResult);
				HASSERT(false, "Failed to create descriptor rtv heap.");
			}
			mRTVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}

		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart());

			for (UINT n = 0; n < mFrameCount; ++n)
			{
				HRESULT hResult = mSwapChain->GetBuffer(n, IID_PPV_ARGS(&mRenderTargets[n]));
				if (FAILED(hResult))
				{
					printHRError(hResult);
					HASSERT(false, "Failed to get RT buffer.");
				}
				mDevice->CreateRenderTargetView(mRenderTargets[n].Get(), nullptr, rtvHandle);
				rtvHandle.Offset(1, mRTVDescriptorSize);
			}
		}
		HRESULT hResult = mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));
		if (FAILED(hResult))
		{
			printHRError(hResult);
			HASSERT(false, "Failed to create command allocator.");
		}
	}

	void Renderer::loadAssets()
	{
		{
			HRESULT hResult = mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&mCommandList));
			if (FAILED(hResult))
			{
				printHRError(hResult);
				HASSERT(false, "Failed to create command list.");
			}
		}

		{
			HRESULT hResult = mCommandList->Close();
			if (FAILED(hResult))
			{
				printHRError(hResult);
				HASSERT(false, "Failed to close command list.");
			}
		}

		{
			{
				HRESULT hResult = mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
				if (FAILED(hResult))
				{
					printHRError(hResult);
					HASSERT(false, "Failed to create fence.");
				}
			}

			mFenceValue = 1;

			mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (mFenceEvent == nullptr)
			{
				HRESULT hResult = HRESULT_FROM_WIN32(GetLastError());
				if (FAILED(hResult))
				{
					printHRError(hResult);
					HASSERT(false, "Failed to create fence event.");
				}
			}
		}
	}

	void Renderer::populateCommandList()
	{
		{
			HRESULT hResult = mCommandAllocator->Reset();
			if (FAILED(hResult))
			{
				printHRError(hResult);
				HASSERT(false, "Failed to reset command allocator.");
			}
		}
		{
			HRESULT hResult = mCommandList->Reset(mCommandAllocator.Get(), mPipelineState.Get());
			if (FAILED(hResult))
			{
				printHRError(hResult);
				HASSERT(false, "Failed to reset command allocator.");
			}
		}

		auto test1 = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		mCommandList->ResourceBarrier(1, &test1);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), mFrameIndex, mRTVDescriptorSize);
		
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		mCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		auto test2 = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		mCommandList->ResourceBarrier(1, &test2);
		
		{
			HRESULT hResult = mCommandList->Close();
			if (FAILED(hResult))
			{
				printHRError(hResult);
				HASSERT(false, "Failed to close command list.");
			}
		}
	}

	void Renderer::waitForPreviousFrame()
	{
		const UINT64 fence = mFenceValue;
		{
			HRESULT hResult = mCommandQueue->Signal(mFence.Get(), fence);
			if (FAILED(hResult))
			{
				printHRError(hResult);
				HASSERT(false, "Failed to signal fence.");
			}
		}
		mFenceValue++;

		if (mFence->GetCompletedValue() < fence)
		{
			HRESULT hResult = mFence->SetEventOnCompletion(fence, mFenceEvent);
			if (FAILED(hResult))
			{
				printHRError(hResult);
				HASSERT(false, "Failed to set fence event.");
			}
			WaitForSingleObject(mFenceEvent, INFINITE);
		}

		mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
	}
}

