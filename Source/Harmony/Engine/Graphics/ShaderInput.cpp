#include "ShaderInput.h"

namespace engine
{
	using namespace graphics;

	ShaderInput::ShaderInput(const std::vector<Parameters>& layout, DynamicDescriptorHeap& shaderViewDescriptorHeap, graphics::GraphicContext& context)
		: mContext(context), mShaderViewDescriptorHeap(shaderViewDescriptorHeap), mCPUDescriptorHeapPage(mShaderViewDescriptorHeap.allocatePage())
	{
		dx12::RootSignature rootSignature = createRootSignature(layout);
		mContext.setRootSignature(rootSignature);
	}
	dx12::RootSignature ShaderInput::createRootSignature(const std::vector<Parameters>& layout)
	{
		dx12::RootSignature();
	}
}

