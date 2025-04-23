#include "CommandContext.h"

#include "dx12/types/PrimitiveTopology.h"

#include "Log.h"

namespace graphics
{
    CommandContext::CommandContext(const CommandListType type, dx12::CommandList& commandList)
        : CommandObject(type), mCommandList(commandList) {}

    void CommandContext::reset(dx12::CommandAllocator& commandAllocator)
    {
        mCommandList->Reset(commandAllocator.get(), nullptr);
    }

    void CommandContext::copyBuffer(dx12::GPUResource& destination, dx12::GPUResource& source)
    {
        mCommandList.transitionResource(source, GPUResourceStates::CopySource);
        mCommandList.transitionResource(destination, GPUResourceStates::CopyDestination);
        flushResourceBarriers();
        mCommandList->CopyResource(destination.get(), source.get());
    }

    void CommandContext::copyBufferRegion(dx12::GPUResource& destination, const size_t destinationOffset, dx12::GPUResource& source, const size_t sourceOffset, const size_t numBytes)
    {
        mCommandList.transitionResource(source, GPUResourceStates::CopySource);
        mCommandList.transitionResource(destination, GPUResourceStates::CopyDestination);
        flushResourceBarriers();
        mCommandList->CopyBufferRegion(destination.get(), destinationOffset, source.get(), sourceOffset, numBytes);
    }

    void CommandContext::copySubresource(dx12::GPUResource& destination, const unsigned int destinationSubIndex, dx12::GPUResource& source, const unsigned int sourceSubIndex)
    {
        flushResourceBarriers();

        D3D12_TEXTURE_COPY_LOCATION destinationLocation =
        {
            destination.get(),
            D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
            destinationSubIndex
        };

        D3D12_TEXTURE_COPY_LOCATION sourceLocation =
        {
            source.get(),
            D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
            sourceSubIndex
        };

        mCommandList->CopyTextureRegion(&destinationLocation, 0, 0, 0, &sourceLocation, nullptr);
    }

    void CommandContext::copyTextureRegion(dx12::GPUResource& destination, const unsigned int destinationX, const unsigned int destinationY, const unsigned int destinationZ, dx12::GPUResource& source, Box& sourceRect)
    {
        mCommandList.transitionResource(source, GPUResourceStates::CopySource);
        mCommandList.transitionResource(destination, GPUResourceStates::CopyDestination);
        flushResourceBarriers();

        D3D12_TEXTURE_COPY_LOCATION destLoc = CD3DX12_TEXTURE_COPY_LOCATION(destination.get(), 0);
        D3D12_TEXTURE_COPY_LOCATION srcLoc = CD3DX12_TEXTURE_COPY_LOCATION(source.get(), 0);

        const D3D12_BOX box{
            sourceRect.left,
            sourceRect.top,
            sourceRect.front,
            sourceRect.right,
            sourceRect.bottom,
            1,
        };

        mCommandList->CopyTextureRegion(&destLoc, destinationX, destinationY, destinationZ, &srcLoc, &box);
    }

    void CommandContext::setPipelineState(dx12::PipelineState& pso)
    {
        ID3D12PipelineState* pipelineState = pso.get();

        mCommandList->SetPipelineState(pipelineState);
    }

    void CommandContext::setPredication(dx12::GPUResource& buffer, const size_t bufferOffset, bool equalZero)
    {
        mCommandList->SetPredication(
            buffer.get(), bufferOffset,
            equalZero ? D3D12_PREDICATION_OP_EQUAL_ZERO : D3D12_PREDICATION_OP_NOT_EQUAL_ZERO);
    }

    /*uint64_t CommandContext::finish(bool waitForCompletion)
    {
        flushResourceBarriers();

        HASSERT(mCurrentCommandAllocator != nullptr, "Command allocator not found.");

        dx12::CommandQueue& queue = mCommandListManager.getCommandQueue(mType);
        uint64_t fence = queue.executeCommandList(mCommandList);
        mCommandListManager.discardAllocator(*mCurrentCommandAllocator);
        mCurrentCommandAllocator = nullptr;

        if (waitForCompletion)
        {
            mCommandListManager.waitForFence(fence);
        }

        return fence;
    }*/

    CommandContext::~CommandContext()
    {
    }
}

namespace graphics
{
    GraphicContext::GraphicContext(dx12::CommandList& commandList)
        : CommandContext(CommandListType::Direct, commandList) {}

    void GraphicContext::clearColor(dx12::DescriptorHandle& target, float color[4], const Box* rect)
    {
        flushResourceBarriers();
        const D3D12_RECT d3dRect{
            rect->left,
            rect->top,
            rect->right,
            rect->bottom
        };
        mCommandList->ClearRenderTargetView(target, color, (rect == nullptr) ? 0 : 1, &d3dRect);
    }

    void GraphicContext::setRootSignature(const dx12::RootSignature& rootSignature)
    {
        mCommandList->SetGraphicsRootSignature(rootSignature.get());
    }

    void GraphicContext::setRenderTargets(unsigned int count, const dx12::DescriptorHandle rtv)
    {
        mCommandList->OMSetRenderTargets(count, &rtv, FALSE, nullptr);
    }

    void GraphicContext::setRenderTargets(unsigned int count, const dx12::DescriptorHandle rtv, const dx12::DescriptorHandle dsv)
    {
        mCommandList->OMSetRenderTargets(count, &rtv, FALSE, &dsv);
    }

