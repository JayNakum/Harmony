#pragma once

#include "common.h"
#include "Log.h"

#include "CommandObject.h"

#include "CommandAllocatorPool.h"

#include "dx12/DWParam.h"
#include "dx12/CommandList.h"
#include "dx12/DescriptorHeap.h"
#include "dx12/GPUResource.h"
#include "dx12/Buffer.h"
#include "dx12/Texture.h"
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
        CommandContext(const CommandListType type, dx12::Device& device);

    public:
        void setName(const std::string& name);
        inline const std::string getName() const { return mName; }

        inline dx12::CommandList commandList() { return mCommandList; }

        void reset(const uint64_t completedFence);

        inline const dx12::Device& getDevice() const { return mDevice; }
        inline dx12::Device& getDevice() { return mDevice; }

        inline void transitionBarrier(dx12::GPUResource& resource, const GPUResourceStates newState, const bool flushImmediate = false)
        {
            mCommandList.transitionBarrierFull(resource, newState, flushImmediate);
        }
        inline void uavBarrier(dx12::GPUResource& resource, const bool flushImmediate = false)
        {
            mCommandList.uavBarrier(resource, flushImmediate);
        }
        inline void aliasBarrier(dx12::GPUResource& resourceBefore, dx12::GPUResource& resourceAfter, const bool flushImmediate = false)
        {
            mCommandList.aliasBarrier(resourceBefore, resourceAfter, flushImmediate);
        }

        void copyBuffer(dx12::GPUResource& destination, dx12::GPUResource& source);

        void copyBufferRegion(dx12::GPUResource& destination, const size_t destinationOffset,
                              dx12::GPUResource& source, const size_t sourceOffset,
                              const size_t numBytes);

        void copySubresource(dx12::GPUResource& destination, const unsigned int destinationSubIndex,
                             dx12::GPUResource& source, const unsigned int sourceSubIndex);

        void copyTextureRegion(dx12::GPUResource& destination, const unsigned int destinationX,
                               const unsigned int destinationY, const unsigned int destinationZ,
                               dx12::GPUResource& source, Box& sourceRect);

        void setPredication(dx12::GPUResource& buffer, const size_t bufferOffset, bool equalZero);

        void bindDescriptorHeap(const dx12::DescriptorHeap& heap);

        // flush existing commands to the GPU and keep the context alive
        void finish(const uint64_t completedFenceValue);

        inline void flushResourceBarriers() { mCommandList.flushResourceBarriers(); }

        virtual ~CommandContext() = default;

        static const dx12::Blob compileShader(const char* shaderName, const char* shaderCode, const char* targetProfile, const char* entryPoint = "main");

    private:
        std::string mName;
        dx12::Device& mDevice;

    protected:
        CommandAllocatorPool mCommandAllocatorPool;
        dx12::CommandAllocator* mCurrentCommandAllocator;
        dx12::CommandList::Description mCmdListDesc;
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
        GraphicContext(dx12::Device& device);

        void setPipelineState(dx12::GraphicsPipelineState& pso);
        void setRootSignature(const dx12::RootSignature& rootSignature);
        void clearColor(dx12::DescriptorHandle& target, const float color[4]);

        void setRenderTargets(const dx12::DescriptorHandle rtv, unsigned int count = 1);
        void setRenderTargets(unsigned int count, const dx12::DescriptorHandle rtv, const dx12::DescriptorHandle dsv);

        void setViewport(const ViewPort& viewport);
        void setScissor(const Box& rect);
        void setStencilRef(const unsigned int stencilRef);
        void setBlendFactor(const float blendFactor[4]);
        void setPrimitiveTopology(const PrimitiveTopology Topology);

        void setIndexBuffer(const dx12::BufferView& view);
        void setVertexBuffers(unsigned int slot, const dx12::BufferView views[], const size_t count = 1);

        void setConstant(const unsigned int rootIndex, dx12::DWParam value, const unsigned int offset = 0);
        void setConstants(const unsigned int rootIndex, const size_t count, const void* pConstants, const unsigned int offset = 0);
        void setDescriptorTable(const unsigned int rootIndex, const dx12::DescriptorHandle& firstHandle);

        void draw(const unsigned int vertexCount, const unsigned int vertexStartOffset = 0);
        void drawIndexed(const unsigned int indexCount, const unsigned int startIndexLocation = 0, const int baseVertexLocation = 0);
        void drawInstanced(const unsigned int vertexCountPerInstance, const unsigned int instanceCount,
                           const unsigned int startVertexLocation = 0, const unsigned int startInstanceLocation = 0);
        void drawIndexedInstanced(const unsigned int indexCountPerInstance, const unsigned int instanceCount, const unsigned int startIndexLocation,
                                  const int baseVertexLocation, const unsigned int startInstanceLocation);

        /*
        void drawIndirect(dx12::Buffer& argumentBuffer, uint64_t argumentBufferOffset = 0);
        void executeIndirect(dx12::CommandSignature& commandSig, dx12::Buffer& argumentBuffer, uint64_t argumentStartOffset = 0,
                             uint32_t maxCommands = 1, dx12::Buffer* commandCounterBuffer = nullptr, uint64_t counterOffset = 0);
        */
    };
}

namespace graphics
{
    class ComputeContext : public CommandContext
    {
    public:
        ComputeContext(dx12::Device& device);

        void setPipelineState(dx12::ComputePipelineState& pso);
        void setRootSignature(const dx12::RootSignature& rootSignature);

        void setConstant(const unsigned int rootIndex, dx12::DWParam value, const unsigned int offset = 0);
        void setConstants(const unsigned int rootIndex, const size_t count, const void* pConstants, const unsigned int offset = 0);
        void setDescriptorTable(const unsigned int rootIndex, const dx12::DescriptorHandle& firstHandle);

        void dispatch(const unsigned int groupCountX = 1, const unsigned int groupCountY = 1, const unsigned int groupCountZ = 1);
        void dispatch1D(const unsigned int threadCountX, const unsigned int groupSizeX = 64);
        void dispatch2D(const unsigned int threadCountX, const unsigned int threadCountY, const unsigned int groupSizeX = 8, const unsigned int groupSizeY = 8);
        void dispatch3D(const unsigned int threadCountX, const unsigned int threadCountY, const unsigned int threadCountZ, const unsigned int groupSizeX, const unsigned int groupSizeY, const unsigned int groupSizeZ);

        /*
        void dispatchIndirect(dx12::Buffer& argumentBuffer, uint64_t argumentBufferOffset = 0);
        void executeIndirect(dx12::CommandSignature& commandSig, dx12::Buffer& argumentBuffer, uint64_t argumentStartOffset = 0,
                             uint32_t maxCommands = 1, dx12::Buffer* commandCounterBuffer = nullptr, uint64_t counterOffset = 0);
        */
    };
}
