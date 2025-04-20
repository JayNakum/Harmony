#include "CommandContext.h"

#include "dx12/types/PrimitiveTopology.h"

#include "Log.h"

namespace graphics
{
    CommandContext::CommandContext(const CommandListType type, CommandListManager& commandListManager)
        : CommandObject(type), 
        mCommandList(nullptr), 
        mCurrentCommandAllocator(nullptr),
        mCommandListManager(commandListManager),
        mCurrentDescriptorHeaps{}
    {
        mCommandListManager.createNewCommandList(mType, mCommandList, mCurrentCommandAllocator);
    }

    void CommandContext::bindDescriptorHeaps() const
    {
        std::array<ID3D12DescriptorHeap*, descriptorHeapTypeCount> heapsToBind{};
        unsigned int nonNullHeaps = 0;
        for (dx12::DescriptorHeap* heap : mCurrentDescriptorHeaps)
        {
            if (heap != nullptr)
            {
                heapsToBind[nonNullHeaps++] = heap->get();
            }
        }

        if (nonNullHeaps > 0)
        {
            getCommandList()->SetDescriptorHeaps(nonNullHeaps, heapsToBind.data());
        }
    }

    void CommandContext::reset()
    {
        // We only call Reset() on previously freed contexts.
        // The command list persists and will be reused, but we must request a new allocator.
        HASSERT(mCommandList != nullptr, "Command list not found.");
        HASSERT_SOFT(mCurrentCommandAllocator == nullptr, "Command allocator is not null. It will get replaced by a new one.");

        mCurrentCommandAllocator = &(mCommandListManager.requestAllocator(mType));

        getCommandList()->Reset(mCurrentCommandAllocator->get(), nullptr);

        bindDescriptorHeaps();
    }

    void CommandContext::copyBuffer(dx12::GPUResource& destination, dx12::GPUResource& source) const
    {
        getCommandList().transitionResource(source, GPUResourceStates::CopySource);
        getCommandList().transitionResource(destination, GPUResourceStates::CopyDestination);
        flushResourceBarriers();
        getCommandList()->CopyResource(destination.get(), source.get());
    }

    void CommandContext::copyBufferRegion(dx12::GPUResource& destination, const size_t destinationOffset, dx12::GPUResource& source, const size_t sourceOffset, const size_t numBytes) const
    {
        getCommandList().transitionResource(source, GPUResourceStates::CopySource);
        getCommandList().transitionResource(destination, GPUResourceStates::CopyDestination);
        flushResourceBarriers();
        getCommandList()->CopyBufferRegion(destination.get(), destinationOffset, source.get(), sourceOffset, numBytes);
    }

    void CommandContext::copySubresource(dx12::GPUResource& destination, const unsigned int destinationSubIndex, dx12::GPUResource& source, const unsigned int sourceSubIndex) const
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

        getCommandList()->CopyTextureRegion(&destinationLocation, 0, 0, 0, &sourceLocation, nullptr);
    }

    void CommandContext::copyTextureRegion(dx12::GPUResource& destination, const unsigned int destinationX, const unsigned int destinationY, const unsigned int destinationZ, dx12::GPUResource& source, Box& sourceRect) const
    {
        getCommandList().transitionResource(source, GPUResourceStates::CopySource);
        getCommandList().transitionResource(destination, GPUResourceStates::CopyDestination);
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

        getCommandList()->CopyTextureRegion(&destLoc, destinationX, destinationY, destinationZ, &srcLoc, &box);
    }

    void CommandContext::setDescriptorHeap(dx12::DescriptorHeap& descriptorHeap)
    {
        dx12::DescriptorHeap* currentDescriptorHeap = mCurrentDescriptorHeaps[enum_cast(descriptorHeap.getDescriptorHeapType())];

        HASSERT_SOFT(currentDescriptorHeap != &descriptorHeap, "Trying to set the same descriptor heap again.");

        currentDescriptorHeap = &descriptorHeap;
    }

    void CommandContext::setPipelineState(dx12::PipelineState& pso) const
    {
        ID3D12PipelineState* pipelineState = pso.get();

        getCommandList()->SetPipelineState(pipelineState);
    }

    void CommandContext::setPredication(dx12::GPUResource& buffer, const size_t bufferOffset, bool equalZero)
    {
        getCommandList()->SetPredication(
            buffer.get(), bufferOffset,
            equalZero ? D3D12_PREDICATION_OP_EQUAL_ZERO : D3D12_PREDICATION_OP_NOT_EQUAL_ZERO);
    }

    uint64_t CommandContext::finish(bool waitForCompletion)
    {
        flushResourceBarriers();

        HASSERT(mCurrentCommandAllocator != nullptr, "Command allocator not found.");

        dx12::CommandQueue& queue = mCommandListManager.getCommandQueue(mType);
        uint64_t fence = queue.executeCommandList(getCommandList());
        mCommandListManager.discardAllocator(*mCurrentCommandAllocator);
        mCurrentCommandAllocator = nullptr;

        if (waitForCompletion)
        {
            mCommandListManager.waitForFence(fence);
        }

        return fence;
    }

    CommandContext::~CommandContext()
    {
    }
}