    void GraphicContext::setViewport(const ViewPort& viewport)
    {
        const D3D12_VIEWPORT d3dViewPort{
            viewport.x,
            viewport.y,
            viewport.width,
            viewport.height,
            viewport.minDepth,
            viewport.maxDepth
        };

        mCommandList->RSSetViewports(1, &d3dViewPort);
    }

    void GraphicContext::setScissor(const Box& rect)
    {
        HASSERT(rect.left < rect.right && rect.top < rect.bottom, "Invalid scissor rect.");

        const D3D12_RECT d3dRect{
            rect.left,
            rect.top,
            rect.right,
            rect.bottom
        };

        mCommandList->RSSetScissorRects(1, &d3dRect);
    }

    void GraphicContext::setStencilRef(const unsigned int stencilRef)
    {
        mCommandList->OMSetStencilRef(stencilRef);
    }

    void GraphicContext::setBlendFactor(const float blendFactor[4])
    {
        mCommandList->OMSetBlendFactor(blendFactor);
    }

    void GraphicContext::setPrimitiveTopology(const PrimitiveTopology topology)
    {
        mCommandList->IASetPrimitiveTopology(dx12::toD3DPrimitiveTopology(topology));
    }

    void GraphicContext::setConstant(const unsigned int rootIndex, dx12::DWParam value, const unsigned int offset)
    {
        mCommandList->SetGraphicsRoot32BitConstant(rootIndex, value.uintValue, offset);
    }

    void GraphicContext::setConstants(const unsigned int rootIndex, dx12::DWParam x)
    {
        setConstant(rootIndex, x, 0);
    }

    void GraphicContext::setConstants(const unsigned int rootIndex, dx12::DWParam x, dx12::DWParam y)
    {
        setConstant(rootIndex, x, 0);
        setConstant(rootIndex, y, 1);
    }

    void GraphicContext::setConstants(const unsigned int rootIndex, dx12::DWParam x, dx12::DWParam y, dx12::DWParam z)
    {
        setConstant(rootIndex, x, 0);
        setConstant(rootIndex, y, 1);
        setConstant(rootIndex, z, 2);
    }

    void GraphicContext::setConstants(const unsigned int rootIndex, dx12::DWParam x, dx12::DWParam y, dx12::DWParam z, dx12::DWParam w)
    {
        setConstant(rootIndex, x, 0);
        setConstant(rootIndex, y, 1);
        setConstant(rootIndex, z, 2);
        setConstant(rootIndex, w, 3);
    }

    void GraphicContext::setConstants(const unsigned int rootIndex, const size_t count, const void* pConstants)
    {
        mCommandList->SetGraphicsRoot32BitConstants(rootIndex, count, pConstants, 0);
    }

    void GraphicContext::setBufferCBV(const unsigned int rootIndex, uint64_t cbv)
    {
        mCommandList->SetGraphicsRootConstantBufferView(rootIndex, (D3D12_GPU_VIRTUAL_ADDRESS)cbv);
    }

    void GraphicContext::setBufferSRV(const unsigned int rootIndex, const dx12::GPUBuffer& srv, const uint64_t offset)
    {
        mCommandList->SetGraphicsRootShaderResourceView(rootIndex, srv.getGPUVirtualAddress() + offset);
    }

    void GraphicContext::setDescriptorTable(const unsigned int rootIndex, const dx12::DescriptorHandle& firstHandle)
    {
        mCommandList->SetGraphicsRootDescriptorTable(rootIndex, firstHandle);
    }

    void GraphicContext::setIndexBuffer(const dx12::BufferView& view)
    {
        const D3D12_INDEX_BUFFER_VIEW d3dView = dx12::BufferView::toD3DIndexBufferView(view);
        mCommandList->IASetIndexBuffer(&d3dView);
    }

    void GraphicContext::setVertexBuffer(unsigned int slot, const dx12::BufferView views[], const size_t count)
    {
        std::vector<D3D12_VERTEX_BUFFER_VIEW> d3dViews(count);

        for (unsigned int i = 0; i < count; ++i)
        {
            d3dViews[i] = dx12::BufferView::toD3DVertexBufferView(views[i]);
        }

        mCommandList->IASetVertexBuffers(slot, count, d3dViews.data());
    }

    void GraphicContext::draw(const unsigned int vertexCount, const unsigned int vertexStartOffset)
    {
        drawInstanced(vertexCount, 1, vertexStartOffset, 0);
    }

    void GraphicContext::drawIndexed(const unsigned int indexCount, const unsigned int startIndexLocation, const int baseVertexLocation)
    {
        drawIndexedInstanced(indexCount, 1, startIndexLocation, baseVertexLocation, 0);
    }

    void GraphicContext::drawInstanced(const unsigned int vertexCountPerInstance, const unsigned int instanceCount, const unsigned int startVertexLocation, const unsigned int startInstanceLocation)
    {
        flushResourceBarriers();
        mCommandList->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
    }

    void GraphicContext::drawIndexedInstanced(const unsigned int indexCountPerInstance, const unsigned int instanceCount, const unsigned int startIndexLocation, const int baseVertexLocation, const unsigned int startInstanceLocation)
    {
        flushResourceBarriers();
        mCommandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
    }
}
