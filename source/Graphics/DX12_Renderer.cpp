#include "DX12_Renderer.h"

#include "Helpers/HLog.h"

static inline std::unique_ptr<char[]> hrErrorDescription(const HRESULT hResult)
{
	HRESULT hr = hResult;
	if (FACILITY_WINDOWS == HRESULT_FACILITY(hResult))
		hr = HRESULT_CODE(hResult);

	LPSTR szErrMsg = nullptr;

	if (FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&szErrMsg, 0, NULL) != 0)
	{
		size_t bufferSize = strlen(szErrMsg) + 1;

		std::unique_ptr<char[]> errorMessage(new char[bufferSize]);
		strcpy_s(errorMessage.get(), bufferSize, szErrMsg);

		LocalFree(szErrMsg);

		return errorMessage;
	}
	else
	{
		const char* genericErrorMsg = "Could not find a description for error # % #x.";
		size_t bufferSize = snprintf(nullptr, 0, genericErrorMsg, hr) + 1;

		std::unique_ptr<char[]> errorMessage(new char[bufferSize]);
		snprintf(errorMessage.get(), bufferSize, genericErrorMsg, hr);

		return errorMessage;
	}
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
		HASSERT(SUCCEEDED(hResult), "Failed to present swapchain.\n%s", hrErrorDescription(hResult).get());

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
		HRESULT hResult;

		*ppAdapter = nullptr;

		ComPtr<IDXGIAdapter1> adapter;

		ComPtr<IDXGIFactory6> factory6;
		hResult = pFactory->QueryInterface(IID_PPV_ARGS(&factory6));
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

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) // skip the warp renderer
					continue;

				hResult = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
				if (SUCCEEDED(hResult))
				{
					break;
				}
				
				HLOG_DEBUG("%ls", desc.Description);
			}
		}
		else
		{
			HSOFT_ASSERT(false, "Failed to fetch adapter by GPU preference. Failed to get factory6.\n%s", hrErrorDescription(hResult).get());
		}

		if (adapter.Get() == nullptr)
		{
			for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) // skip the warp renderer
					continue;

				hResult = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
				if (SUCCEEDED(hResult))
				{
					break;
				}

				HLOG_DEBUG("%ls", desc.Description);
			}
		}

		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		HLOG_INFO("Adapter: %ls(%zu)", desc.Description, desc.DeviceId);
		HLOG_INFO("Dedicated Video Memory: %zu bytes", desc.DedicatedVideoMemory);
		HLOG_INFO("Shared System Memory: %zu bytes", desc.SharedSystemMemory);

		*ppAdapter = adapter.Detach();
	}

	void Renderer::loadPipeline()
	{
		HRESULT hResult;

		UINT dxgiFactoryFlags = 0;
#ifdef _DEBUG
		{
			ComPtr<ID3D12Debug> debugController;
			hResult = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
			
			if (SUCCEEDED(hResult))
			{
				debugController->EnableDebugLayer();
				dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
			else
			{
				HSOFT_ASSERT(false, "Failed to get d3d12 debug interface.\n%s", hrErrorDescription(hResult).get());
			}
		}
#endif

		ComPtr<IDXGIFactory4> factory;
		hResult = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
		HASSERT(SUCCEEDED(hResult), "Failed to create DXGI factory.\n%s", hrErrorDescription(hResult).get());
		

		if (mDescription.useWarpDevice)
		{
			ComPtr<IDXGIAdapter> warpAdapter;
			hResult = factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
			HASSERT(SUCCEEDED(hResult), "Failed to get warp adapter.\n%s", hrErrorDescription(hResult).get());
			
			hResult = D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice));
			HASSERT(SUCCEEDED(hResult), "Failed to create warp device.\n%s", hrErrorDescription(hResult).get());

			DXGI_ADAPTER_DESC desc;
			warpAdapter->GetDesc(&desc);
			HLOG_INFO("Adapter: %ls(%zu)", desc.Description, desc.DeviceId);
			HLOG_INFO("Dedicated Video Memory: %zu bytes", desc.DedicatedVideoMemory);
			HLOG_INFO("Shared System Memory: %zu bytes", desc.SharedSystemMemory);
		}
		else
		{
			ComPtr<IDXGIAdapter1> hardwareAdapter;
			getHardwareAdapter(factory.Get(), &hardwareAdapter);
			hResult = D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice));
			HASSERT(SUCCEEDED(hResult), "Failed to create hardware device.\n%s", hrErrorDescription(hResult).get());
		}

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		hResult = mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue));
		HASSERT(SUCCEEDED(hResult), "Failed to create command queue.\n%s", hrErrorDescription(hResult).get());

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
		hResult = factory->CreateSwapChainForHwnd(
				mCommandQueue.Get(),
				*((HWND*)(harmony::getWindow()->getHandle())),
				&swapChainDesc,
				nullptr, nullptr,
				&swapChain
			);
		HASSERT(SUCCEEDED(hResult), "Failed to create the default swapchain.\n%s", hrErrorDescription(hResult).get());
		
		hResult = swapChain.As(&mSwapChain);
		HASSERT(SUCCEEDED(hResult), "Failed to set the default swapchain. \n%s", hrErrorDescription(hResult).get());
		
		mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

		{
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = mFrameCount;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			hResult = mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRTVHeap));
			HASSERT(SUCCEEDED(hResult), "Failed to create descriptor RTV heap.\n%s", hrErrorDescription(hResult).get());

			mRTVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}

		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart());

			for (UINT n = 0; n < mFrameCount; ++n)
			{
				hResult = mSwapChain->GetBuffer(n, IID_PPV_ARGS(&mRenderTargets[n]));
				HASSERT(SUCCEEDED(hResult), "Failed to get RT buffer.\n%s", hrErrorDescription(hResult).get());
				
				mDevice->CreateRenderTargetView(mRenderTargets[n].Get(), nullptr, rtvHandle);
				rtvHandle.Offset(1, mRTVDescriptorSize);
			}
		}
		
		hResult = mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));
		HASSERT(SUCCEEDED(hResult), "Failed to create command allocator.\n%s", hrErrorDescription(hResult).get());
	}

	void Renderer::loadAssets()
	{
		HRESULT hResult;

		{
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
			
			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;

			hResult = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
			HASSERT(SUCCEEDED(hResult), "Failed to serialize root signature.\n%s", hrErrorDescription(hResult).get());

			hResult = mDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
			HASSERT(SUCCEEDED(hResult), "Failed to create root signature.\n%s", hrErrorDescription(hResult).get());
		}

		{
			ComPtr<ID3DBlob> vertexShader;
			ComPtr<ID3DBlob> pixelShader;

#ifdef _DEBUG
			UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
			UINT compileFlags = 0;
#endif // _DEBUG

			// TODO: Replace the assert with error and provide a default fall-back shader.
			hResult = D3DCompileFromFile(L"D:\\Harmony\\resources\\shaders\\shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
			HASSERT(SUCCEEDED(hResult), "Failed to compile vertex shader.\n%s", hrErrorDescription(hResult).get());
			
			hResult = D3DCompileFromFile(L"D:\\Harmony\\resources\\shaders\\shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &vertexShader, nullptr);
			HASSERT(SUCCEEDED(hResult), "Failed to compile pixel shader.\n%s", hrErrorDescription(hResult).get());

			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
			psoDesc.pRootSignature = mRootSignature.Get();
			psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
			psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			psoDesc.DepthStencilState.DepthEnable = FALSE;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.SampleDesc.Count = 1;

			hResult = mDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipelineState));
			HASSERT(SUCCEEDED(hResult), "Failed to create a graphics pipeline state.\n%s", hrErrorDescription(hResult).get());
		}

		hResult = mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&mCommandList));
		HASSERT(SUCCEEDED(hResult), "Failed to create command list.\n%s", hrErrorDescription(hResult).get());
		
		hResult = mCommandList->Close();
		HASSERT(SUCCEEDED(hResult), "Failed to close command list.\n%s", hrErrorDescription(hResult).get());

		{

		}


		hResult = mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
		HASSERT(SUCCEEDED(hResult), "Failed to create fence.\n%s", hrErrorDescription(hResult).get());
		mFenceValue = 1;

		mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (mFenceEvent == nullptr)
		{
			hResult = HRESULT_FROM_WIN32(GetLastError());
			HASSERT(SUCCEEDED(hResult), "Failed to create fence event.\n%s", hrErrorDescription(hResult).get());
		}
	}

	void Renderer::populateCommandList()
	{
		HRESULT hResult;

		hResult = mCommandAllocator->Reset();
		HASSERT(SUCCEEDED(hResult), "Failed to reset command allocator.\n%s", hrErrorDescription(hResult).get());

		hResult = mCommandList->Reset(mCommandAllocator.Get(), mPipelineState.Get());
		HASSERT(SUCCEEDED(hResult), "Failed to reset command allocator.\n%s", hrErrorDescription(hResult).get());
		
		auto rtBarrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		mCommandList->ResourceBarrier(1, &rtBarrier);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), mFrameIndex, mRTVDescriptorSize);
		
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		mCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		auto presentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		mCommandList->ResourceBarrier(1, &presentBarrier);
		
		hResult = mCommandList->Close();
		HASSERT(SUCCEEDED(hResult), "Failed to close command list.\n%s", hrErrorDescription(hResult).get());
	}

	void Renderer::waitForPreviousFrame()
	{
		HRESULT hResult;
		const UINT64 fence = mFenceValue;
		{
			hResult = mCommandQueue->Signal(mFence.Get(), fence);
			HASSERT(SUCCEEDED(hResult), "Failed to signal fence.\n%s", hrErrorDescription(hResult).get());
		}
		mFenceValue++;

		if (mFence->GetCompletedValue() < fence)
		{
			hResult = mFence->SetEventOnCompletion(fence, mFenceEvent);
			HASSERT(SUCCEEDED(hResult), "Failed to set fence event.\n%s", hrErrorDescription(hResult).get());

			WaitForSingleObject(mFenceEvent, INFINITE);
		}

		mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
	}
}
