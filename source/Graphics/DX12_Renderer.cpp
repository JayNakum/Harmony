#include "DX12_Renderer.h"

#include "Helpers/HLog.h"

namespace dx12
{
	ComPtr<ID3D12Device> gDevice;
	ComPtr<ID3D12GraphicsCommandList> gGFXCommandList;
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

		ID3D12CommandList* ppCommandList[] = {gGFXCommandList.Get()};
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

		HLOG_INFO("Adapter Selected.\nAdapter: %ls(%zu)\nDedicated Video Memory: %zu bytes\nShared System Memory: %zu bytes", 
			desc.Description, desc.DeviceId, 
			desc.DedicatedVideoMemory, 
			desc.SharedSystemMemory
		);

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
			getHardwareAdapter(factory.Get(), &hardwareAdapter);
			hResult = D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&gDevice));
			HASSERT(SUCCEEDED(hResult), "Failed to create hardware device.\n%s", hrErrorDescription(hResult).get());
		}

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		hResult = gDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue));
		HASSERT(SUCCEEDED(hResult), "Failed to create command queue.\n%s", hrErrorDescription(hResult).get());

		auto& windowDesc = harmony::getWindow()->getDescription();

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = mFrameCount;
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

			hResult = gDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRTVHeap));
			HASSERT(SUCCEEDED(hResult), "Failed to create descriptor RTV heap.\n%s", hrErrorDescription(hResult).get());

			mRTVDescriptorSize = gDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}

		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart());

			for (UINT n = 0; n < mFrameCount; ++n)
			{
				hResult = mSwapChain->GetBuffer(n, IID_PPV_ARGS(&mRenderTargets[n]));
				HASSERT(SUCCEEDED(hResult), "Failed to get RT buffer.\n%s", hrErrorDescription(hResult).get());
				
				gDevice->CreateRenderTargetView(mRenderTargets[n].Get(), nullptr, rtvHandle);
				rtvHandle.Offset(1, mRTVDescriptorSize);
			}
		}
		
		hResult = gDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));
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

			hResult = gDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
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

			// TODO: Replace the HASSERT with HLOG_ERROR and provide a default fall-back.
			hResult = D3DCompileFromFile(L"D:\\Harmony\\resources\\shaders\\shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
			HASSERT(SUCCEEDED(hResult), "Failed to compile vertex shader.\n%s", hrErrorDescription(hResult).get());
			
			hResult = D3DCompileFromFile(L"D:\\Harmony\\resources\\shaders\\shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);
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
			psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			psoDesc.DepthStencilState.DepthEnable = FALSE;
			psoDesc.DepthStencilState.StencilEnable = FALSE;
			
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;

			hResult = gDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipelineState));
			HASSERT(SUCCEEDED(hResult), "Failed to create a graphics pipeline state.\n%s", hrErrorDescription(hResult).get());
		}

		hResult = gDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&gGFXCommandList));
		HASSERT(SUCCEEDED(hResult), "Failed to create command list.\n%s", hrErrorDescription(hResult).get());
		
		hResult = gGFXCommandList->Close();
		HASSERT(SUCCEEDED(hResult), "Failed to close command list.\n%s", hrErrorDescription(hResult).get());


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
	}

	void Renderer::populateCommandList()
	{
		HRESULT hResult;

		hResult = mCommandAllocator->Reset();
		HASSERT(SUCCEEDED(hResult), "Failed to reset command allocator.\n%s", hrErrorDescription(hResult).get());

		hResult = gGFXCommandList->Reset(mCommandAllocator.Get(), mPipelineState.Get());
		HASSERT(SUCCEEDED(hResult), "Failed to reset command allocator.\n%s", hrErrorDescription(hResult).get());
		
		gGFXCommandList->SetGraphicsRootSignature(mRootSignature.Get());
		mViewport.TopLeftX = 0.0f;
		mViewport.TopLeftY = 0.0f;
		mViewport.Width = (float)harmony::getWindow()->getDescription().width;
		mViewport.Height = (float)harmony::getWindow()->getDescription().height;
		mViewport.MinDepth = 0.0f;
		mViewport.MaxDepth = 1.0f;
		gGFXCommandList->RSSetViewports(1, &mViewport);
		mScissorRect.left = 0;
		mScissorRect.top = 0;
		mScissorRect.right = harmony::getWindow()->getDescription().width;
		mScissorRect.bottom = harmony::getWindow()->getDescription().height;
		gGFXCommandList->RSSetScissorRects(1, &mScissorRect);

		auto rtBarrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		gGFXCommandList->ResourceBarrier(1, &rtBarrier);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), mFrameIndex, mRTVDescriptorSize);
		
		gGFXCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		const float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		gGFXCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		gGFXCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		

		auto presentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		gGFXCommandList->ResourceBarrier(1, &presentBarrier);
		
		hResult = gGFXCommandList->Close();
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

namespace dx12
{
	Buffer::Buffer(const harmony::Buffer::Description& description)
		: mDescription(description), mBuffer{ 0 }, mBufferView{ 0 }
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
		return &mBufferView;
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

		mBufferView.BufferLocation = mBuffer->GetGPUVirtualAddress();
		mBufferView.StrideInBytes = mDescription.stride;
		mBufferView.SizeInBytes = mDescription.size;
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
			HSOFT_ASSERT(iBufferDesc.type == harmony::Buffer::Type::eVERTEX_BUFFER, "Buffer type mismatch in %s", iBufferDesc.label);

			mDescription.indexBuffer->initialize();
		}

		HLOG_DEBUG("Initialized %s", mDescription.label);
	}

	void Drawable::bind(unsigned int slot) const
	{
		gGFXCommandList->IASetVertexBuffers(slot, 1, (D3D12_VERTEX_BUFFER_VIEW*)mDescription.vertexBuffer->getID());
	}

	void Drawable::draw(const harmony::Drawable::DrawDescription& drawDescription) const
	{
		gGFXCommandList->DrawInstanced(drawDescription.numElements, drawDescription.numInstances, drawDescription.baseElement, drawDescription.baseInstance);
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

		switch (mDescription.shader.type)
		{
		default:
			HLOG_WARNING("Invalid shader type(%d) in %s. Assuming type vertex shader.", mDescription.shader.type, mDescription.label);
		
		case harmony::Shader::Type::eVERTEX:
			target = "vs_5_0";
			break;

		case harmony::Shader::Type::eFRAGMENT:
			target = "fs_5_0";
			break;

		}


		ComPtr<ID3DBlob> errorBlob;

		hResult = D3DCompile(
			mDescription.shader.source,
			strlen(mDescription.shader.source),
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
			// TODO: Provide a fall back vertex shader
			HLOG_ERROR("Failed to compile shader\n%s\n%s", mDescription.label, errorBlob->GetBufferPointer(), hrErrorDescription(hResult).get());
		}

		errorBlob->Release();

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
		HLOG_INFO("Created %s", mDescription.label);
	}

	void Pipeline::initialize()
	{
		HLOG_DEBUG("Initialized %s", mDescription.label);
	}

	void Pipeline::bind() const
	{

	}

	void Pipeline::deinitialize()
	{
		HLOG_DEBUG("Deinitialized %s", mDescription.label);
	}

	Pipeline::~Pipeline()
	{
		HLOG_INFO("Destroyed %s", mDescription.label);
		mDescription = { 0 };
	}
}