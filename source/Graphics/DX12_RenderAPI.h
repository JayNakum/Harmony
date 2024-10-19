#pragma once

#include "Graphics/HRenderAPI.h"
#include "Graphics/HSwapchain.h"
#include "Graphics/HBuffer.h"
#include "Graphics/HDrawable.h"
#include "Graphics/HShader.h"
#include "Graphics/HPipeline.h"
#include "Graphics/HRenderTarget.h"
#include "Graphics/HRenderPass.h"

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

#include <unordered_map>

using namespace Microsoft::WRL;

namespace dx12
{
	class RenderAPI : public harmony::RenderAPI
	{
	public:
		RenderAPI(const harmony::RenderAPI::Description& description);
		~RenderAPI();
		const harmony::RenderAPI::Description& getDescription() const;
		void initialize() override;
		void frameBegin() override;
		void frameEnd() override;
		void present() override;
		void deinitialize() override;

	private:
		void closeFrame();
		void execute();

	private:
		void getHardwareAdapter(_In_ IDXGIFactory1* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter = false) const;
		void waitForPreviousFrame();

	private:
		harmony::RenderAPI::Description mDescription;

	private:
		ComPtr<ID3D12Fence> mFence;
		UINT64 mFenceValue = 0;
		HANDLE mFenceEvent = nullptr;

		CD3DX12_VIEWPORT mViewport;
		CD3DX12_RECT mScissorRect;
	};
}

namespace dx12
{
	class RenderTarget : harmony::RenderTarget
	{
		friend class Swapchain;
		RenderTarget(const harmony::RenderTarget::Description& description, ComPtr<ID3D12Resource> renderTarget, CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle);

	public:
		~RenderTarget();
		const harmony::RenderTarget::Description& getDescription() const override;
		const void* getID() const override;
		void initialize() override;
		void bind(const Action& action = {}) override;
		void unbind() override;
		void deinitialize() override;

	private:
		harmony::RenderTarget::Description mDescription;
		ComPtr<ID3D12Resource> mRenderTarget;
		CD3DX12_CPU_DESCRIPTOR_HANDLE mRTVHandle;
	};
}

namespace dx12
{
	class Swapchain : public harmony::Swapchain
	{
	public:
		Swapchain(const harmony::Swapchain::Description& description);
		~Swapchain();

		const void* getID() const override;
		const harmony::Swapchain::Description& getDescription() const override;
		harmony::RenderTarget* getCurrentRenderTarget() const override;

		void initialize() override;
		void prepare() override;
		void resize(unsigned int width, unsigned int height) override;
		void present() override;
		void deinitialize() override;

	private:
		harmony::Swapchain::Description mDescription;
		
		ComPtr<IDXGISwapChain3> mSwapChain;

		static const UINT mBackbufferCount = 2;
		UINT mFrameIndex = 0;
		UINT mRTVDescriptorSize = 0;

		ComPtr<ID3D12DescriptorHeap> mRTVHeap;

		harmony::RenderTarget* mRenderTargets[mBackbufferCount] = { nullptr };
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
		D3D12_VERTEX_BUFFER_VIEW mVBufferView;
		D3D12_INDEX_BUFFER_VIEW mIBufferView;
	};
}

namespace dx12
{
	class Drawable : public harmony::Drawable
	{
	public:
		Drawable(const harmony::Drawable::Description& description);
		~Drawable();

	public:
		const harmony::Drawable::Description& getDescription() const override;

		void initialize() override;
		void bind(unsigned int slot = 0) const override;
		void draw() const override;
		void unbind() const override;
		void deinitialize() override;

	private:
		harmony::Drawable::Description mDescription;
	};
}

namespace dx12
{
	class Shader : public harmony::Shader
	{
	public:
		Shader(const harmony::Shader::Description& description);
		~Shader();
	public:
		const harmony::Shader::Description& getDescription() const override;
		const void* getID() const override;
		void initialize() override;
		void deinitialize() override;

	private:
		harmony::Shader::Description mDescription;
		ComPtr<ID3DBlob> mShader;
	};
}

namespace dx12
{
	class Pipeline : public harmony::Pipeline
	{
	public:
		Pipeline(const harmony::Pipeline::Description& description);
		~Pipeline();

	public:
		const void* getID() const override;
		const harmony::Pipeline::Description& getDescription() const override;
		void initialize() override;
		void bind() override;
		void applyConstants(unsigned int constantBlockIndex, const void* data, size_t size, const harmony::Pipeline::ShaderStageType type = harmony::Pipeline::ShaderStageType::eVERTEX_SHADER) override;
		void unbind() const override;
		void deinitialize() override;

	private:
		harmony::Pipeline::Description mDescription;
		ComPtr<ID3D12PipelineState> mPipelineState;
		ComPtr<ID3D12RootSignature> mRootSignature;
		ComPtr<ID3D12Resource> mVSConstantBuffer;

		ComPtr<ID3D12DescriptorHeap> mCBVHeap;
		UINT8* mBufferDataBegin;

	private:
		static const std::unordered_map<harmony::Pipeline::VertexFormat, DXGI_FORMAT> sVERTEX_FORMAT;
	};
}
