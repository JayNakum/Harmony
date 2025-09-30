#include "WaterPass.h"

#include "GeometryGenerator.h"

#include "Maths.h"

namespace shaders
{
    static const char* GerstnerWaveWaterShader = R"###(
static const float PI = 3.14159265358979323826264f;
static const float TWO_PI = 2.0f * PI;
static const float G = 9.81f;

cbuffer cbFrame : register(b0)
{
    float gTotalTime;
    float gDeltaTime;
    uint _framePadding0;
    uint _framePadding1;
};

cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;

    float3 gEyePosW;
    uint _passPadding0;

    float4x4 gProj;
    float4x4 gInvProj;

    float gNearZ;
    float gFarZ;
    uint _passPadding1;
    uint _passPadding2;

    float4x4 gViewProj;
    float4x4 gInvViewProj;

    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    uint _passPaddingFinal[5];
};

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

//////////////////////////////////////////////////////////////////////////////////////////

struct GerstnerWaveParams
{
    float2 windDirection;

    float amplitude;
    float steepness;
    float wavelength;
};

struct FBMParams
{
    uint octaves;
    float lacunarity;
    float gain;
    float noiseScale;

    float noiseSpeed;
    float rippleAmp;

    float lowFreq;
    float highFreq;
};

static const uint NUM_WAVES = 3;
cbuffer cbWaterPassParams : register(b2)
{
    GerstnerWaveParams gGerstnerWaveParams[NUM_WAVES];

    FBMParams gFBMParams;

    float4 gDeepColor;
    float4 gShallowColor;
    float gShallowFade;

    float gWavesTotalAmplitude;
};

Texture2D<float> gNoiseTexture : register(t0);

float3 calculateWaveOffsets(float3 vertex, GerstnerWaveParams params, float t)
{
    float distance = (params.windDirection.x * vertex.x + params.windDirection.y * vertex.z);

    float k = TWO_PI / params.wavelength;
    float angularFreq = sqrt(G * k);
    float phase = (k * distance) - (angularFreq * t);

    float phaseCosine = cos(phase);
    float phaseSine = sin(phase);

    float dx = (params.steepness * params.amplitude * params.windDirection.x * phaseCosine);
    float dy = params.amplitude * phaseSine;
    float dz = (params.steepness * params.amplitude * params.windDirection.y * phaseCosine);

    return float3(dx, dy, dz);
}

float2 fbm2D(float2 uv, FBMParams params, Texture2D<float> noiseTex, float t)
{
    float sum = 0.0f;
    float maxAmplitude = 0.0f;
    float amplitude = 1.0f;
    float frequency = params.noiseScale;
    float2 offset = float2(params.noiseSpeed * t, 0.0f);

    [loop]
    for (uint i = 0; i < params.octaves; ++i)
    {
        float noise = noiseTex.SampleLevel(gsamLinearWrap, uv * frequency + offset, 0).r;
        sum += amplitude * noise;

        frequency *= params.lacunarity;
        maxAmplitude += amplitude;
        amplitude *= params.gain;
    }

    return float2(sum, maxAmplitude) * params.rippleAmp;
}

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangentU : TANGENT;
    float2 textureCoord : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 textureCoord : TEXCOORD0;
    float normalizedHeight : TEXCOORD1;
};

VSOutput vsMain(VSInput input)
{
    VSOutput result;

    float3 waveOffset = float3(0.0f, 0.0f, 0.0f);

    [unroll]
    for(uint i = 0; i < NUM_WAVES; ++i)
    {
        waveOffset += calculateWaveOffsets(input.position, gGerstnerWaveParams[i], gTotalTime);
    }

    float2 swell = fbm2D(input.textureCoord, gFBMParams, gNoiseTexture, gTotalTime);
    waveOffset.y += swell.x;

    float3 finalPosition = float3(
        input.position.x + waveOffset.x,
        waveOffset.y,
        input.position.z + waveOffset.z
    );

    float4x4 mvp = mul(gView, gProj);
    result.position = mul(float4(finalPosition, 1.0f), mvp);
    result.textureCoord = input.textureCoord;
    result.normalizedHeight = saturate(finalPosition.y / swell.y);

    result.normal = input.normal;
    return result;
}

