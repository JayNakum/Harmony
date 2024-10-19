#include "DX12_RenderAPI.h"

#include "Helpers/HLog.h"

namespace dx12
{
	static ComPtr<IDXGIFactory4> gFactory;
	static ComPtr<ID3D12Device> gDevice;
	static ComPtr<ID3D12GraphicsCommandList> gGFXCommandList;
	static ComPtr<ID3D12CommandAllocator> gCommandAllocator;
	static ComPtr<ID3D12CommandQueue> gCommandQueue;
}

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
	RenderAPI::RenderAPI(const harmony::RenderAPI::Description& description)
		: mDescription(description)
	{
		HLOG_INFO("Created.");
	}

	RenderAPI::~RenderAPI()
	{
		HLOG_INFO("Destroyed.");
	}

	const harmony::RenderAPI::Description& RenderAPI::getDescription() const
	{
		return mDescription;
	}

	void RenderAPI::initialize()
	{
		HRESULT hResult;

		{
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

			
			hResult = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&gFactory));
			HASSERT(SUCCEEDED(hResult), "Failed to create DXGI factory.\n%s", hrErrorDescription(hResult).get());


			if (mDescription.useWarpDevice)
			{
				ComPtr<IDXGIAdapter> warpAdapter;
				hResult = gFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
				HASSERT(SUCCEEDED(hResult), "Failed to get warp adapter.\n%s", hrErrorDescription(hResult).get());

				hResult = D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&gDevice));
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
				getHardwareAdapter(gFactory.Get(), &hardwareAdapter);
				hResult = D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&gDevice));
				HASSERT(SUCCEEDED(hResult), "Failed to create hardware device.\n%s", hrErrorDescription(hResult).get());
			}

			D3D12_COMMAND_QUEUE_DESC queueDesc = {};
			queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			hResult = gDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&gCommandQueue));
			HASSERT(SUCCEEDED(hResult), "Failed to create command queue.\n%s", hrErrorDescription(hResult).get());

			
			hResult = gDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&gCommandAllocator));
			HASSERT(SUCCEEDED(hResult), "Failed to create command allocator.\n%s", hrErrorDescription(hResult).get());
		
			hResult = gDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, gCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&gGFXCommandList));
			HASSERT(SUCCEEDED(hResult), "Failed to create command list.\n%s", hrErrorDescription(hResult).get());

			hResult = gGFXCommandList->Close();
			HASSERT(SUCCEEDED(hResult), "Failed to close command list.\n%s", hrErrorDescription(hResult).get());
		}

		{
			hResult = gDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
			HASSERT(SUCCEEDED(hResult), "Failed to create fence.\n%s", hrErrorDescription(hResult).get());
			mFenceValue = 1;

			mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (mFenceEvent == nullptr)
			{
				hResult = HRESULT_FROM_WIN32(GetLastError());
				HASSERT(SUCCEEDED(hResult), "Failed to create fence event.\n%s", hrErrorDescription(hResult).get());
			}

			waitForPreviousFrame();
		}
		HLOG_DEBUG("Initialized.");
	}


	void RenderAPI::closeFrame() 
	{
		HRESULT hResult = gGFXCommandList->Close();
		HASSERT(SUCCEEDED(hResult), "Failed to close command list.\n%s", hrErrorDescription(hResult).get());
	}

	void RenderAPI::execute() 
	{
		ID3D12CommandList* ppCommandList[] = { gGFXCommandList.Get() };
		gCommandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
	}

	void RenderAPI::present()
	{
		waitForPreviousFrame();
	}

	void RenderAPI::frameBegin()
	{
		HRESULT hResult;

		{
			hResult = gCommandAllocator->Reset();
			HASSERT(SUCCEEDED(hResult), "Failed to reset command allocator.\n%s", hrErrorDescription(hResult).get());

			hResult = gGFXCommandList->Reset(gCommandAllocator.Get(), nullptr);
			HASSERT(SUCCEEDED(hResult), "Failed to reset command allocator.\n%s", hrErrorDescription(hResult).get());
		}

		{
			mViewport.TopLeftX = 0.0f;
			mViewport.TopLeftY = 0.0f;
			mViewport.Width = (float)HARMONY->getWindow()->getDescription().width;
			mViewport.Height = (float)HARMONY->getWindow()->getDescription().height;
			mViewport.MinDepth = 0.0f;
			mViewport.MaxDepth = 1.0f;
			gGFXCommandList->RSSetViewports(1, &mViewport);
			mScissorRect.left = 0;
			mScissorRect.top = 0;
			mScissorRect.right = HARMONY->getWindow()->getDescription().width;
			mScissorRect.bottom = HARMONY->getWindow()->getDescription().height;
			gGFXCommandList->RSSetScissorRects(1, &mScissorRect);
		}
	}

	void RenderAPI::frameEnd()
	{
		closeFrame();
		execute();
	}


	void RenderAPI::deinitialize()
	{
		waitForPreviousFrame();

		CloseHandle(mFenceEvent);

		HLOG_DEBUG("Deinitialized.");
	}


	void RenderAPI::getHardwareAdapter(_In_ IDXGIFactory1* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter) const
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

		HLOG_INFO("Adapter Selected.\nAdapter: %ls(%zu)\nDedicated Video Memory: %zu bytes\nShared System Memory: %zu bytes", 
			desc.Description, desc.DeviceId, 
			desc.DedicatedVideoMemory, 
			desc.SharedSystemMemory
		);

		*ppAdapter = adapter.Detach();
	}

	void RenderAPI::waitForPreviousFrame()
	{
		HRESULT hResult;
		const UINT64 fence = mFenceValue;
		{
			hResult = gCommandQueue->Signal(mFence.Get(), fence);
			HASSERT(SUCCEEDED(hResult), "Failed to signal fence.\n%s", hrErrorDescription(hResult).get());
		}
		mFenceValue++;

		if (mFence->GetCompletedValue() < fence)
		{
			hResult = mFence->SetEventOnCompletion(fence, mFenceEvent);
			HASSERT(SUCCEEDED(hResult), "Failed to set fence event.\n%s", hrErrorDescription(hResult).get());

			WaitForSingleObject(mFenceEvent, INFINITE);
		}
	}
}

