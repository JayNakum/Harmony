//#include "DescriptorAllocator.h"
//
//#include "Renderer.h"
//#include "CommandContext.h"
//
//#include "Log.h"
//
//
//namespace graphics
//{
//    using namespace dx12;
//
//    DescriptorHeapPool::DescriptorHeapPool(const Description& description)
//        : mDescription(description)
//    {}
//
//    DescriptorHeap& DescriptorHeapPool::requestDescriptorHeap()
//    {
//        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);
//
//        updateAvailableQueue();
//
//        if (!mAvailableQueue.empty())
//        {
//            DescriptorHeap* descriptorHeap = mAvailableQueue.front();
//            mAvailableQueue.pop();
//            return *descriptorHeap;
//        }
//
//        DescriptorHeap::Description createDesc;
//        createDesc.type = mDescription.type;
//        createDesc.name = (mDescriptorHeapPool.size() + " " + toString(mDescription.type) + " type CPU Visible descriptor heap");
//        createDesc.maxDescriptors = mDescription.numDescriptorsPerHeap;
//        createDesc.flag = DescriptorHeapFlag::CPUVisible;
//
//        mDescriptorHeapPool.emplace_back(createDesc);
//
//        return mDescriptorHeapPool.back();
//    }
//
//    void DescriptorHeapPool::discardDescriptorHeap(const uint64_t completedFenceValue, dx12::DescriptorHeap& usedHeaps)
//    {
//        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);
//
//        mUsedQueue.push(std::make_pair(completedFenceValue, &usedHeaps));
//    }
//
//    void DescriptorHeapPool::destroyPool()
//    {
//        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);
//
//        updateAvailableQueue();
//        HASSERT(mUsedQueue.empty(), "Trying to destroy %s descriptor heap pool while %d CPU visible heaps are being used.", toString(mDescription.type), mUsedQueue.size());
//
//        // clear the queues to prevent dangling pointers
//        while (!mAvailableQueue.empty())
//        {
//            mAvailableQueue.pop();
//        }
//
//        mDescriptorHeapPool.clear();
//    }
//
//    DescriptorHeapPool::~DescriptorHeapPool()
//    {
//        destroyPool();
//    }
//
//    void DescriptorHeapPool::updateAvailableQueue()
//    {
//        while (!mUsedQueue.empty() && GfxCommandListManager().isFenceComplete(mUsedQueue.front().first))
//        {
//            mAvailableQueue.push(mUsedQueue.front().second);
//            mUsedQueue.pop();
//        }
//    }
//}
//
//namespace graphics
//{
//    DescriptorAllocator::DescriptorAllocator(const DescriptorHeapType type, const size_t initialGPUDescriptors)
//        : mType(type), mCPUDescriptorHeap(nullptr), mCPUDescriptorHeapPool({mType}),
//        mGPUDescriptorHeap({ "DescriptorAllocator dynamic GPU heap", mType, initialGPUDescriptors, DescriptorHeapFlag::GPUVisible })
//    {}
//
//    dx12::DescriptorHandle DescriptorAllocator::allocate(const size_t count)
//    {
//        std::lock_guard<std::mutex> scoppedLockGuard(mHeapMutex);
//
//        if (mCPUDescriptorHeap == nullptr || !mCPUDescriptorHeap->canAllocate(count))
//        {
//            mCPUDescriptorHeap = &(mCPUDescriptorHeapPool.requestDescriptorHeap());
//        }
//
//        return mCPUDescriptorHeap->allocate(count);
//    }
//
//    void DescriptorAllocator::stageDescriptors(dx12::DescriptorHandle& handle, const size_t count)
//    {
//        std::lock_guard<std::mutex> scoppedLockGuard(mHeapMutex);
//
//        HASSERT(mCPUDescriptorHeap->validateHandle(handle), "Invalid descriptor handle.");
//        HASSERT(mCPUDescriptorHeap->validateHandle(
//            handle + mCPUDescriptorHeap->validateHandle(handle + (count * mCPUDescriptorHeap->getDescriptorSize()))),
//            "Invalid descriptor handles. Not enough(%d) descriptors in heap.", count);
//
//        mStagingCache.emplace_back(handle, count, true);
//    }
//
//    dx12::DescriptorHandle DescriptorAllocator::finalize()
//    {
//        std::lock_guard<std::mutex> scopedLockGuard(mHeapMutex);
//
//        if (mStagingCache.empty())
//            return {};
//
//        // Allocate space in the GPU descriptor heap
//        size_t totalDescriptors = 0;
//        for (const Descriptor& entry : mStagingCache)
//            totalDescriptors += entry.count;
//
//        dx12::DescriptorHandle gpuHandle = mGPUDescriptorHeap.allocate(totalDescriptors);
//        dx12::DescriptorHandle currentDest = gpuHandle;
//
//        // Prepare arguments for CopyDescriptors
//        static constexpr uint32_t kMaxDescriptorsPerCopy = 16;
//
//        UINT numDestRanges = 0;
//        D3D12_CPU_DESCRIPTOR_HANDLE destRangeStarts[kMaxDescriptorsPerCopy];
//        UINT destRangeSizes[kMaxDescriptorsPerCopy];
//
//        UINT numSrcRanges = 0;
//        D3D12_CPU_DESCRIPTOR_HANDLE srcRangeStarts[kMaxDescriptorsPerCopy];
//        UINT srcRangeSizes[kMaxDescriptorsPerCopy];
//
//        for (Descriptor& entry : mStagingCache)
//        {
//            if (!entry.isDirty)
//                continue;
//
//            entry.isDirty = false;
//
//            // Copying non-contiguous CPU descriptors to a contiguous GPU heap
//            dx12::DescriptorHandle srcHandle = entry.handle;
//            UINT descriptorCount = entry.count;
//
//            // Handle range batching
//            while (descriptorCount > 0)
//            {
//                UINT batchSize = std::min(descriptorCount, kMaxDescriptorsPerCopy - numSrcRanges);
//
//                // Setup destination range (contiguous block in GPU heap)
//                destRangeStarts[numDestRanges] = currentDest;
//                destRangeSizes[numDestRanges] = batchSize;
//                numDestRanges++;
//
//                // Setup source ranges (individual descriptors)
//                for (UINT i = 0; i < batchSize; i++)
//                {
//                    srcRangeStarts[numSrcRanges] = srcHandle;
//                    srcRangeSizes[numSrcRanges] = 1;
//                    numSrcRanges++;
//
//                    srcHandle += mCPUDescriptorHeap->getDescriptorSize();
//                }
//
//                currentDest += batchSize * mGPUDescriptorHeap.getDescriptorSize();
//                descriptorCount -= batchSize;
//
//                // Perform batched copy when buffer is full
//                if (numSrcRanges == kMaxDescriptorsPerCopy)
//                {
//                    GfxDevice()->CopyDescriptors(
//                        numDestRanges, destRangeStarts, destRangeSizes,
//                        numSrcRanges, srcRangeStarts, srcRangeSizes,
//                        descriptorHeapType.at(mType));
//
//                    numSrcRanges = 0;
//                    numDestRanges = 0;
//                }
//            }
//        }
//
//        // Final copy for remaining descriptors
//        if (numSrcRanges > 0)
//        {
//            GfxDevice()->CopyDescriptors(
//                numDestRanges, destRangeStarts, destRangeSizes,
//                numSrcRanges, srcRangeStarts, srcRangeSizes,
//                descriptorHeapType.at(mType));
//        }
//
//        // Clear the staging cache
//        mStagingCache.clear();
//
//        return gpuHandle;
//    }
//
//    DescriptorAllocator::~DescriptorAllocator()
//    {
//        std::lock_guard<std::mutex> scoppedLockGuard(mHeapMutex);
//
//        mCPUDescriptorHeapPool.destroyPool();
//    }
//}
