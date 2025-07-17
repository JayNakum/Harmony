#pragma once

#include "common.h"
#include "Log.h"

#include "CommandContext.h"

#include "dx12/RootSignature.h"
#include "dx12/Buffer.h"
#include "dx12/Texture.h"

namespace graphics
{
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

        ResourceTable(const Layout& layout, dx12::DescriptorHandle allocationStart);

        inline const Layout& getLayout() const { return mLayout; }
        inline const dx12::DescriptorHandle& getHandleStart() const { return mDescriptors; }
        inline const unsigned int getHandlesCount() const { return mDescriptorsCount; }

        // Set a buffer at the specified layoutIndex, parameterIndex and rangeIndex.
        const dx12::DescriptorHandle& setResource(dx12::Device& device, const dx12::ByteAddressBuffer& buffer,
                                                  const unsigned int row = 0, const unsigned int column = 0);

        const dx12::DescriptorHandle& setResource(dx12::Device& device, const dx12::ByteAddressBuffer& buffer,
                                                  const uint64_t sizeInBytes, const unsigned int offsetInBytes,
                                                  const unsigned int row = 0, const unsigned int column = 0);

        // Set a texture at the specified layoutIndex, parameterIndex and rangeIndex.
        const dx12::DescriptorHandle& setResource(dx12::Device& device, const dx12::Texture& texture,
                                                  const dx12::Texture::ShaderResourceViewInfo& viewInfo,
                                                  const unsigned int row = 0, const unsigned int column = 0);

        const dx12::DescriptorHandle& setResource(dx12::Device& device, const dx12::Texture& texture,
                                                  const dx12::Texture::UnorderedAccessViewInfo& viewInfo,
                                                  const unsigned int row = 0, const unsigned int column = 0);

        void copyDescriptorsFrom(dx12::Device& device, const ResourceTable& table);

    private:
        const unsigned int getDescriptorOffsetAt(const unsigned int row, const unsigned int column) const;

    private:
        const Layout mLayout;
        dx12::DescriptorHandle mDescriptors;
        unsigned int mDescriptorsCount;
    };

    class ShaderInput
    {
    public:
        struct Constants
        {
            const ShaderVisibility shaderVisibility;
            const unsigned int count = 1;
        };

        using RootConstants = std::vector<dx12::DWParam>;

        // Scoped shader inputs.
        // 32bit constants are expected at the top of root parameters
        // Layout is expected in order of shader registers.
        ShaderInput(const std::vector<Constants>& constantsLayout, const std::vector<ResourceTable::Layout>& gpuResourceTablesLayout);

        dx12::RootSignature& getRootSignature(dx12::Device& device);

        void setConstants(const RootConstants values, const unsigned int layoutIndex);

        inline void setResourceTable(const ResourceTable& resourceTable)
        {
            mGPUResourceTables.push_back(resourceTable);
        }
        void updateResourceTable(dx12::Device& device, const ResourceTable& resourceTable, const unsigned int layoutIndex);

        void finalize(GraphicContext& context);

        ~ShaderInput() = default;

        inline static void freeAllRootSignatures() { ShaderInput::msRootSignatures.clear(); }

    private:
        std::string createRootSignatureID();

        static const std::array<dx12::StaticSampler, 6> getStaticSamplers();

    private:
        std::vector<RootConstants> mRootConstants;

        const std::vector<Constants> mConstantsLayout;
        const std::vector<ResourceTable::Layout> mGPUResourceTablesLayout;
        std::vector<ResourceTable> mGPUResourceTables;

    private:
        static std::unordered_map<std::string, dx12::RootSignature> msRootSignatures;
    };
}