namespace dx12
{
	RenderTarget::RenderTarget(const harmony::RenderTarget::Description& description, ComPtr<ID3D12Resource> renderTarget, CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle)
		: mRenderTarget(renderTarget), mRTVHandle(rtvHandle)
	{
		HLOG_INFO("Created %s", mDescription.label);
	}

	const harmony::RenderTarget::Description& RenderTarget::getDescription() const
	{
		return mDescription;
	}

	const void* RenderTarget::getID() const
	{
		return mRenderTarget.Get();
	}

	void RenderTarget::initialize()
	{
		HLOG_DEBUG("Initialized %s", mDescription.label);
	}
	void RenderTarget::bind(const Action& action)
	{
		auto rtBarrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		gGFXCommandList->ResourceBarrier(1, &rtBarrier);

		gGFXCommandList->OMSetRenderTargets(1, &mRTVHandle, FALSE, nullptr);

		gGFXCommandList->ClearRenderTargetView(mRTVHandle, action.colors.value, 0, nullptr);
	}

	void RenderTarget::unbind()
	{
		auto presentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		gGFXCommandList->ResourceBarrier(1, &presentBarrier);
	}

	void RenderTarget::deinitialize()
	{
		HLOG_DEBUG("Deinitialized %s", mDescription.label);
	}

	RenderTarget::~RenderTarget()
	{
		HLOG_INFO("Destroyed %s", mDescription.label);
		mDescription = { 0 };
	}
}

namespace dx12
{
	Swapchain::Swapchain(const harmony::Swapchain::Description& description)
		: mDescription(description)
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = mBackbufferCount;
		swapChainDesc.Width = mDescription.width;
		swapChainDesc.Height = mDescription.height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> swapChain;
		HRESULT hResult = gFactory->CreateSwapChainForHwnd(
			gCommandQueue.Get(),
			*((HWND*)(HARMONY->getWindow()->getHandle())),
			&swapChainDesc,
			nullptr, nullptr,
			&swapChain
		);
		HASSERT(SUCCEEDED(hResult), "Failed to create the default swapchain.\n%s", hrErrorDescription(hResult).get());

		hResult = swapChain.As(&mSwapChain);
		HASSERT(SUCCEEDED(hResult), "Failed to set the default swapchain. \n%s", hrErrorDescription(hResult).get());