float4 psMain(VSOutput input) : SV_TARGET
{
    float3 finalColor = lerp(gDeepColor.rgb, gShallowColor.rgb, input.normalizedHeight);

    return float4(finalColor, 0.8f);
}
)###";
}

namespace engine
{
    WaterPassData::WaterPassData()
    {
        reset();
    }

    void WaterPassData::debugUIRender()
    {
        ImGui::Begin("Water Pass Data");

        debugUICommonParams();
        if (ImGui::CollapsingHeader("Water Pass Params"))
        {
            WaterPassParams& wp = mWaterPassParams;
            // Gerstner Waves
            if (ImGui::TreeNode("Gerstner Waves"))
            {
                for (unsigned int i = 0; i < WaterPassParams::NUM_WAVES; ++i)
                {
                    std::string waveName = "Wave " + std::to_string(i);
                    if (ImGui::TreeNode(waveName.c_str()))
                    {
                        auto& wave = wp.gerstnerWaveParams[i];
                        ImGui::InputFloat2("Direction", (float*)&wave.direction);
                        ImGui::InputFloat("Amplitude", &wave.amplitude);
                        ImGui::InputFloat("Steepness", &wave.steepness);
                        ImGui::InputFloat("Wavelength", &wave.wavelength);
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }

            // FBM Params
            if (ImGui::TreeNode("FBM Params"))
            {
                ImGui::InputInt("Octaves", (int*)&wp.fbmParams.octaves);
                ImGui::InputFloat("Lacunarity", &wp.fbmParams.lacunarity);
                ImGui::InputFloat("Gain", &wp.fbmParams.gain);
                ImGui::InputFloat("Noise Scale", &wp.fbmParams.noiseScale);

                ImGui::InputFloat("Noise Speed", &wp.fbmParams.noiseSpeed);
                ImGui::InputFloat("Ripple Amplitude", &wp.fbmParams.rippleAmp);

                ImGui::InputFloat("Low Frequency", &wp.fbmParams.lowFreq);
                ImGui::InputFloat("High Frequency", &wp.fbmParams.highFreq);
                ImGui::TreePop();
            }

            // Colors
            if (ImGui::TreeNode("Colors"))
            {
                ImGui::ColorEdit4("Deep Color", (float*)&wp.deepColor);
                ImGui::ColorEdit4("Shallow Color", (float*)&wp.shallowColor);
                ImGui::InputFloat("Shallow Fade", &wp.shallowFade);
                ImGui::TreePop();
            }

            // Derived params
            ImGui::InputFloat("Waves Total Amplitude", &wp.wavesTotalAmplitude);
        }

        // Water Mesh Info
        if (ImGui::CollapsingHeader("Water Mesh"))
        {
            ImGui::Text("Vertices: %zu", mWaterMesh.vertices.size());
            ImGui::Text("Indices: %zu", mWaterMesh.indices32.size());
        }
        debugUISave();

        ImGui::End();
    }

    void WaterPassData::reset()
    {
        setEyePosition({ 5.0f, 10.0f, -25.0f });

        setProjectionMatrix(mat4f(), 0.01f, 1000.0f);

        setRenderTargetSize(4096, 2160);

        mWaterPassParams.gerstnerWaveParams[0].direction = vec2f(1.0f, 0.0f);
        mWaterPassParams.gerstnerWaveParams[0].amplitude = 0.2f;
        mWaterPassParams.gerstnerWaveParams[0].steepness = 0.80f;
        mWaterPassParams.gerstnerWaveParams[0].wavelength = 10.0f;

        mWaterPassParams.gerstnerWaveParams[1].direction = vec2f(-0.5f, 0.866f);
        mWaterPassParams.gerstnerWaveParams[1].amplitude = 0.1f;
        mWaterPassParams.gerstnerWaveParams[1].steepness = 0.60f;
        mWaterPassParams.gerstnerWaveParams[1].wavelength = 6.0f;

        mWaterPassParams.gerstnerWaveParams[2].direction = vec2f(-0.5f, -0.866f);
        mWaterPassParams.gerstnerWaveParams[2].amplitude = 0.15f;
        mWaterPassParams.gerstnerWaveParams[2].steepness = 0.40f;
        mWaterPassParams.gerstnerWaveParams[2].wavelength = 3.0f;

        mWaterPassParams.fbmParams.octaves = 6;
        mWaterPassParams.fbmParams.lacunarity = 1.8f;
        mWaterPassParams.fbmParams.gain = 0.7f;
        mWaterPassParams.fbmParams.noiseScale = 0.5f;
        mWaterPassParams.fbmParams.noiseSpeed = 0.05f;
        mWaterPassParams.fbmParams.rippleAmp = 1.5f;
        mWaterPassParams.fbmParams.lowFreq = 0.0f;
        mWaterPassParams.fbmParams.highFreq = 1.0f;

        mWaterPassParams.deepColor = vec4f(0.0f, 0.3f, 0.5f, 1.0f);
        mWaterPassParams.shallowColor = vec4f(0.4f, 0.9f, 1.0f, 1.0f);
        mWaterPassParams.shallowFade = 5.0f;

        mWaterPassParams.wavesTotalAmplitude = 0.0f;
        for (int i = 0; i < WaterPassParams::NUM_WAVES; ++i)
        {
            mWaterPassParams.wavesTotalAmplitude += mWaterPassParams.gerstnerWaveParams[i].amplitude;
        }

        mWaterMesh = mGeoGen.createGrid(120.0f, 120.0f, 64, 64);
    }
}

namespace engine
{
    WaterPass::WaterPass(graphics::Device& device, ResourceManager& resourceManager)
        : mWaterData(),
        mStaticShaderResourceTableLayout{
            graphics::ShaderVisibility::Vertex | graphics::ShaderVisibility::Pixel,
            {
                {graphics::DescriptorRangeType::CBV, 2},
                {graphics::DescriptorRangeType::SRV, 1}
            }
        },
        mShaderResources(resourceManager.createResourceTable("GerstnerWaveWaterShader_ResourceTable", mStaticShaderResourceTableLayout, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::StaticGPU)),
        mWaterPassSI(
            { {graphics::ShaderVisibility::Vertex, 4} },
            { mStaticShaderResourceTableLayout }
        ),
        mWaterPassPSO(createPSO(device)),
        mRTV(),
        mRenderTargetDescription{
            graphics::HeapWithAllocator::ResourceCategory::RenderTargetData,
            {
                "RT_Water_RenderPass",
                (size_t)mWaterData.getPassConstants().renderTargetSize.x,
                (size_t)mWaterData.getPassConstants().renderTargetSize.y,
                1, 1,
                graphics::GPUResourceDimension::Texture2D,
                graphics::DefaultFormats::R16G16B16A16_UNORM,
                graphics::GPUResourceFlags::AllowRenderTarget,
            }
        },
        mWaterRI()
    {
        // RenderTargets
        resourceManager.createRenderTarget(mRenderTargetDescription);
        mRTV = resourceManager.getRenderTargetRTV("RT_Water_RenderPass");

        // ConstantBuffers
        graphics::HeapWithAllocator::ResourceDescription commonPassConstantsBuffer{
            graphics::HeapWithAllocator::ResourceCategory::UploadPersistentData,
            { "CB_CommonPassParams_Water_RenderPass", maths::alignUp(sizeof(PassConstants), 256) }
        };
        resourceManager.createBuffer(commonPassConstantsBuffer);
        mCommonPassConstantsBuffer = &resourceManager.getBuffer("CB_CommonPassParams_Water_RenderPass");

        graphics::HeapWithAllocator::ResourceDescription waterPassConstantsBufferUploader{
            graphics::HeapWithAllocator::ResourceCategory::UploadData,
            { "Uploader_CB_WaterPassParams_Water_RenderPass", maths::alignUp(sizeof(WaterPassData::WaterPassParams), 256) }
        };
        resourceManager.createBuffer(waterPassConstantsBufferUploader);

        graphics::HeapWithAllocator::ResourceDescription waterPassConstantsBuffer{
            graphics::HeapWithAllocator::ResourceCategory::ConstantsData,
            { "CB_WaterPassParams_Water_RenderPass", maths::alignUp(sizeof(WaterPassData::WaterPassParams), 256) }
        };
        resourceManager.createBuffer(waterPassConstantsBuffer);

        // MeshBuffers
        const size_t vbByteSize = mWaterData.getWaterMesh().getVBByteSize();
        const size_t ibByteSize = mWaterData.getWaterMesh().getIB16ByteSize();

        graphics::HeapWithAllocator::ResourceDescription waterMeshVertexBufferUploader{
            graphics::HeapWithAllocator::ResourceCategory::UploadData,
            { "Uploader_VB_WaterMesh_Water_RenderPass", vbByteSize }
        };
        resourceManager.createBuffer(waterMeshVertexBufferUploader);

        graphics::HeapWithAllocator::ResourceDescription waterMeshVertexBuffer{
            graphics::HeapWithAllocator::ResourceCategory::MeshData,
            { "VB_WaterMesh_Water_RenderPass", vbByteSize }
        };
        resourceManager.createBuffer(waterMeshVertexBuffer);

        graphics::HeapWithAllocator::ResourceDescription waterMeshIndexBufferUploader{
            graphics::HeapWithAllocator::ResourceCategory::UploadData,
            { "Uploader_IB_WaterMesh_Water_RenderPass", ibByteSize }
        };
        resourceManager.createBuffer(waterMeshIndexBufferUploader);

        graphics::HeapWithAllocator::ResourceDescription waterMeshIndexBuffer{
            graphics::HeapWithAllocator::ResourceCategory::MeshData,
            { "IB_WaterMesh_Water_RenderPass", ibByteSize }
        };
        resourceManager.createBuffer(waterMeshIndexBuffer);

        MeshGeometry geo;
        SubmeshGeometry waterSubmesh;
        waterSubmesh.indexCount = (unsigned int)mWaterData.getWaterMesh().getIndices16().size();

        geo.name = "RI_Geometry_Water_Mesh";
        geo.vertexBuffer = &resourceManager.getBuffer("VB_WaterMesh_Water_RenderPass");
        geo.indexBuffer = &resourceManager.getBuffer("IB_WaterMesh_Water_RenderPass");
        geo.vertexByteStride = sizeof(GeometryGenerator::Vertex);
        geo.drawArgs["RI_Submesh_Geometry_Water_Mesh"] = waterSubmesh;
        mWaterRI.geometry = geo;

        mWaterPassSI.setResourceTable(mShaderResources);
    }

    void WaterPass::copyDataToGPU(graphics::CopyContext& context, ResourceManager& resourceManager)
    {
        // Water Pass Parameters
        graphics::Buffer& waterPassConstantsBufferUploader = resourceManager.getBuffer("Uploader_CB_WaterPassParams_Water_RenderPass");
        {
            void* waterPassConstants = waterPassConstantsBufferUploader.map(context.getDevice());
            memcpy(waterPassConstants, &mWaterData.getWaterParams(), maths::alignUp(sizeof(WaterPassData::WaterPassParams), 256));
            waterPassConstantsBufferUploader.unmap();
        }
        graphics::Buffer& waterPassConstantsBuffer = resourceManager.getBuffer("CB_WaterPassParams_Water_RenderPass");
        context.copyBuffer(waterPassConstantsBuffer, waterPassConstantsBufferUploader);

        // Water Mesh Vertex Buffer
        graphics::Buffer& waterMeshVertexBufferUploader = resourceManager.getBuffer("Uploader_VB_WaterMesh_Water_RenderPass");
        {
            void* destination = waterMeshVertexBufferUploader.map(context.getDevice());
            memcpy(destination, mWaterData.getWaterMesh().vertices.data(), mWaterData.getWaterMesh().getVBByteSize());
            waterMeshVertexBufferUploader.unmap();
        }
        graphics::Buffer& waterMeshVertexBuffer = resourceManager.getBuffer("VB_WaterMesh_Water_RenderPass");
        context.copyBuffer(waterMeshVertexBuffer, waterMeshVertexBufferUploader);

        // Water Mesh Index Buffer
        graphics::Buffer& waterMeshIndexBufferUploader = resourceManager.getBuffer("Uploader_IB_WaterMesh_Water_RenderPass");
        {
            void* destination = waterMeshIndexBufferUploader.map(context.getDevice());
            memcpy(destination, mWaterData.getWaterMesh().getIndices16().data(), mWaterData.getWaterMesh().getIB16ByteSize());
            waterMeshIndexBufferUploader.unmap();
        }
        graphics::Buffer& waterMeshIndexBuffer = resourceManager.getBuffer("IB_WaterMesh_Water_RenderPass");
        context.copyBuffer(waterMeshIndexBuffer, waterMeshIndexBufferUploader);

        mWaterData.setIsDirty(false);
    }

    void WaterPass::freeGPUUploadData(ResourceManager& resourceManager)
    {
        resourceManager.releaseBuffer("Uploader_CB_WaterPassParams_Water_RenderPass", graphics::HeapWithAllocator::ResourceCategory::UploadData);
        resourceManager.releaseBuffer("Uploader_VB_WaterMesh_Water_RenderPass", graphics::HeapWithAllocator::ResourceCategory::UploadData);
        resourceManager.releaseBuffer("Uploader_IB_WaterMesh_Water_RenderPass", graphics::HeapWithAllocator::ResourceCategory::UploadData);
    }

    void WaterPass::resize(const unsigned int width, const unsigned int height, graphics::Device& device, ResourceManager& resourceManager)
    {
        mWaterData.setRenderTargetSize(width, height);
        mRenderTargetDescription.resource.width = width;
        mRenderTargetDescription.resource.height = height;

        resourceManager.destroyDescriptor("RT_Water_RenderPass");
        resourceManager.releaseRenderTarget("RT_Water_RenderPass", mRenderTargetDescription.category);
        resourceManager.createRenderTarget(mRenderTargetDescription);
        mRTV = resourceManager.getRenderTargetRTV("RT_Water_RenderPass");

        const float fov = 0.25f * (float)M_PI;
        const float aspectRatio = (float)width / (float)height;
        const float nearZ = mWaterData.getPassConstants().nearZ;
        const float farZ = mWaterData.getPassConstants().farZ;
        const mat4f newProjectionMatrix = perspectiveProjection(fov, aspectRatio, nearZ, farZ);
        mWaterData.setProjectionMatrix(newProjectionMatrix, nearZ, farZ);

        mShaderResources.setResource(device, resourceManager.getBufferCBV("CB_CommonPassParams_Water_RenderPass"), 0, 0);
        mShaderResources.setResource(device, resourceManager.getBufferCBV("CB_WaterPassParams_Water_RenderPass"), 0, 1);
        mShaderResources.setResource(device, resourceManager.getTextureSRV("UA_Water_Noise_Texture", "SR_Noise_Texture"), 1);
    }

    void WaterPass::update(const double timeStep)
    {
        const vec3f pos = mWaterData.getPassConstants().eyePosition;
        const vec3f target(0.0f);
        const vec3f up(0.0f, 1.0f, 0.0f);

        mWaterData.setViewMatrix(lookAtLH<float>(pos, target, up));
    }

    void WaterPass::getInputOutputs(graphics::GraphicContext& context, ResourceManager& resourceManager)
    {
        graphics::RenderTarget& renderTarget = resourceManager.getRenderTarget("RT_Water_RenderPass");
        context.transitionBarrier(renderTarget.getTexture(), graphics::GPUResourceStates::RenderTarget);

        graphics::Buffer& commonPassConstantsBuffer = resourceManager.getBuffer("CB_CommonPassParams_Water_RenderPass");
        context.transitionBarrier(commonPassConstantsBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer);

        graphics::Buffer& waterPassConstantsBuffer = resourceManager.getBuffer("CB_WaterPassParams_Water_RenderPass");
        context.transitionBarrier(waterPassConstantsBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer);

        graphics::Buffer& vertexBuffer = resourceManager.getBuffer("VB_WaterMesh_Water_RenderPass");
        context.transitionBarrier(vertexBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer);

        graphics::Buffer& indexBuffer = resourceManager.getBuffer("IB_WaterMesh_Water_RenderPass");
        context.transitionBarrier(indexBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer);

        graphics::Texture& noiseTexture = resourceManager.getTexture("UA_Water_Noise_Texture");
        context.transitionBarrier(noiseTexture, graphics::GPUResourceStates::Common);

        context.flushResourceBarriers();
    }

    void WaterPass::render(graphics::GraphicContext& context, const FrameConstants& frameConstants)
    {
        context.setPipelineState(mWaterPassPSO);
        context.setRootSignature(mWaterPassSI.getRootSignature(context.getDevice()));

        mWaterPassSI.setConstants({ frameConstants.totalTime, frameConstants.deltaTime, 0x1deadb0b, 0x1deadb0b }, 0);
        void* commonPassConstants = mCommonPassConstantsBuffer->map(context.getDevice());
        const PassConstants& passConstants = mWaterData.getPassConstants();
        memcpy(commonPassConstants, &passConstants, sizeof(PassConstants));
        mCommonPassConstantsBuffer->unmap();

        mWaterPassSI.finalize(context);

        context.setRenderTargets(mRTV);

        graphics::ViewPort viewport{};
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = (float)mRenderTargetDescription.resource.width;
        viewport.height = (float)mRenderTargetDescription.resource.height;
        context.setViewport(viewport);
        graphics::Box scissor{};
        scissor.left = (unsigned int)viewport.x;
        scissor.top = (unsigned int)viewport.y;
        scissor.right = (unsigned int)viewport.x + (unsigned int)viewport.width;
        scissor.bottom = (unsigned int)viewport.y + (unsigned int)viewport.height;
        context.setScissor(scissor);

        const float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        context.clearColor(mRTV, color);

        graphics::BufferView vertexBufferView = mWaterRI.geometry.vertexBufferView();
        context.setVertexBuffers(0, &vertexBufferView);

        graphics::BufferView indexBufferView = mWaterRI.geometry.indexBufferView();
        context.setIndexBuffer(indexBufferView);

        context.setPrimitiveTopology(mWaterRI.primitiveTopology);

        const SubmeshGeometry drawData = mWaterRI.geometry.drawArgs.at("RI_Submesh_Geometry_Water_Mesh");
        context.drawIndexedInstanced(drawData.indexCount, 1, 0, 0, 0);
    }

    graphics::GraphicsPipelineState WaterPass::createPSO(graphics::Device& device)
    {
        graphics::GraphicsPipelineState::Description psoDesc;
        psoDesc.name = "Graphics_PSO_Water";
        psoDesc.inputLayout = {
            {"POSITION", graphics::DefaultFormats::R32G32B32_FLOAT, 0, graphics::InputClassification::PerVertex},
            {"NORMAL", graphics::DefaultFormats::R32G32B32_FLOAT, 12, graphics::InputClassification::PerVertex},
            {"TANGENT", graphics::DefaultFormats::R32G32B32_FLOAT, 24, graphics::InputClassification::PerVertex},
            {"TEXCOORD", graphics::DefaultFormats::R32G32_FLOAT, 36, graphics::InputClassification::PerVertex},
        };
        psoDesc.rootSignature = &(mWaterPassSI.getRootSignature(device));

        psoDesc.vertexShader = graphics::CommandContext::compileShader(
            "WaterVS",
            shaders::GerstnerWaveWaterShader,
            "vs_5_0",
            "vsMain"
        );
        psoDesc.pixelShader = graphics::CommandContext::compileShader(
            "WaterPS",
            shaders::GerstnerWaveWaterShader,
            "ps_5_0",
            "psMain"
        );
        psoDesc.numRenderTargets = 1;
        psoDesc.rtvFormats[0] = graphics::DefaultFormats::R16G16B16A16_UNORM;

        graphics::GraphicsPipelineState::BlendDescription::RenderTargetBlendDescription transparentWater;
        transparentWater.blendEnable = true;
        transparentWater.logicOpEnable = false;
        transparentWater.srcBlend = graphics::Blend::SourceAlpha;
        transparentWater.destBlend = graphics::Blend::InverseSourceAlpha;
        psoDesc.blendDescription.renderTarget[0] = transparentWater;

        psoDesc.cullMode = graphics::CullMode::None;
        return graphics::GraphicsPipelineState(device, psoDesc);
    }
}
