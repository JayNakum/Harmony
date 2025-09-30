#include "NoisePass.h"

namespace shaders
{
    static const char* SimplexNoise = R"###(
cbuffer cbFrame : register(b0)
{
    float gTotalTime;
    float gDeltaTime;
    uint _framePadding0;
    uint _framePadding1;
};

cbuffer cbNoiseParams : register(b1)
{
    float2 gResolution0;
    float gNoiseScale0;
    uint _noiseParamsPadding0;
}

RWTexture2D<float> outputTexture0 : register(u0);

static const float PI = 3.14159265358979323826264f;
static const float TWO_PI = 2.0f * PI;

static const uint perm[] = {
    151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,
    129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,
    49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,

    151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,
    129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,
    49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

static const int3 grad3[] = {
    int3( 1, 1, 0), int3(-1, 1, 0), int3( 1,-1, 0),
    int3(-1,-1, 0), int3( 1, 0, 1), int3(-1, 0, 1),
    int3( 1, 0,-1), int3(-1, 0,-1), int3( 0, 1, 1),
    int3( 0,-1, 1), int3( 0, 1,-1), int3( 0,-1,-1)
};

static const int4 grad4[]= {
    int4( 0, 1, 1, 1), int4( 0, 1, 1,-1), int4( 0, 1,-1, 1), int4( 0, 1,-1,-1),
    int4( 0,-1, 1, 1), int4( 0,-1, 1,-1), int4( 0,-1,-1, 1), int4( 0,-1,-1,-1),
    int4( 1, 0, 1, 1), int4( 1, 0, 1,-1), int4( 1, 0,-1, 1), int4( 1, 0,-1,-1),
    int4(-1, 0, 1, 1), int4(-1, 0, 1,-1), int4(-1, 0,-1, 1), int4(-1, 0,-1,-1),
    int4( 1, 1, 0, 1), int4( 1, 1, 0,-1), int4( 1,-1, 0, 1), int4( 1,-1, 0,-1),
    int4(-1, 1, 0, 1), int4(-1, 1, 0,-1), int4(-1,-1, 0, 1), int4(-1,-1, 0,-1),
    int4( 1, 1, 1, 0), int4( 1, 1,-1, 0), int4( 1,-1, 1, 0), int4( 1,-1,-1, 0),
    int4(-1, 1, 1, 0), int4(-1, 1,-1, 0), int4(-1,-1, 1, 0), int4(-1,-1,-1, 0)
};

static const uint4 simplex[] = {
    uint4( 0, 1, 2, 3), uint4( 0, 1, 3, 2), uint4( 0, 0, 0, 0), uint4( 0, 2, 3, 1),
    uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0), uint4( 1, 2, 3, 0),
    uint4( 0, 2, 1, 3), uint4( 0, 0, 0, 0), uint4( 0, 3, 1, 2), uint4( 0, 3, 2, 1),
    uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0), uint4( 1, 3, 2, 0),
    uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0),
    uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0),
    uint4( 1, 2, 0, 3), uint4( 0, 0, 0, 0), uint4( 1, 3, 0, 2), uint4( 0, 0, 0, 0),
    uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0), uint4( 2, 3, 0, 1), uint4( 2, 3, 1, 0),
    uint4( 1, 0, 2, 3), uint4( 1, 0, 3, 2), uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0),
    uint4( 0, 0, 0, 0), uint4( 2, 0, 3, 1), uint4( 0, 0, 0, 0), uint4( 2, 1, 3, 0),
    uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0),
    uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0),
    uint4( 2, 0, 1, 3), uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0),
    uint4( 3, 0, 1, 2), uint4( 3, 0, 2, 1), uint4( 0, 0, 0, 0), uint4( 3, 1, 2, 0),
    uint4( 2, 1, 0, 3), uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0), uint4( 0, 0, 0, 0),
    uint4( 3, 1, 0, 2), uint4( 0, 0, 0, 0), uint4( 3, 2, 0, 1), uint4( 3, 2, 1, 0)
};