		HLOG_INFO("Created %s", mDescription.label);
	}

	const void* Swapchain::getID() const
	{
		return mSwapChain.Get();
	}

	const harmony::Swapchain::Description& Swapchain::getDescription() const
	{
		return mDescription;
	}

	void Swapchain::initialize()
	{
		{
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = mBackbufferCount;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			HRESULT hResult = gDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRTVHeap));
			HASSERT(SUCCEEDED(hResult), "Failed to create descriptor RTV heap.\n%s", hrErrorDescription(hResult).get());

			mRTVDescriptorSize = gDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}

		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart());

			for (UINT n = 0; n < mBackbufferCount; ++n)
			{
				ComPtr<ID3D12Resource> renderTarget;
				HRESULT hResult = mSwapChain->GetBuffer(n, IID_PPV_ARGS(&renderTarget));
				HASSERT(SUCCEEDED(hResult), "Failed to get RT buffer.\n%s", hrErrorDescription(hResult).get());

				gDevice->CreateRenderTargetView(renderTarget.Get(), nullptr, rtvHandle);
				
				{
					std::string label = "render target";
					label += " " + std::to_string(n);

					harmony::RenderTarget::Description createDesc;
					createDesc.label = label.c_str();

					mRenderTargets[n] = new RenderTarget(createDesc, renderTarget, rtvHandle);
				}

				rtvHandle.Offset(1, mRTVDescriptorSize);
			}
		}

		HLOG_DEBUG("Initialized %s", mDescription.label);
	}

	harmony::RenderTarget* Swapchain::getCurrentRenderTarget() const
	{
		return mRenderTargets[mFrameIndex];
	}

	void Swapchain::prepare()
	{
		
	}

	void Swapchain::resize(unsigned int width, unsigned int height)
	{
		// TODO: resize swapchain
	}

	void Swapchain::present()
	{
		HRESULT hResult = mSwapChain->Present(1, 0);
		HASSERT(SUCCEEDED(hResult), "Failed to present swapchain.\n%s", hrErrorDescription(hResult).get());

		mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
	}

	void Swapchain::deinitialize()
	{
		HLOG_DEBUG("Deinitialized %s", mDescription.label);
	}

	Swapchain::~Swapchain()
	{
		HLOG_INFO("Destroyed %s", mDescription.label);
		mDescription = { 0 };
	}
}

namespace dx12
{
	Buffer::Buffer(const harmony::Buffer::Description& description)
		: mDescription(description), mBuffer{ 0 }, mVBufferView{ 0 }, mIBufferView{ 0 }
	{
		CD3DX12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(mDescription.size);
		HRESULT hResult = gDevice->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mBuffer)
		);
		HASSERT(SUCCEEDED(hResult), "Failed to create buffer %s\n%s", mDescription.label, hrErrorDescription(hResult).get());

		HLOG_INFO("Created %s", mDescription.label);
	}

	const void* Buffer::getID() const
	{
		switch (mDescription.type)
		{
		default:
			HLOG_WARNING("ID requested for unknown buffer type. Assuming Type::eVERTEX_BUFFER.");
		case harmony::Buffer::Type::eVERTEX_BUFFER:
			return &mVBufferView;
		case harmony::Buffer::Type::eINDEX_BUFFER:
			return &mIBufferView;
		}
	}

	const harmony::Buffer::Description& Buffer::getDescription() const
	{
		return mDescription;
	}

	void Buffer::initialize()
	{
		UINT8* pBufferDataBegin;
		CD3DX12_RANGE readRange(0, 0);
		HRESULT hResult = mBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pBufferDataBegin));
		HASSERT(SUCCEEDED(hResult), "Failed to map buffer %s\n%s", mDescription.label, hrErrorDescription(hResult).get());
		memcpy(pBufferDataBegin, mDescription.data, mDescription.size);

		if (mDescription.type == harmony::Buffer::Type::eVERTEX_BUFFER)
		{
			mVBufferView.BufferLocation = mBuffer->GetGPUVirtualAddress();
			mVBufferView.StrideInBytes = (UINT)mDescription.stride;
			mVBufferView.SizeInBytes = (UINT)mDescription.size;
		}
		else if (mDescription.type == harmony::Buffer::Type::eINDEX_BUFFER)
		{
			mIBufferView.BufferLocation = mBuffer->GetGPUVirtualAddress();
			mIBufferView.SizeInBytes = (UINT)mDescription.size;
			mIBufferView.Format = DXGI_FORMAT_R16_UINT;
		}
		mBuffer->Unmap(0, nullptr);

		HLOG_DEBUG("Initialized %s", mDescription.label);
	}

	void Buffer::deinitialize()
	{
		HLOG_DEBUG("Deinitialized %s", mDescription.label);
	}
	
	Buffer::~Buffer()
	{
		mBuffer->Release();
		mVBufferView = { 0 };
		mIBufferView = { 0 };
		HLOG_INFO("Destroyed %s", mDescription.label);
		mDescription = { 0 };
	}
}

