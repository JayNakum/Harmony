#pragma once

#include "common.h"

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
    * This parent class is inherited by GraphicsContext and ComputeContext.
    */
    class CommandContext : public CommandObject
    {
    protected:
        // Command context is only ever meant to be created, initialized or reset via the Context Manager
        CommandContext(const CommandListType type, Device& device);

    public:
        void setName(const std::string& name);
        inline const std::string getName() const { return mName; }

        inline CommandList& commandList() { return mCommandList; }

        void reset(const uint64_t completedFence);

        inline const Device& getDevice() const { return mDevice; }
        inline Device& getDevice() { return mDevice; }

        inline void transitionBarrier(GPUResource& resource, const GPUResourceStates newState, const bool flushImmediate = false)
        {
            mCommandList.transitionBarrierFull(resource, newState, flushImmediate);
        }
        inline void uavBarrier(GPUResource& resource, const bool flushImmediate = false)
        {
            mCommandList.uavBarrier(resource, flushImmediate);
        }
        inline void aliasBarrier(GPUResource& resourceBefore, GPUResource& resourceAfter, const bool flushImmediate = false)
        {
            mCommandList.aliasBarrier(resourceBefore, resourceAfter, flushImmediate);
        }

        void copyBuffer(GPUResource& destination, GPUResource& source);

        void copyBufferRegion(GPUResource& destination, const size_t destinationOffset,
            GPUResource& source, const size_t sourceOffset,
            const size_t numBytes);

        void copySubresource(GPUResource& destination, const unsigned int destinationSubIndex,
            GPUResource& source, const unsigned int sourceSubIndex);

        void copyTextureRegion(GPUResource& destination, const unsigned int destinationX,
            const unsigned int destinationY, const unsigned int destinationZ,
            GPUResource& source, Box& sourceRect);

        void setPredication(GPUResource& buffer, const size_t bufferOffset, bool equalZero);

        void bindDescriptorHeap(const DescriptorHeap& heap);

        // flush existing commands to the GPU and keep the context alive
        void finish(const uint64_t completedFenceValue);

        inline void flushResourceBarriers() { mCommandList.flushResourceBarriers(); }

        virtual ~CommandContext() = default;

        static const Blob compileShader(const char* shaderName, const char* shaderCode, const char* targetProfile, const char* entryPoint);

    private:
        std::string mName;
        Device& mDevice;

    protected:
        CommandAllocatorPool mCommandAllocatorPool;
        CommandAllocator* mCurrentCommandAllocator;
        CommandList::Description mCmdListDesc;
        CommandList mCommandList;
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
        GraphicContext(Device& device);

        void setPipelineState(GraphicsPipelineState& pso);
        void setRootSignature(const RootSignature& rootSignature);
        void clearColor(DescriptorHandle& target, const float color[4]);

        void setRenderTargets(const DescriptorHandle rtv, unsigned int count = 1);
        void setRenderTargets(unsigned int count, const DescriptorHandle rtv, const DescriptorHandle dsv);

        void setViewport(const ViewPort& viewport);
        void setScissor(const Box& rect);
        void setStencilRef(const unsigned int stencilRef);
        void setBlendFactor(const float blendFactor[4]);
        void setPrimitiveTopology(const PrimitiveTopology Topology);

        void setIndexBuffer(const BufferView& view);
        void setVertexBuffers(unsigned int slot, const BufferView views[], const size_t count = 1);

        void setConstant(const unsigned int rootIndex, DWParam value, const unsigned int offset = 0);
        void setConstants(const unsigned int rootIndex, const size_t count, const void* pConstants, const unsigned int offset = 0);
        void setDescriptorTable(const unsigned int rootIndex, const DescriptorHandle& firstHandle);

        void draw(const unsigned int vertexCount, const unsigned int vertexStartOffset = 0);
        void drawIndexed(const unsigned int indexCount, const unsigned int startIndexLocation = 0, const int baseVertexLocation = 0);
        void drawInstanced(const unsigned int vertexCountPerInstance, const unsigned int instanceCount,
            const unsigned int startVertexLocation = 0, const unsigned int startInstanceLocation = 0);
        void drawIndexedInstanced(const unsigned int indexCountPerInstance, const unsigned int instanceCount, const unsigned int startIndexLocation,
            const int baseVertexLocation, const unsigned int startInstanceLocation);

        /*
        void drawIndirect(Buffer& argumentBuffer, uint64_t argumentBufferOffset = 0);
        void executeIndirect(CommandSignature& commandSig, Buffer& argumentBuffer, uint64_t argumentStartOffset = 0,
                             uint32_t maxCommands = 1, Buffer* commandCounterBuffer = nullptr, uint64_t counterOffset = 0);
        */
    };
}

namespace graphics
{
    class ComputeContext : public CommandContext
    {
    public:
        ComputeContext(Device& device);

        void setPipelineState(ComputePipelineState& pso);
        void setRootSignature(const RootSignature& rootSignature);

        void setConstant(const unsigned int rootIndex, DWParam value, const unsigned int offset = 0);
        void setConstants(const unsigned int rootIndex, const size_t count, const void* pConstants, const unsigned int offset = 0);
        void setDescriptorTable(const unsigned int rootIndex, const DescriptorHandle& firstHandle);

        void dispatch(const unsigned int groupCountX = 1, const unsigned int groupCountY = 1, const unsigned int groupCountZ = 1);
        void dispatch1D(const unsigned int threadCountX, const unsigned int groupSizeX = 64);
        void dispatch2D(const unsigned int threadCountX, const unsigned int threadCountY, const unsigned int groupSizeX = 8, const unsigned int groupSizeY = 8);
        void dispatch3D(const unsigned int threadCountX, const unsigned int threadCountY, const unsigned int threadCountZ, const unsigned int groupSizeX, const unsigned int groupSizeY, const unsigned int groupSizeZ);

        /*
        void dispatchIndirect(Buffer& argumentBuffer, uint64_t argumentBufferOffset = 0);
        void executeIndirect(CommandSignature& commandSig, Buffer& argumentBuffer, uint64_t argumentStartOffset = 0,
                             uint32_t maxCommands = 1, Buffer* commandCounterBuffer = nullptr, uint64_t counterOffset = 0);
        */
    };
}

namespace graphics
{
    class CopyContext : public CommandContext
    {
    public:
        CopyContext(Device& device);
    };
}
