#pragma once

#include "common.h"
#include "Log.h"

#include "RenderPass.h"

namespace engine
{
    namespace renderPass
    {
        class WhirlpoolHeightMap : RenderPass
        {
        public:
            WhirlpoolHeightMap(graphics::dx12::Device& device, ResourceManager& resourceManager);
            void copyDataToGPU(graphics::GraphicContext& context, ResourceManager& resourceManager) override;
            void freeGPUUploadData(ResourceManager& resourceManager) override;

            void resize(const unsigned int width, const unsigned int height, graphics::dx12::Device& device, ResourceManager& resourceManager) override;
            void update(const double timeStep) override;

            void getInputOutputs(graphics::GraphicContext& context, ResourceManager& resourceManager) override;
            void render(graphics::GraphicContext& context, const FrameConstants& frameConstants) override;

            ~WhirlpoolHeightMap() = default;

        private:
            void updateCamera();

            struct WhirlpoolHeightMapParams
            {
                vec2f center = vec2f(0.0f);
                float swirlRadius = 0.0f;
                float swirlAngle = 0.0f;

                float sinkRadius = 0.0f;
                float sinkDepth = 0.0f;
                const uint32_t _whirlpoolParamsPaddingFinal[2] = { 0x1deadb0b, 0x1deadb0b };
            };
            float mCurrentSwirlAngle = 0.0f;

        private:
            graphics::dx12::GraphicsPipelineState createPSO(graphics::dx12::Device& device);

        private:
            const graphics::ResourceTable::Layout mStaticShaderResourceTableLayout;
            graphics::ResourceTable mShaderResources;
            graphics::ShaderInput mWhirlpoolHeightMapSI;

            graphics::dx12::GraphicsPipelineState mWhirlpoolHeightMapPSO;

            graphics::dx12::DescriptorHandle mRTV;
            graphics::HeapWithAllocator::ResourceDescription mRenderTargetDescription;

            graphics::dx12::Buffer* mCommonPassConstantsBuffer;

            RenderItem mWhirlpoolHeightMapRI;

        private:
            vec3f mEyePos = { 5.0f, 10.0f, -25.0f };
            mat4f mView;
            mat4f mProjection;
            vec2f mNearFarZ;
        };
    }
}
