#include "SimulateParticlesPass.h"

namespace shaders
{
    static const char* SimulateParticles = R"###(
static const float PI = 3.14159265358979323826264f;
static const float TWO_PI = 2.0f * PI;
static const float PHI = 1.618033988749895; // Golden Ratio

cbuffer cbFrame : register(b0)
{
    float gTotalTime;
    float gDeltaTime;
    uint _framePadding0;
    uint _framePadding1;
};

cbuffer cbEmitterParams : register(b0)
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

    float theta = acos(1.0f - 2.0f * frac(i / PHI) * (spread / PI));
    float phi = TWO_PI * frac(i * PHI);

    float sinTheta = sin(theta);
    float3 localDir = float3(sinTheta * cos(phi), sinTheta * sin(phi), cos(theta));

    float3 up = abs(emitterDir.y) < 0.999f ? float3(0.0f, 1.0f, 0.0f) : float3(1.0f, 0.0f, 0.0f);
    float3 right = normalize(cross(up, emitterDir));
    up = normalize(cross(emitterDir, right));

    float3 worldDir = localDir.x * right + localDir.y * up + localDir.z * emitterDir;
    return normalize(worldDir) * speed;
}

[numthreads(32, 1, 1)]
void csMain(
    uint3 dtid : SV_DispatchThreadID,
    uint3 gtid : SV_GroupThreadID,
    uint3 gid : SV_GroupID,
    uint idx : SV_GroupIndex
)
{
    if (dtid.x >= gParticlesCount)
    {
        return;
    }

    Particle particle = gParticles[dtid.x];
    if (particle.duration < 0)
    {
        particle.duration = gInitialDuration;
        particle.speed = gInitialSpeed;
        particle.velocity = CalculateParticleVelocitySpiral(gEmitterDirection, gEmitterSpread, gInitialSpeed, dtid.x);
        particle.position = gEmitterPosition;
        particle.scale = 0.1f;
    }

    particle.duration -= gDeltaTime;
    particle.velocity *= gSpeedRamp;
    particle.position += particle.velocity * gDeltaTime;
    particle.color = float4(0.5f + 0.5f * (particle.duration / gInitialDuration), 0.1f, 0.0f, 1.0f);

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

        ImGui::DragFloat3("Emitter Position", (float*)&mEmitterParams.position);
        ImGui::InputFloat("Initial Duration", &mEmitterParams.initialDuration);
        ImGui::DragFloat3("Emitter Direction", (float*)&mEmitterParams.direction);
        ImGui::SliderFloat("Emitter Spread", &mEmitterParams.spread, 1.0f, 10.0f);
        ImGui::InputFloat("Initial Speed", &mEmitterParams.initialSpeed);
        ImGui::InputFloat("Speed Ramp", &mEmitterParams.speedRamp, 0.01f);
        ImGui::SliderFloat("Particles Count", &mEmitterParams.particlesCount, 1.0f, MAX_PARTICLES);

        debugUISave();

        ImGui::End();
    }

    void SimulateParticlesPassData::reset()
    {
        mEmitterParams.position = vec3f(0.0f);
        mEmitterParams.direction = vec3f(0.0f, 1.0f, 0.0f);
        mEmitterParams.spread = 2.0f;
        mEmitterParams.initialDuration = 10.0f;
        mEmitterParams.initialSpeed = 10.0f;
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
    }
}
