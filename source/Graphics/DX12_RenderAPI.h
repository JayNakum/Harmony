#pragma once

#include "Graphics/HRenderAPI.h"
#include "Graphics/HBuffer.h"
#include "Graphics/HDrawable.h"
#include "Graphics/HShader.h"
#include "Graphics/HPipeline.h"
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
		void deinitialize() override;

	private:
		void setupFrame();
		void closeFrame();
		void execute();
		void presentNow();

	private:
		void getHardwareAdapter(_In_ IDXGIFactory1* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter = false) const;
		void waitForPreviousFrame();

	private:
		static const UINT mFrameCount = 2;
		UINT mFrameIndex = 0;

		harmony::RenderAPI::Description mDescription;
	
	private:
		ComPtr<IDXGISwapChain3> mSwapChain;
		ComPtr<ID3D12CommandQueue> mCommandQueue;

		UINT mRTVDescriptorSize = 0;
		ComPtr<ID3D12DescriptorHeap> mRTVHeap;
		ComPtr<ID3D12Resource> mRenderTargets[mFrameCount];
		
		ComPtr<ID3D12Fence> mFence;
		UINT64 mFenceValue = 0;
		HANDLE mFenceEvent = nullptr;

		CD3DX12_VIEWPORT mViewport;
		CD3DX12_RECT mScissorRect;
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
		void bind() const override;
		void applyConstants(unsigned int constantBlockIndex, const void* data, size_t size, const harmony::Pipeline::ShaderStageType type = harmony::Pipeline::ShaderStageType::eVERTEX_SHADER) const override;
		void unbind() const override;
		void deinitialize() override;

	private:
		harmony::Pipeline::Description mDescription;
		ComPtr<ID3D12PipelineState> mPipelineState;
		ComPtr<ID3D12RootSignature> mRootSignature;

	private:
		static const std::unordered_map<harmony::Pipeline::VertexFormat, DXGI_FORMAT> sVERTEX_FORMAT;
	};
}

namespace dx12
{
	class RenderPass : public harmony::RenderPass
	{
	public:
		RenderPass(const harmony::RenderPass::Description& description);
		~RenderPass();

	public:
		const void* getID() const override;
		const harmony::RenderPass::Description& getDescription() const override;
		void initialize() override;
		void begin() override;
		void end() const override;
		void deinitialize() override;

	private:
		harmony::RenderPass::Description mDescription;
	};
}
