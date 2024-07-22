#pragma once

#include "Graphics/HBuffer.h"
#include "Graphics/HDrawable.h"
#include "Graphics/HShader.h"
#include "Graphics/HPipeline.h"
#include "Graphics/HRenderPass.h"
#include "Graphics/HRenderer.h"

#include "directx/d3dx12.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include <wrl.h>
#include <shellapi.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

using namespace Microsoft::WRL;

namespace dx12
{
	class Renderer : public harmony::Renderer
	{
	public:
		Renderer(const harmony::Renderer::Description& description);
		~Renderer();
		const harmony::Renderer::Description& getDescription() const;
		void initialize() override;
		void render() override;
		void deinitialize() override;
	private:
		void getHardwareAdapter(_In_ IDXGIFactory1* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter = false) const;
		void loadPipeline();
		void loadAssets();
		void populateCommandList();
		void waitForPreviousFrame();

	private:
		static const UINT mFrameCount = 2;
		struct Vertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT4 color;
		};
		harmony::Renderer::Description mDescription;
		UINT mFrameIndex = 0;
	private:
		ComPtr<ID3D12Device> mDevice;
		ComPtr<ID3D12CommandQueue> mCommandQueue;
		ComPtr<IDXGISwapChain3> mSwapChain;
		UINT mRTVDescriptorSize = 0;
		ComPtr<ID3D12DescriptorHeap> mRTVHeap;
		ComPtr<ID3D12Resource> mRenderTargets[mFrameCount];
		ComPtr<ID3D12CommandAllocator> mCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> mCommandList;
		ComPtr<ID3D12Fence> mFence;
		UINT64 mFenceValue = 0;
		HANDLE mFenceEvent = nullptr;
		ComPtr<ID3D12PipelineState> mPipelineState;

		CD3DX12_VIEWPORT mViewport;
		CD3DX12_RECT mScissorRect;
		ComPtr<ID3D12RootSignature> mRootSignature;

	};
}

namespace dx12
{
	class Buffer : public harmony::Buffer
	{
	public:
		Buffer(const harmony::Buffer::Description& description);
		~Buffer();

		const void* getID() const override;
		const Description& getDescription() const override;

		void initialize() override;
		void deinitialize() override;

	private:
		harmony::Buffer::Description mDescription;

		ComPtr<ID3D12Resource> mBuffer;
		D3D12_VERTEX_BUFFER_VIEW mBufferView;
	};
}