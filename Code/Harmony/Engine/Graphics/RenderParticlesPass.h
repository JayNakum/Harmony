#pragma once

#include "PassCore.h"

namespace engine
{
    class RenderParticlesPassData : public PassData
    {
    public:
        RenderParticlesPassData();

        void debugUIRender() override;
        void reset() override;

        ~RenderParticlesPassData() override = default;

    private:
        // GeometryGenerator mGeoGen;
    };


    class RenderParticlesPass : RenderPass
    {
    public:
        RenderParticlesPass(graphics::Device& device, ResourceManager& resourceManager);

        bool needsCopyDataToGPU() const override { return false; }
        void copyDataToGPU(graphics::CopyContext&, ResourceManager&) override {}
        void freeGPUUploadData(ResourceManager&) override {}

        void resize(const unsigned int width, const unsigned int height, graphics::Device& device, ResourceManager& resourceManager) override;
        void update(const double timeStep) override;

        void getInputOutputs(graphics::GraphicContext& context, ResourceManager& resourceManager) override;
        void render(graphics::GraphicContext& context, const FrameConstants& frameConstants) override;

    private:
        graphics::GraphicsPipelineState createPSO(graphics::Device& device);

    private:
        RenderParticlesPassData mRenderParticlesPassData;

        const graphics::ResourceTable::Layout mStaticShaderResourceTableLayout;
        graphics::ResourceTable mShaderResources;
        graphics::ShaderInput mRenderParticlesPassSI;

        graphics::GraphicsPipelineState mRenderParticlesPassPSO;

        graphics::DescriptorHandle mRTV;
        graphics::HeapWithAllocator::ResourceDescription mRenderTargetDescription;

        graphics::Buffer* mCommonPassConstantsBuffer;

    private:
        RenderItem mRenderParticlesRI;
    };
}