float noise(float2 v)
{
    const float tSQRT3 = sqrt(3.0f);
    const float F2 = 0.5f * (tSQRT3 - 1.0f);
    const float G2 = (3.0f - tSQRT3) / 6.0f;

    float skew = (v.x + v.y) * F2;

    int2 cellCenter = (int2)floor(v + skew);

    float unskew = (cellCenter.x + cellCenter.y) * G2;

    float2 delta0 = v - (float2(cellCenter) - unskew);
    int2 offset1 = (delta0.x > delta0.y) ? int2(1, 0) : int2(0, 1);

    float2 delta1 = delta0 - offset1 + G2;
    float2 delta2 = delta0 - 1.0f + 2.0f * G2;

    float3 fallOff = float3(
        0.5f - dot(delta0, delta0),
        0.5f - dot(delta1, delta1),
        0.5f - dot(delta2, delta2)
    );
    fallOff = max(fallOff, 0.0f);
    // fallOff = pow(fallOff, 4);
    fallOff *= fallOff;
    fallOff *= fallOff;

    int ii = cellCenter.x & 255;
    int jj = cellCenter.y & 255;

    int gi0 = perm[ii + perm[jj]] % 12;
    int gi1 = perm[ii + offset1.x + perm[jj + offset1.y]] % 12;
    int gi2 = perm[ii + 1 + perm[jj + 1]] % 12;

    float3 dotProduct = float3(
        dot(grad3[gi0].xy, delta0),
        dot(grad3[gi1].xy, delta1),
        dot(grad3[gi2].xy, delta2)
    );

    float3 noise = fallOff * dotProduct;

    return 70.0f * (noise.x + noise.y + noise.z);
}

float noise(float3 v)
{
    const float F3 = 1.0f / 3.0f;
    const float G3 = 1.0f / 6.0f;

    float skew = (v.x + v.y + v.z) * F3;

    int3 cellCenter = (int3)floor(v + skew);

    float unskew = (cellCenter.x + cellCenter.y + cellCenter.z) * G3;

    float3 delta0 = v - (float3(cellCenter) - unskew);
    int3 offset1, offset2;

    if (delta0.x >= delta0.y)
    {
        if (delta0.y >= delta0.z)
        {
            offset1 = int3(1, 0, 0);
            offset2 = int3(1, 1, 0);
        }
        else if (delta0.x >= delta0.z)
        {
            offset1 = int3(1, 0, 0);
            offset2 = int3(1, 0, 1);
        }
        else
        {
            offset1 = int3(0, 0, 1);
            offset2 = int3(1, 0, 1);
        }
    }
    else
    {
        if (delta0.y < delta0.z)
        {
            offset1 = int3(0, 0, 1);
            offset2 = int3(0, 1, 1);
        }
        else if (delta0.x < delta0.z)
        {
            offset1 = int3(0, 1, 0);
            offset2 = int3(0, 1, 1);
        }
        else
        {
            offset1 = int3(0, 1, 0);
            offset2 = int3(1, 1, 0);
        }
    }

    float3 delta1 = delta0 - float3(offset1) + G3;
    float3 delta2 = delta0 - float3(offset2) + 2.0f * G3;
    float3 delta3 = delta0 - 1.0f + 3.0f * G3;

    float4 fallOff = float4(
        0.5f - dot(delta0, delta0),
        0.5f - dot(delta1, delta1),
        0.5f - dot(delta2, delta2),
        0.5f - dot(delta3, delta3)
    );
    fallOff = max(fallOff, 0.0f);
    // fallOff = pow(fallOff, 4);
    fallOff *= fallOff;
    fallOff *= fallOff;

    int ii = cellCenter.x & 255;
    int jj = cellCenter.y & 255;
    int kk = cellCenter.z & 255;

    int gi0 = perm[ii + perm[jj + perm[kk]]] % 12;
    int gi1 = perm[ii + offset1.x + perm[jj + offset1.y + perm[kk + offset1.z]]] % 12;
    int gi2 = perm[ii + offset2.x + perm[jj + offset2.y + perm[kk + offset2.z]]] % 12;
    int gi3 = perm[ii + 1 + perm[jj + 1 + perm[kk + 1]]] % 12;

    float4 dotProduct = float4(
        dot(grad3[gi0], delta0),
        dot(grad3[gi1], delta1),
        dot(grad3[gi2], delta2),
        dot(grad3[gi3], delta3)
    );

    float4 noise = fallOff * dotProduct;

    return 32.0f * (noise.x + noise.y + noise.z + noise.w);
}

