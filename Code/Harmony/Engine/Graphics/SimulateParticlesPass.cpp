#include "SimulateParticlesPass.h"

namespace shaders
{
    static const char* SimulateParticles = R"###(
static const float PI = 3.14159265358979323846264f;
static const float TWO_PI = 2.0f * PI;
static const float PHI = 1.618033988749895; // Golden Ratio

cbuffer cbFrame : register(b0)
{
    float gTotalTime;
    float gDeltaTime;
    uint _framePadding0;
    uint _framePadding1;
};

cbuffer cbEmitterParams : register(b1)
{
    float3 gEmitterPosition;
    float gInitialDuration;
    float3 gEmitterDirection;
    float gEmitterSpread;
    float gInitialSpeed;
    float gSpeedRamp;
    float gParticlesCount;
};

struct Particle
{
    float4 color;
    float3 position;
    float duration;
    float3 velocity;
    float scale;
};

RWStructuredBuffer<Particle> gParticles : register(u0);

float3 CalculateParticleVelocitySpiral(float3 emitterDir, float spread, float speed, uint particleIndex)
{
    float i = float(particleIndex);

    float goldenAngle = PI * (3.0f - sqrt(5.0f)); // ~137.5 degrees

    // Spiral angle - this creates the rotation around the cone
    float spiralAngle = goldenAngle * i;

    // Distance from center axis (0 to 1)
    // Using square root for better distribution from center to edge
    float normalizedRadius = sqrt(i / gParticlesCount);

    // Convert spread (degrees) to radians
    float spreadRadians = spread * (PI / 180.0f);

    // Cone angle for this particle (0 at center, spreadRadians at max radius)
    float coneAngle = normalizedRadius * spreadRadians;

    // Create direction vector in cone space
    // The cone axis is along the Z direction
    float radiusInCone = sin(coneAngle);
    float forwardInCone = cos(coneAngle);

    // Apply spiral rotation around the cone
    float x = radiusInCone * cos(spiralAngle);
    float y = radiusInCone * sin(spiralAngle);
    float z = forwardInCone;

    float3 localDir = float3(x, y, z);

    // Transform from local cone space to world space
    float3 up = abs(emitterDir.y) < 0.999f ? float3(0.0f, 1.0f, 0.0f) : float3(1.0f, 0.0f, 0.0f);
    float3 right = normalize(cross(up, emitterDir));
    up = normalize(cross(emitterDir, right));

    // Transform: right is X, up is Y, emitterDir is Z
    float3 worldDir = localDir.x * right + localDir.y * up + localDir.z * emitterDir;

    // Add speed variation based on particle index to prevent clumping
    float speedVariation = 0.8f + 0.4f * frac(i * PHI); // Range: 0.8 to 1.2

    return normalize(worldDir) * speed * speedVariation;
}

[numthreads(64, 1, 1)]
void csMain(
    uint3 dtid : SV_DispatchThreadID,
    uint3 gtid : SV_GroupThreadID,
    uint3 gid : SV_GroupID,
    uint idx : SV_GroupIndex
)
{
    if (dtid.x >= (uint)gParticlesCount)
    {
        return;
    }

    Particle particle = gParticles[dtid.x];

    if (particle.duration < 0)
    {
        particle.duration = gInitialDuration * (1.0f - (float(dtid.x) / gParticlesCount));
        particle.velocity = CalculateParticleVelocitySpiral(gEmitterDirection, gEmitterSpread, gInitialSpeed, dtid.x);
        particle.position = gEmitterPosition;
        particle.scale = 0.01f;
    }

    particle.duration -= gDeltaTime;

    particle.position += particle.velocity * gDeltaTime;
    particle.color = float4(particle.position, 1.0f);
    particle.scale = 0.05f;

    gParticles[dtid.x] = particle;
}
)###";
}

namespace engine
{
    SimulateParticlesPassData::SimulateParticlesPassData()
    {
        reset();
    }

    void SimulateParticlesPassData::debugUIRender()
    {
        ImGui::Begin("Simulate Particles Pass Data");

        ImGui::DragFloat3("Emitter Position", (float*)&mEmitterParams.position, 0.01f);
        ImGui::InputFloat("Initial Duration", &mEmitterParams.initialDuration);
        ImGui::DragFloat3("Emitter Direction", (float*)&mEmitterParams.direction, 0.1f);
        ImGui::SliderFloat("Emitter Spread", &mEmitterParams.spread, 1.0f, 100.0f);
        ImGui::InputFloat("Initial Speed", &mEmitterParams.initialSpeed);
        ImGui::InputFloat("Speed Ramp", &mEmitterParams.speedRamp, 0.01f);
        ImGui::SliderFloat("Particles Count", &mEmitterParams.particlesCount, 1.0f, MAX_PARTICLES);

        debugUISave();

        ImGui::End();
    }