namespace dx12
{
	Drawable::Drawable(const harmony::Drawable::Description& description)
		: mDescription(description)
	{
		HLOG_INFO("Created %s", mDescription.label);
	}

	const harmony::Drawable::Description& Drawable::getDescription() const
	{
		return mDescription;
	}

	void Drawable::initialize()
	{
		const auto& vBufferDesc = mDescription.vertexBuffer->getDescription();
		HSOFT_ASSERT(vBufferDesc.type == harmony::Buffer::Type::eVERTEX_BUFFER, "Buffer type mismatch in %s", vBufferDesc.label);
		
		mDescription.vertexBuffer->initialize();


		if (mDescription.indexBuffer)
		{
			const auto& iBufferDesc = mDescription.indexBuffer->getDescription();
			HSOFT_ASSERT(iBufferDesc.type == harmony::Buffer::Type::eINDEX_BUFFER, "Buffer type mismatch in %s", iBufferDesc.label);

			mDescription.indexBuffer->initialize();
		}

		HLOG_DEBUG("Initialized %s", mDescription.label);
	}

	void Drawable::bind(unsigned int slot) const
	{
		gGFXCommandList->IASetVertexBuffers(slot, 1, (D3D12_VERTEX_BUFFER_VIEW*)mDescription.vertexBuffer->getID());
		if (mDescription.indexBuffer)
		{
			gGFXCommandList->IASetIndexBuffer((D3D12_INDEX_BUFFER_VIEW*)mDescription.indexBuffer->getID());
		}
	}

	void Drawable::draw() const
	{
		if (mDescription.indexBuffer)
		{
			gGFXCommandList->DrawIndexedInstanced(mDescription.drawDescription.numElements, mDescription.drawDescription.numInstances, mDescription.drawDescription.baseElement, mDescription.drawDescription.baseInstance, 0);
		}
		else
		{
			gGFXCommandList->DrawInstanced(mDescription.drawDescription.numElements, mDescription.drawDescription.numInstances, mDescription.drawDescription.baseElement, mDescription.drawDescription.baseInstance);
		}
	}

	void Drawable::unbind() const
	{
		
	}

	void Drawable::deinitialize()
	{
		mDescription.vertexBuffer->deinitialize();
		if (mDescription.indexBuffer)
		{
			mDescription.indexBuffer->deinitialize();
		}

		HLOG_DEBUG("Deinitialized %s", mDescription.label);
	}

	Drawable::~Drawable()
	{
		delete mDescription.vertexBuffer;
		delete mDescription.indexBuffer;

		HLOG_INFO("Destroyed %s", mDescription.label);
		mDescription = { 0 };
	}
}

namespace dx12
{
	Shader::Shader(const harmony::Shader::Description& description) 
		: mDescription(description), mShader(nullptr)
	{
		HLOG_INFO("Created %s", mDescription.label);
	}

	const harmony::Shader::Description& Shader::getDescription() const
	{
		return mDescription;
	}

	const void* Shader::getID() const
	{
		return mShader.Get();
	}

	void Shader::initialize()
	{
		HRESULT hResult;

#ifdef _DEBUG
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif // _DEBUG

		const char* target;

		switch (mDescription.type)
		{
		default:
			HLOG_WARNING("Invalid shader type(%d) in %s. Assuming type vertex shader.", mDescription.type, mDescription.label);
		
		case harmony::Shader::Type::eVERTEX:
			target = "vs_5_0";
			break;

		case harmony::Shader::Type::eFRAGMENT:
			target = "ps_5_0";
			break;

		}

		ComPtr<ID3DBlob> errorBlob;

		hResult = D3DCompile(
			mDescription.source,
			strlen(mDescription.source),
			nullptr,
			nullptr,
			nullptr,
			"main",
			target,
			compileFlags,
			0,
			&mShader,
			&errorBlob
		);

		if (FAILED(hResult))
		{
			// TODO: Provide a fall back shader
			HLOG_ERROR("Failed to compile shader\n%s\n%s", mDescription.label, errorBlob->GetBufferPointer(), hrErrorDescription(hResult).get());
			HASSERT(false, "Provide a fall back");
		}

		//errorBlob->Release();

		HLOG_INFO("Initialized %s", mDescription.label);
	}