float noise(float4 v)
{
    const float tSQRT5 = sqrt(5.0f);
    const float F4     = (tSQRT5 - 1.0f) / 4.0f;
    const float G4     = (5.0f - tSQRT5) / 20.0f;

    float skew = dot(v, float4(1,1,1,1)) * F4;
    int4  cellCenter = (int4)floor(v + skew);

    float unskew = dot(float4(cellCenter), float4(1,1,1,1)) * G4;
    float4 cellOrigin = float4(cellCenter) - unskew;
    float4 delta0     = v - cellOrigin;

    int c1 = (delta0.x > delta0.y) ? 32 : 0;
    int c2 = (delta0.x > delta0.z) ? 16 : 0;
    int c3 = (delta0.y > delta0.z) ?  8 : 0;
    int c4 = (delta0.x > delta0.w) ?  4 : 0;
    int c5 = (delta0.y > delta0.w) ?  2 : 0;
    int c6 = (delta0.z > delta0.w) ?  1 : 0;
    int c  = c1 | c2 | c3 | c4 | c5 | c6;

    uint4 si      = simplex[c];
    int4  offset1 = int4(si >= 3);
    int4  offset2 = int4(si >= 2);
    int4  offset3 = int4(si >= 1);

    float4 delta1 = delta0 - float4(offset1) + G4;
    float4 delta2 = delta0 - float4(offset2) + 2.0f * G4;
    float4 delta3 = delta0 - float4(offset3) + 3.0f * G4;
    float4 delta4 = delta0 - 1.0f + 4.0f * G4;

    float fallOff0 = max(0.5f - dot(delta0, delta0), 0.0f);
    float fallOff1 = max(0.5f - dot(delta1, delta1), 0.0f);
    float fallOff2 = max(0.5f - dot(delta2, delta2), 0.0f);
    float fallOff3 = max(0.5f - dot(delta3, delta3), 0.0f);
    float fallOff4 = max(0.5f - dot(delta4, delta4), 0.0f);

    fallOff0 *= fallOff0;
    fallOff0 *= fallOff0;

    fallOff1 *= fallOff1;
    fallOff1 *= fallOff1;

    fallOff2 *= fallOff2;
    fallOff2 *= fallOff2;

    fallOff3 *= fallOff3;
    fallOff3 *= fallOff3;

    fallOff4 *= fallOff4;
    fallOff4 *= fallOff4;


    int ii = cellCenter.x & 255;
    int jj = cellCenter.y & 255;
    int kk = cellCenter.z & 255;
    int ll = cellCenter.w & 255;

    int gi0 = perm[ii + perm[jj + perm[kk + perm[ll]]]] % 32;
    int gi1 = perm[ii + offset1.x + perm[jj + offset1.y + perm[kk + offset1.z + perm[ll + offset1.w]]]] % 32;
    int gi2 = perm[ii + offset2.x + perm[jj + offset2.y + perm[kk + offset2.z + perm[ll + offset2.w]]]] % 32;
    int gi3 = perm[ii + offset3.x + perm[jj + offset3.y + perm[kk + offset3.z + perm[ll + offset3.w]]]] % 32;
    int gi4 = perm[ii + 1 + perm[jj + 1 + perm[kk + 1 + perm[ll + 1 ]]]] % 32;

    float d0 = dot(grad4[gi0], delta0);
    float d1 = dot(grad4[gi1], delta1);
    float d2 = dot(grad4[gi2], delta2);
    float d3 = dot(grad4[gi3], delta3);
    float d4 = dot(grad4[gi4], delta4);

    return 27.0f * (fallOff0*d0 + fallOff1*d1 + fallOff2*d2 + fallOff3*d3 + fallOff4*d4);
}

float noise2D(float2 uv)
{
    return noise(uv);
}

float noise3D(float2 uv, float z)
{
    return noise(float3(uv, z));
}

float noise3DAnimated(float2 uv)
{
    return noise(float3(uv, gTotalTime));
}

float noise4D(float2 uv, float z, float w)
{
    return noise(float4(uv, z, w));
}

float noise4DAnimated(float2 uv)
{
    return noise(float4(uv, gTotalTime, sin(gTotalTime)));
}