namespace graphics
{
    GraphicContext::GraphicContext(CommandListManager& commandListManager)
        : CommandContext(CommandListType::Direct, commandListManager) {}

    void GraphicContext::clearColor(dx12::DescriptorHandle& target, float color[4], const Box* rect)
    {
        flushResourceBarriers();
        const D3D12_RECT d3dRect{
            rect->left,
            rect->top,
            rect->right,
            rect->bottom
        };
        getCommandList()->ClearRenderTargetView(target, color, (rect == nullptr) ? 0 : 1, &d3dRect);
    }

    void GraphicContext::setRootSignature(const dx12::RootSignature& rootSignature)
    {
        getCommandList()->SetGraphicsRootSignature(rootSignature.get());
    }

    void GraphicContext::setRenderTargets(unsigned int count, const dx12::DescriptorHandle rtv)
    {
        getCommandList()->OMSetRenderTargets(count, &rtv, FALSE, nullptr);
    }

    void GraphicContext::setRenderTargets(unsigned int count, const dx12::DescriptorHandle rtv, const dx12::DescriptorHandle dsv)
    {
        getCommandList()->OMSetRenderTargets(count, &rtv, FALSE, &dsv);
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

        getCommandList()->RSSetViewports(1, &d3dViewPort);
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

        getCommandList()->RSSetScissorRects(1, &d3dRect);
    }

    void GraphicContext::setStencilRef(const unsigned int stencilRef)
    {
        getCommandList()->OMSetStencilRef(stencilRef);
    }

    void GraphicContext::setBlendFactor(const float blendFactor[4])
    {
        getCommandList()->OMSetBlendFactor(blendFactor);
    }

    void GraphicContext::setPrimitiveTopology(const PrimitiveTopology topology)
    {
        getCommandList()->IASetPrimitiveTopology(dx12::toD3DPrimitiveTopology(topology));
    }

    void GraphicContext::setConstant(const unsigned int rootIndex, dx12::DWParam value, const unsigned int offset)
    {
        getCommandList()->SetGraphicsRoot32BitConstant(rootIndex, value.uintValue, offset);
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
        getCommandList()->SetGraphicsRoot32BitConstants(rootIndex, count, pConstants, 0);
    }

    void GraphicContext::setBufferCBV(const unsigned int rootIndex, uint64_t cbv)
    {
        getCommandList()->SetGraphicsRootConstantBufferView(rootIndex, (D3D12_GPU_VIRTUAL_ADDRESS)cbv);
    }

    void GraphicContext::setBufferSRV(const unsigned int rootIndex, const dx12::GPUBuffer& srv, const uint64_t offset)
    {
        getCommandList()->SetGraphicsRootShaderResourceView(rootIndex, srv.getGPUVirtualAddress() + offset);
    }

    void GraphicContext::setDescriptorTable(const unsigned int rootIndex, const dx12::DescriptorHandle& firstHandle)
    {
        getCommandList()->SetGraphicsRootDescriptorTable(rootIndex, firstHandle);
    }

    void GraphicContext::setIndexBuffer(const dx12::BufferView& view)
    {
        const D3D12_INDEX_BUFFER_VIEW d3dView = dx12::BufferView::toD3DIndexBufferView(view);
        getCommandList()->IASetIndexBuffer(&d3dView);
    }

    void GraphicContext::setVertexBuffer(unsigned int slot, const dx12::BufferView views[], const size_t count)
    {
        std::vector<D3D12_VERTEX_BUFFER_VIEW> d3dViews(count);

        for (unsigned int i = 0; i < count; ++i)
        {
            d3dViews[i] = dx12::BufferView::toD3DVertexBufferView(views[i]);
        }

        getCommandList()->IASetVertexBuffers(slot, count, d3dViews.data());
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
        getCommandList()->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
    }

    void GraphicContext::drawIndexedInstanced(const unsigned int indexCountPerInstance, const unsigned int instanceCount, const unsigned int startIndexLocation, const int baseVertexLocation, const unsigned int startInstanceLocation)
    {
        flushResourceBarriers();
        getCommandList()->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
    }
}