	void Shader::deinitialize()
	{
		mShader->Release();
		HLOG_INFO("Deinitialized %s", mDescription.label);
	}

	Shader::~Shader()
	{
		HLOG_INFO("Destroyed %s", mDescription.label);
		mDescription = { 0 };
	}
}

namespace dx12
{
	Pipeline::Pipeline(const harmony::Pipeline::Description& description)
		: mDescription(description), mPipelineState(nullptr)
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

		if (FAILED(gDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}


		HRESULT hResult;
		{
			CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
			CD3DX12_ROOT_PARAMETER1 rootParameters[1];

			ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
			rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);

			D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;


			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init_1_1(1, rootParameters, 0, nullptr, rootSignatureFlags);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;

			hResult = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error);
			HASSERT(SUCCEEDED(hResult), "Failed to serialize root signature.\n%s", hrErrorDescription(hResult).get());

			hResult = gDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
			HASSERT(SUCCEEDED(hResult), "Failed to create root signature.\n%s", hrErrorDescription(hResult).get());
		}

		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
		cbvHeapDesc.NumDescriptors = 1;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		hResult = gDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCBVHeap));
		HASSERT(SUCCEEDED(hResult), "Failed to create descriptor heap for CBV.");


		HLOG_INFO("Created %s", mDescription.label);
	}

	const void* Pipeline::getID() const
	{
		return mPipelineState.Get();
	}

	const harmony::Pipeline::Description& Pipeline::getDescription() const
	{
		return mDescription;
	}

	void Pipeline::initialize()
	{
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[16] = {};
		
		for (unsigned int i = 0, offset = 0; i < mDescription.layout.attributesCount; ++i)
		{
			DXGI_FORMAT fmt = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			
			HASSERT((sVERTEX_FORMAT.find(mDescription.layout.attributes[i].format) != sVERTEX_FORMAT.end()), "Format not found.");

			fmt = sVERTEX_FORMAT.at(mDescription.layout.attributes[i].format);
			inputElementDescs[i] = {
				mDescription.layout.attributes[i].name,
				(unsigned int)mDescription.layout.attributes[i].index,
				fmt,
				0, 
				offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // TODO: Add support for per instance data
			};

			offset += Pipeline::getByteSize(mDescription.layout.attributes[i].format);
		}


		D3D12_GRAPHICS_PIPELINE_STATE_DESC createDesc = {};

		createDesc.InputLayout = { inputElementDescs, mDescription.layout.attributesCount };
		createDesc.pRootSignature = mRootSignature.Get();
		createDesc.VS = CD3DX12_SHADER_BYTECODE((ID3DBlob*)(mDescription.vertexShader->getID()));
		createDesc.PS = CD3DX12_SHADER_BYTECODE((ID3DBlob*)(mDescription.fragmentShader->getID()));
		
		createDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		createDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // TODO: use description variable
		
		createDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		
		createDesc.DepthStencilState.DepthEnable = FALSE;
		createDesc.DepthStencilState.StencilEnable = FALSE;

		createDesc.SampleMask = UINT_MAX;
		createDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		createDesc.NumRenderTargets = 1;
		createDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		createDesc.SampleDesc.Count = 1;

		HRESULT hResult = gDevice->CreateGraphicsPipelineState(&createDesc, IID_PPV_ARGS(&mPipelineState));
		
		if (FAILED(hResult))
		{
			// TODO: Provide a fall back pipeline state
			HLOG_ERROR("Failed to create a graphics pipeline state.\n%s", hrErrorDescription(hResult).get());
			HASSERT(false, "Provide a fall back");
		}


		const auto& vsDescription = mDescription.vertexShader->getDescription();
		const auto& fsDescription = mDescription.fragmentShader->getDescription();
		

		// Create the constant buffer.
		{
			unsigned int size = (vsDescription.constantBlocks[0].size + 255) & ~255;
			// HASSERT(size % 256 == 0, "Constant Buffer size must be 256-byte aligned.");

			CD3DX12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

			hResult = gDevice->CreateCommittedResource(
				&uploadHeapProperties,
				D3D12_HEAP_FLAG_NONE,
				&bufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&mVSConstantBuffer));

			HASSERT(SUCCEEDED(hResult), "Failed to create constant buffer.\n%s", hrErrorDescription(hResult).get());

		}

		HLOG_DEBUG("Initialized %s", mDescription.label);
	}

	void Pipeline::bind()
	{
		gGFXCommandList->SetGraphicsRootSignature(mRootSignature.Get());
		gGFXCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		gGFXCommandList->SetPipelineState(mPipelineState.Get());
		
		ID3D12DescriptorHeap* ppHeaps[] = { mCBVHeap.Get() };
		gGFXCommandList->SetDescriptorHeaps(1, ppHeaps);
		gGFXCommandList->SetGraphicsRootDescriptorTable(0, mCBVHeap->GetGPUDescriptorHandleForHeapStart());


	}

	void Pipeline::applyConstants(unsigned int constantBlockIndex, const void* data, size_t size, const harmony::Pipeline::ShaderStageType type)
	{
		CD3DX12_RANGE readRange(0, 0);
		HRESULT hResult = mVSConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mBufferDataBegin));
		HASSERT(SUCCEEDED(hResult), "Failed to map resource, constant buffer. %s", hrErrorDescription(hResult).get());
		memcpy(mBufferDataBegin, data, size);
		const auto& vsDescription = mDescription.vertexShader->getDescription();
		unsigned int alignedSize = (vsDescription.constantBlocks[0].size + 255) & ~255;

		mVSConstantBuffer->Unmap(0, nullptr);
		// Describe and create a constant buffer view.
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = mVSConstantBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = alignedSize;
		gDevice->CreateConstantBufferView(&cbvDesc, mCBVHeap->GetCPUDescriptorHandleForHeapStart());
	}

	void Pipeline::unbind() const
	{
	}

	void Pipeline::deinitialize()
	{
		mPipelineState->Release();
		HLOG_DEBUG("Deinitialized %s", mDescription.label);
	}

	Pipeline::~Pipeline()
	{
		mRootSignature->Release();
		HLOG_INFO("Destroyed %s", mDescription.label);
		mDescription = { 0 };
	}

	const std::unordered_map<harmony::Pipeline::VertexFormat, DXGI_FORMAT> Pipeline::sVERTEX_FORMAT = {
		{ harmony::Pipeline::VertexFormat::eFLOAT4, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT },
		{ harmony::Pipeline::VertexFormat::eFLOAT3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT },
		{ harmony::Pipeline::VertexFormat::eHALF4, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT },
		{ harmony::Pipeline::VertexFormat::eUSHORT4N, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_UNORM },
		{ harmony::Pipeline::VertexFormat::eSHORT4, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_UINT },
		{ harmony::Pipeline::VertexFormat::eSHORT4N, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_SNORM },
		{ harmony::Pipeline::VertexFormat::eFLOAT2, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT },
		{ harmony::Pipeline::VertexFormat::eUBYTE4N, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM },
		{ harmony::Pipeline::VertexFormat::eUBYTE4, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UINT },
		{ harmony::Pipeline::VertexFormat::eBYTE4N, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_SNORM },
		{ harmony::Pipeline::VertexFormat::eBYTE4, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_SINT },
		{ harmony::Pipeline::VertexFormat::eHALF2, DXGI_FORMAT::DXGI_FORMAT_R16G16_FLOAT },
		{ harmony::Pipeline::VertexFormat::eUSHORT2N, DXGI_FORMAT::DXGI_FORMAT_R16G16_UNORM },
		{ harmony::Pipeline::VertexFormat::eSHORT2N, DXGI_FORMAT::DXGI_FORMAT_R16G16_SNORM },
		{ harmony::Pipeline::VertexFormat::eSHORT2, DXGI_FORMAT::DXGI_FORMAT_R16G16_SINT },
		{ harmony::Pipeline::VertexFormat::eFLOAT, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT }
	};
}