[numthreads(8, 8, 1)]
void csMain(
    uint3 dtid : SV_DispatchThreadID,
    uint3 gtid : SV_GroupThreadID,
    uint3 gid : SV_GroupID,
    uint idx : SV_GroupIndex
)
{
    {// Water Noise Texture
        float2 uv = dtid.xy / gResolution0;

        float raw = noise3DAnimated(uv * gNoiseScale0);
        float mapped = raw * 0.5f + 0.5f;

        outputTexture0[dtid.xy] = mapped;
    }
}
)###";
}

namespace engine
{
    NoisePassData::NoisePassData()
        : mWaterNoiseTextureWidth(256.0f), mWaterNoiseTextureHeight(256.0f)
    {
        reset();
    }

    void NoisePassData::debugUIRender()
    {
        ImGui::Begin("Noise Pass Data");
        ImGui::SliderFloat("Noise Scale", &mWaterNoiseScale, 1.0f, 100.0f);
        ImGui::End();
    }

    void NoisePassData::reset()
    {
        mWaterNoiseScale = 16.0f;
    }
}

namespace engine
{
    NoisePass::NoisePass(graphics::Device& device, ResourceManager& resourceManager)
        : mNoiseData(),
        mStaticShaderResourceTableLayout{
            graphics::ShaderVisibility::All,
            {
                {graphics::DescriptorRangeType::UAV, 1}
            }
        },
        mShaderResources(resourceManager.createResourceTable("SimplexNoiseShader_ResourceTable", mStaticShaderResourceTableLayout, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::StaticGPU)),
        mNoiseSI(
            {
                {graphics::ShaderVisibility::Vertex, 4},
                {graphics::ShaderVisibility::Vertex, 4}
            },
            { mStaticShaderResourceTableLayout }
        ),
        mNoisePSO(createPSO(device))
    {

        graphics::HeapWithAllocator::ResourceDescription waterNoiseTexture{
            graphics::HeapWithAllocator::ResourceCategory::PersistentData,
            {
                "UA_Water_Noise_Texture",
                (size_t)mNoiseData.getWaterNoiseTextureWidth(), (size_t)mNoiseData.getWaterNoiseTextureHeight(),
                1, 1,
                graphics::GPUResourceDimension::Texture2D,
                graphics::DefaultFormats::getUAVFormat(graphics::DefaultFormats::R32_FLOAT),
                graphics::GPUResourceFlags::AllowUnorderedAccess
            }
        };
        resourceManager.createTexture(waterNoiseTexture);

        mShaderResources.setResource(device, resourceManager.getTextureUAV("UA_Water_Noise_Texture"));
        mNoiseSI.setResourceTable(mShaderResources);
    }

    void NoisePass::getInputOutputs(graphics::ComputeContext& context, ResourceManager& resourceManager)
    {
        graphics::Texture& noise0 = resourceManager.getTexture("UA_Water_Noise_Texture");
        context.transitionBarrier(noise0, graphics::GPUResourceStates::UnorderedAccess);
        context.uavBarrier(noise0);

        context.flushResourceBarriers();
    }

    void NoisePass::dispatch(graphics::ComputeContext& context, const FrameConstants& frameConstants)
    {
        context.setPipelineState(mNoisePSO);
        context.setRootSignature(mNoiseSI.getRootSignature(context.getDevice()));

        mNoiseSI.setConstants({ frameConstants.totalTime, frameConstants.deltaTime, 0x1deadb0b, 0x1deadb0b }, 0);
        mNoiseSI.setConstants({ mNoiseData.getWaterNoiseTextureWidth(), mNoiseData.getWaterNoiseTextureHeight(), mNoiseData.getWaterNoiseScale(), 0x1deadb0b }, 1);
        mNoiseSI.finalize(context);

        context.dispatch2D(256, 256);
    }

    graphics::ComputePipelineState NoisePass::createPSO(graphics::Device& device)
    {
        graphics::ComputePipelineState::Description psoDesc;
        psoDesc.name = "Compute_PSO_Noise";
        psoDesc.computeShader = graphics::CommandContext::compileShader(
            "NoiseCS",
            shaders::SimplexNoise,
            "cs_5_0",
            "csMain"
        );
        psoDesc.rootSignature = &(mNoiseSI.getRootSignature(device));

        return graphics::ComputePipelineState(device, psoDesc);
    }
}
