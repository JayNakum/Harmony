#pragma once

#include "common.h"

#include "CommandContext.h"

#include "dx12/RootSignature.h"
#include "dx12/Buffer.h"
#include "dx12/Texture.h"

namespace graphics
{
    /*
    * Resource Table represents a descriptor table.
    * It needs a pre allocated allocationStart handle, meaning that it can represent both CPU and GPU table.
    */
    class ResourceTable
    {
    public:
        // Descriptor Table Entry
        struct Range
        {
            const DescriptorRangeType type;
            const unsigned int count = 1;
        };

        // Descriptor Table
        struct Layout
        {
            const ShaderVisibility shaderVisibility;
            const std::vector<Range> ranges;
        };

        ResourceTable(const Layout& layout, DescriptorHandle allocationStart);

        inline const Layout& getLayout() const { return mLayout; }
        inline const DescriptorHandle& getHandleStart() const { return mDescriptors; }
        inline unsigned int getHandlesCount() const { return mDescriptorsCount; }

        // Set a resource descriptor at the specified row and column
        void setResource(Device& device, const DescriptorHandle& descriptor,
            const unsigned int row = 0, const unsigned int column = 0);

        /*
                // Set a buffer at the specified row and column
                const DescriptorHandle& setResource(Device& device, const ByteAddressBuffer& buffer,
                    const unsigned int row = 0, const unsigned int column = 0);

                const DescriptorHandle& setResource(Device& device, const ByteAddressBuffer& buffer,
                    const uint64_t sizeInBytes, const unsigned int offsetInBytes,
                    const unsigned int row = 0, const unsigned int column = 0);

                // Set a texture at the specified row and column
                const DescriptorHandle& setResource(Device& device, const Texture& texture,
                                                          const Texture::ShaderResourceViewInfo& viewInfo,
                                                          const unsigned int row = 0, const unsigned int column = 0);

                const DescriptorHandle& setResource(Device& device, const Texture& texture,
                                                          const Texture::UnorderedAccessViewInfo& viewInfo,
                                                          const unsigned int row = 0, const unsigned int column = 0);

        */
        void copyDescriptorsFrom(Device& device, const ResourceTable& table);

    private:
        unsigned int getDescriptorOffsetAt(const unsigned int row, const unsigned int column) const;

    private:
        const Layout mLayout;
        DescriptorHandle mDescriptors;
        unsigned int mDescriptorsCount;
    };

    /*
    * Shader Input represents the input layout of a shader.
    * Based on the layout a root signature can be created.
    * It also keeps track of all the root signatures in a hash map and reuses an existing one if the layout is the same.
    * It expects GPU resource tables which can be directly used to set as root descriptor tabels.
    */
    class ShaderInput
    {
    public:
        struct Constants
        {
            const ShaderVisibility shaderVisibility;
            const unsigned int count = 1;
        };

        using RootConstants = std::vector<DWParam>;

        // Scoped shader inputs.
        // 32bit constants are expected at the top of root parameters
        // Layout is expected in order of shader registers.
        ShaderInput(const std::vector<Constants>& constantsLayout, const std::vector<ResourceTable::Layout>& gpuResourceTablesLayout);

        // Get or Create a root signature for the specified layout.
        // The key from createRootSignatureID() is used to get or create from the hash map.
        RootSignature& getRootSignature(Device& device);

        // Stages constants for root signature.
        // The layout index is from the constantsLayout.
        void setConstants(const RootConstants values, const unsigned int layoutIndex);

        // Stages a GPU resource table for root signature.
        // The order in which setResourceTable() is called determines the layout index.
        inline void setResourceTable(const ResourceTable& resourceTable)
        {
            mGPUResourceTables.push_back(resourceTable);
        }

        // Updates an existing resource table at layoutIndex.
        // This makes use of Device::CopyDescriptors().
        void updateResourceTable(Device& device, const ResourceTable& resourceTable, const unsigned int layoutIndex);

        // Sets all the staged root constants and descriptor tables.
        void finalize(GraphicContext& context);
        void finalize(ComputeContext& context);

        ~ShaderInput() = default;

        // Clear the root signature hash map.
        inline static void freeAllRootSignatures() { ShaderInput::msRootSignatures.clear(); }

    private:
        // To keep track of all the root signature, it creates a string key based on layout which is used for hash map.
        std::string createRootSignatureID();

        // All the root signatures are equipped with 6 static samplers.
        static const std::array<StaticSampler, 6> getStaticSamplers();

    private:
        std::vector<RootConstants> mRootConstants;

        const std::vector<Constants> mConstantsLayout;
        const std::vector<ResourceTable::Layout> mGPUResourceTablesLayout;
        std::vector<ResourceTable> mGPUResourceTables;

    private:
        std::mutex mRootSignatureMutex;
        static std::unordered_map<std::string, RootSignature> msRootSignatures;
    };
}