    void SimulateParticlesPassData::reset()
    {
        mEmitterParams.position = vec3f(0.0f, -2.0f, 0.0f);
        mEmitterParams.direction = vec3f(0.0f, 1.0f, 0.0f);
        mEmitterParams.spread = 15.0f;
        mEmitterParams.initialDuration = 10.0f;
        mEmitterParams.initialSpeed = 5.0f;
        mEmitterParams.speedRamp = 0.1f;
        mEmitterParams.particlesCount = MAX_PARTICLES;
    }
}

namespace engine
{
    SimulateParticlesPass::SimulateParticlesPass(graphics::Device& device, ResourceManager& resourceManager)
        : mSimulateParticlesData(),
        mStaticShaderResourceTableLayout{
            graphics::ShaderVisibility::All,
            {
                { graphics::DescriptorRangeType::UAV, 1 },
            }
        },
        mShaderResources(resourceManager.createResourceTable("SimulateParticlesShader_ResourceTable", mStaticShaderResourceTableLayout, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::StaticGPU)),
        mSimulateParticlesSI(
            {
                { graphics::ShaderVisibility::Vertex, 4 },
                { graphics::ShaderVisibility::Vertex, 16 },
            },
            { mStaticShaderResourceTableLayout }
        ),
        mSimulateParticlesPSO(createPSO(device))
    {
        graphics::HeapWithAllocator::ResourceDescription particlesBuffer{
            graphics::HeapWithAllocator::ResourceCategory::PersistentData,
            {
                "UA_Particles_Buffer",
                (size_t)SimulateParticlesPassData::MAX_PARTICLES * sizeof(SimulateParticlesPassData::Particle), 1,
                1, 1,
                graphics::GPUResourceDimension::Unknown, graphics::DefaultFormats::UNKNOWN,
                graphics::GPUResourceFlags::AllowUnorderedAccess
            }
        };
        resourceManager.createStructuredBuffer(particlesBuffer, SimulateParticlesPassData::MAX_PARTICLES, sizeof(SimulateParticlesPassData::Particle));

        mShaderResources.setResource(device, resourceManager.getStructuredBufferUAV("UA_Particles_Buffer"));
        mSimulateParticlesSI.setResourceTable(mShaderResources);
    }

    void SimulateParticlesPass::getInputOutputs(graphics::ComputeContext& context, ResourceManager& resourceManager)
    {
        graphics::StructuredBuffer& particlesBuffer = resourceManager.getStructuredBuffer("UA_Particles_Buffer");
        context.transitionBarrier(particlesBuffer, graphics::GPUResourceStates::UnorderedAccess);
        context.uavBarrier(particlesBuffer);

        context.flushResourceBarriers();
    }

    void SimulateParticlesPass::dispatch(graphics::ComputeContext& context, const FrameConstants& frameConstants)
    {
        context.setPipelineState(mSimulateParticlesPSO);
        context.setRootSignature(mSimulateParticlesSI.getRootSignature(context.getDevice()));

        mSimulateParticlesSI.setConstants({ frameConstants.totalTime, frameConstants.deltaTime, 0x1deadb0b, 0x1deadb0b }, 0);
        const SimulateParticlesPassData::EmitterParams emitterParams = mSimulateParticlesData.getEmitterParams();
        mSimulateParticlesSI.setConstants(
            {
                emitterParams.position.x, emitterParams.position.y, emitterParams.position.z,
                emitterParams.initialDuration,
                emitterParams.direction.x, emitterParams.direction.y, emitterParams.direction.z,
                emitterParams.spread,
                emitterParams.initialSpeed,
                emitterParams.speedRamp,
                emitterParams.particlesCount,
                0x1deadb0b,
                0x1deadb0b, 0x1deadb0b, 0x1deadb0b, 0x1deadb0b
            }, 1);
        mSimulateParticlesSI.finalize(context);

        context.dispatch1D(SimulateParticlesPassData::MAX_PARTICLES);
    }

    graphics::ComputePipelineState SimulateParticlesPass::createPSO(graphics::Device& device)
    {
        graphics::ComputePipelineState::Description psoDesc;
        psoDesc.name = "Compute_PSO_SimulateParticles";
        psoDesc.computeShader = graphics::CommandContext::compileShader(
            "SimulateParticlesCS",
            shaders::SimulateParticles,
            "cs_5_0",
            "csMain"
        );
        psoDesc.rootSignature = &(mSimulateParticlesSI.getRootSignature(device));

        return graphics::ComputePipelineState(device, psoDesc);
    }
}
