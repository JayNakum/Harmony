#pragma once

#include "common.h"

#include "CommandObject.h"

#include "DynamicDescriptorHeap.h"
#include "CommandAllocatorPool.h"

#include "dx12/DWParam.h"
#include "dx12/CommandList.h"
#include "dx12/DescriptorHeap.h"
#include "dx12/GPUResource.h"
#include "dx12/UploadBuffer.h"
#include "dx12/GPUBuffer.h"
#include "dx12/GPUTexture.h"
#include "dx12/PipelineState.h"

namespace graphics
{
    struct Box
    {
        unsigned int left;
        unsigned int top;
        unsigned int front;
        unsigned int right;
        unsigned int bottom;
        unsigned int back = 1;
    };

    /*
    * This is the classes that connects graphics classes with DX12.
    * Contains the state and resources needed to record and execute commands on the GPU.
    * This parent class is inherited by GraphicsContext and // TODO: ComputeContext.
    */
    class CommandContext : public CommandObject, NonCopyable
    {
    protected:
        // Command context is only ever meant to be created, initialized or reset via the Context Manager
        CommandContext(const CommandListType type, dx12::Device& device,
                       DynamicDescriptorHeap& descriptorHeap);

    public:
        void setName(const std::string& name);
        inline const std::string getName() const { return mName; }

        inline dx12::CommandList commandList() { return mCommandList; }

        void reset(const uint64_t completedFence);

        inline const dx12::Device& getDevice() const { return mDevice; }
        inline dx12::Device& getDevice() { return mDevice; }

        inline const DescriptorHeapWithAllocator::Page& getShaderViewHeapPage() const { return mShaderViewsHeapCPUPage; }
        inline DescriptorHeapWithAllocator::Page& getShaderViewHeapPage() { return mShaderViewsHeapCPUPage; }

        void transitionResource(dx12::GPUResource& resource, const GPUResourceStates newState, const bool flushImmediate = false);

        void copyBuffer(dx12::GPUResource& destination, dx12::GPUResource& source);

        void copyBufferRegion(dx12::GPUResource& destination, const size_t destinationOffset,
                              dx12::GPUResource& source, const size_t sourceOffset,
                              const size_t numBytes);

        void copySubresource(dx12::GPUResource& destination, const unsigned int destinationSubIndex,
                             dx12::GPUResource& source, const unsigned int sourceSubIndex);

        void copyTextureRegion(dx12::GPUResource& destination, const unsigned int destinationX,
                               const unsigned int destinationY, const unsigned int destinationZ,
                               dx12::GPUResource& source, Box& sourceRect);

        void setPipelineState(dx12::PipelineState& pso);

        void setPredication(dx12::GPUResource& buffer, const size_t bufferOffset, bool equalZero);

        const dx12::DescriptorHandle copyDescriptorsToGPU(const dx12::DescriptorHandle& allocationStart, const unsigned int descriptorsCount = 1);
        void bindDescriptorHeaps();

        // flush existing commands to the GPU and keep the context alive
        void finish(const uint64_t completedFenceValue);

        inline void flushResourceBarriers() { mCommandList.flushResourceBarriers(); }

        virtual ~CommandContext();

    private:
        std::string mName;
        dx12::Device& mDevice;
        DynamicDescriptorHeap& mShaderViewsHeap;
        DescriptorHeapWithAllocator::Page mShaderViewsHeapCPUPage;

    protected:
        CommandAllocatorPool mCommandAllocatorPool;
        dx12::CommandAllocator* mCurrentCommandAllocator;
        dx12::CommandList mCommandList;
    };
}

namespace graphics
{
    struct ViewPort
    {
        float x;
        float y;
        float width;
        float height;
        float minDepth;
        float maxDepth;
    };

    class GraphicContext : public CommandContext
    {
    public:
        GraphicContext(dx12::Device& device, DynamicDescriptorHeap& descriptorHeap);

        static const dx12::Blob compileShader(const char* shaderName, const char* shaderCode, const char* targetProfile, const char* entryPoint = "main");

        void clearColor(dx12::DescriptorHandle& target, const float color[4]);
        void setRootSignature(const dx12::RootSignature& rootSignature);

        void setRenderTargets(const dx12::DescriptorHandle rtv, unsigned int count = 1);
        void setRenderTargets(unsigned int count, const dx12::DescriptorHandle rtv, const dx12::DescriptorHandle dsv);

        void setViewport(const ViewPort& viewport);
        void setScissor(const Box& rect);
        void setStencilRef(const unsigned int stencilRef);
        void setBlendFactor(const float blendFactor[4]);
        void setPrimitiveTopology(const PrimitiveTopology Topology);

        void setConstant(const unsigned int rootIndex, dx12::DWParam value, const unsigned int offset);
        void setConstants(const unsigned int rootIndex, dx12::DWParam x);
        void setConstants(const unsigned int rootIndex, dx12::DWParam x, dx12::DWParam y);
        void setConstants(const unsigned int rootIndex, dx12::DWParam x, dx12::DWParam y, dx12::DWParam z);
        void setConstants(const unsigned int rootIndex, dx12::DWParam x, dx12::DWParam y, dx12::DWParam z, dx12::DWParam w);
        void setConstants(const unsigned int rootIndex, const size_t count, const void* pConstants);

        void setBufferCBV(const unsigned int rootIndex, uint64_t cbv);
        void setBufferSRV(const unsigned int rootIndex, const dx12::GPUBuffer& srv, const uint64_t offset = 0);
        // void setBufferUAV(const unsigned int rootIndex, const dx12::DescriptorHandle& uav, const unsigned int offset = 0);
        void setDescriptorTable(const unsigned int rootIndex, const dx12::DescriptorHandle& firstHandle);

        void setIndexBuffer(const dx12::BufferView& view);
        void setVertexBuffer(unsigned int slot, const dx12::BufferView views[], const size_t count = 1);

        void draw(const unsigned int vertexCount, const unsigned int vertexStartOffset = 0);
        void drawIndexed(const unsigned int indexCount, const unsigned int startIndexLocation = 0, const int baseVertexLocation = 0);
        void drawInstanced(const unsigned int vertexCountPerInstance, const unsigned int instanceCount,
                           const unsigned int startVertexLocation = 0, const unsigned int startInstanceLocation = 0);
        void drawIndexedInstanced(const unsigned int indexCountPerInstance, const unsigned int instanceCount, const unsigned int startIndexLocation,
                                  const int baseVertexLocation, const unsigned int startInstanceLocation);

        /*void drawIndirect(dx12::GPUBuffer& argumentBuffer, uint64_t argumentBufferOffset = 0);
        void executeIndirect(dx12::CommandSignature& commandSig, dx12::GPUBuffer& argumentBuffer, uint64_t argumentStartOffset = 0,
                             uint32_t maxCommands = 1, dx12::GPUBuffer* commandCounterBuffer = nullptr, uint64_t counterOffset = 0);*/
    };
}
