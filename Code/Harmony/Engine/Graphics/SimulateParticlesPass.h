#pragma once

#include "PassCore.h"

namespace engine
{
    class SimulateParticlesPassData : public PassData
    {
    public:
        static constexpr unsigned int MAX_PARTICLES = 10000;

        struct alignas(16) Particle
        {
            vec4f color;

            vec3f position;
            float duration;
            vec3f velocity;
            float scale;
        };

        struct alignas(16) EmitterParams
        {
            vec3f position;
            float initialDuration;

            vec3f direction;
            float spread;

            float initialSpeed;
            float speedRamp;
            float particlesCount;
        };

    public:
        SimulateParticlesPassData();

        inline const EmitterParams& getEmitterParams() const { return mEmitterParams; }

        void debugUIRender() override;
        void reset() override;

        ~SimulateParticlesPassData() override = default;

    private:
        EmitterParams mEmitterParams;
    };

    class SimulateParticlesPass : DispatchPass
    {
    public:
        SimulateParticlesPass(graphics::Device& device, ResourceManager& resourceManager);

        void copyDataToGPU(graphics::CopyContext& context, ResourceManager& resourceManager) override {}
        void freeGPUUploadData(ResourceManager& resourceManager) override {}

        void resize(const unsigned int width, const unsigned int height, graphics::Device& device, ResourceManager& resourceManager) override {}
        void update(const double timeStep) override {}

        void getInputOutputs(graphics::ComputeContext& context, ResourceManager& resourceManager) override;
        void dispatch(graphics::ComputeContext& context, const FrameConstants& frameConstants) override;

        ~SimulateParticlesPass() = default;

    private:
        graphics::ComputePipelineState createPSO(graphics::Device& device);

    private:
        SimulateParticlesPassData mSimulateParticlesData;

        const graphics::ResourceTable::Layout mStaticShaderResourceTableLayout;
        graphics::ResourceTable mShaderResources;
        graphics::ShaderInput mSimulateParticlesSI;

        graphics::ComputePipelineState mSimulateParticlesPSO;
    };
}
