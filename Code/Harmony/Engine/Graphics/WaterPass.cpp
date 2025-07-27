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
cbuffer cbWaterParams : register(b2)
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
    float sum = 0;
    float maxAmplitude = 0;
    float amplitude = 1;
    float frequency = params.noiseScale;
    float2 offset = float2(params.noiseSpeed * t, 0);

    [unroll]
    for (uint i = 0; i < 6; ++i)
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

    //[unroll]
    //for(uint i = 0; i < NUM_WAVES; ++i)
    //{
    //    waveOffset += calculateWaveOffsets(input.position, gGerstnerWaveParams[i], gTotalTime);
    //}

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

    return float4(finalColor, 1.0f);
}
)###";
}

namespace engine
{
    namespace renderPass
    {
        Water::Water(graphics::dx12::Device& device, ResourceManager& resourceManager)
            :
            mStaticShaderResourceTableLayout{
                graphics::ShaderVisibility::Vertex | graphics::ShaderVisibility::Pixel,
                {
                    {graphics::DescriptorRangeType::CBV, 2},
                    {graphics::DescriptorRangeType::SRV, 1}
                }
            },
            mShaderResources(resourceManager.createResourceTable("GerstnerWaveWaterShader_ResourceTable", mStaticShaderResourceTableLayout, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::StaticGPU)),
            mWaterSI(
                { {graphics::ShaderVisibility::Vertex, 4} },
                { mStaticShaderResourceTableLayout }
            ),
            mWaterPSO(createPSO(device)),
            mRTV(resourceManager.allocateDescriptors(1, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::RenderTargetCPU)),
            mRenderTargetDescription{
                graphics::HeapWithAllocator::ResourceCategory::RenderTarget,
                {
                    "RT_Water_RenderPass",
                    4096, 2160,
                    1, 1,
                    graphics::GPUResourceDimension::Texture2D,
                    graphics::DefaultFormats::R16G16B16A16_UNORM,
                    graphics::GPUResourceFlags::AllowRenderTarget,
                }
            },
            mWaterRI()
        {
            {// RenderTargets
                resourceManager.createRenderTarget(mRenderTargetDescription);
                resourceManager.getRenderTarget("RT_Water_RenderPass").createRenderTargetView(device, mRTV);
            }

            {// WaterParams

                WaterParams waterParams;
                {
                    waterParams.gerstnerWaveParams[0].direction = vec2f(1.0f, 0.0f);
                    waterParams.gerstnerWaveParams[0].amplitude = 0.2f;
                    waterParams.gerstnerWaveParams[0].steepness = 0.80f;
                    waterParams.gerstnerWaveParams[0].wavelength = 10.0f;

                    waterParams.gerstnerWaveParams[1].direction = vec2f(-0.5f, 0.866f);
                    waterParams.gerstnerWaveParams[1].amplitude = 0.1f;
                    waterParams.gerstnerWaveParams[1].steepness = 0.60f;
                    waterParams.gerstnerWaveParams[1].wavelength = 6.0f;

                    waterParams.gerstnerWaveParams[2].direction = vec2f(-0.5f, -0.866f);
                    waterParams.gerstnerWaveParams[2].amplitude = 0.15f;
                    waterParams.gerstnerWaveParams[2].steepness = 0.40f;
                    waterParams.gerstnerWaveParams[2].wavelength = 3.0f;

                    waterParams.fbmParams.octaves = 6;
                    waterParams.fbmParams.lacunarity = 1.8f;
                    waterParams.fbmParams.gain = 0.7f;
                    waterParams.fbmParams.noiseScale = 0.5f;
                    waterParams.fbmParams.noiseSpeed = 0.05f;
                    waterParams.fbmParams.rippleAmp = 1.5f;
                    waterParams.fbmParams.lowFreq = 0.0f;
                    waterParams.fbmParams.highFreq = 1.0f;

                    waterParams.deepColor = vec4f(0.0f, 0.3f, 0.5f, 1.0f);
                    waterParams.shallowColor = vec4f(0.4f, 0.9f, 1.0f, 1.0f);
                    waterParams.shallowFade = 5.0f;

                    waterParams.wavesTotalAmplitude = 0.0f;
                    for (int i = 0; i < WaterParams::NUM_WAVES; ++i)
                    {
                        waterParams.wavesTotalAmplitude += waterParams.gerstnerWaveParams[i].amplitude;
                    }

                }

                {// ConstantBuffers
                    graphics::HeapWithAllocator::ResourceDescription commonPassConstantsBuffer{
                        graphics::HeapWithAllocator::ResourceCategory::Upload,
                        { "CB_CommonPassParams_Water_RenderPass", sizeof(PassConstants) }
                    };
                    resourceManager.createBuffer(commonPassConstantsBuffer);
                    mCommonPassConstantsBuffer = &resourceManager.getBuffer("CB_CommonPassParams_Water_RenderPass");

                    graphics::HeapWithAllocator::ResourceDescription waterPassConstantsBufferUploader{
                        graphics::HeapWithAllocator::ResourceCategory::Upload,
                        { "Uploader_CB_WaterPassParams_Water_RenderPass", sizeof(WaterParams) }
                    };
                    resourceManager.createBuffer(waterPassConstantsBufferUploader);

                    graphics::HeapWithAllocator::ResourceDescription waterPassConstantsBuffer{
                        graphics::HeapWithAllocator::ResourceCategory::Constants,
                        { "CB_WaterPassParams_Water_RenderPass", sizeof(WaterParams) }
                    };
                    resourceManager.createBuffer(waterPassConstantsBuffer);
                }

                graphics::dx12::Buffer& waterPassConstantsBufferUploader = resourceManager.getBuffer("Uploader_CB_WaterPassParams_Water_RenderPass");
                void* waterPassConstants = waterPassConstantsBufferUploader.map();
                memcpy(waterPassConstants, &waterParams, sizeof(WaterParams));
                waterPassConstantsBufferUploader.unmap();
            }

            {// RenderItems
                GeometryGenerator geoGen;
                GeometryGenerator::MeshData grid = geoGen.createGrid(120.0f, 120.0f, 128, 128);

                const size_t vbByteSize = grid.vertices.size() * sizeof(GeometryGenerator::Vertex);
                const size_t ibByteSize = grid.getIndices16().size() * sizeof(uint16_t);

                {// MeshBuffers

                    graphics::HeapWithAllocator::ResourceDescription waterMeshVertexBufferUploader{
                        graphics::HeapWithAllocator::ResourceCategory::Upload,
                        { "Uploader_VB_WaterMesh_Water_RenderPass", vbByteSize }
                    };
                    resourceManager.createBuffer(waterMeshVertexBufferUploader);
                    graphics::HeapWithAllocator::ResourceDescription waterMeshVertexBuffer{
                        graphics::HeapWithAllocator::ResourceCategory::Mesh,
                        { "VB_WaterMesh_Water_RenderPass", vbByteSize }
                    };
                    resourceManager.createBuffer(waterMeshVertexBuffer);

                    graphics::HeapWithAllocator::ResourceDescription waterMeshIndexBufferUploader{
                        graphics::HeapWithAllocator::ResourceCategory::Upload,
                        { "Uploader_IB_WaterMesh_Water_RenderPass", ibByteSize }
                    };
                    resourceManager.createBuffer(waterMeshIndexBufferUploader);
                    graphics::HeapWithAllocator::ResourceDescription waterMeshIndexBuffer{
                        graphics::HeapWithAllocator::ResourceCategory::Mesh,
                        { "IB_WaterMesh_Water_RenderPass", ibByteSize }
                    };
                    resourceManager.createBuffer(waterMeshIndexBuffer);
                }

                graphics::dx12::Buffer& waterMeshVertexBufferUploader = resourceManager.getBuffer("Uploader_VB_WaterMesh_Water_RenderPass");
                {
                    void* destination = waterMeshVertexBufferUploader.map();
                    memcpy(destination, grid.vertices.data(), vbByteSize);
                    waterMeshVertexBufferUploader.unmap();
                }

                graphics::dx12::Buffer& waterMeshIndexBufferUploader = resourceManager.getBuffer("Uploader_IB_WaterMesh_Water_RenderPass");
                {
                    void* destination = waterMeshIndexBufferUploader.map();
                    memcpy(destination, grid.getIndices16().data(), ibByteSize);
                    waterMeshIndexBufferUploader.unmap();
                }

                MeshGeometry geo;
                SubmeshGeometry waterSubmesh;
                waterSubmesh.indexCount = (unsigned int)grid.getIndices16().size();

                geo.name = "RI_Geometry_Water_Mesh";
                geo.vertexBuffer = &resourceManager.getBuffer("VB_WaterMesh_Water_RenderPass");
                geo.indexBuffer = &resourceManager.getBuffer("IB_WaterMesh_Water_RenderPass");
                geo.vertexByteStride = sizeof(GeometryGenerator::Vertex);
                geo.drawArgs["RI_Submesh_Geometry_Water_Mesh"] = waterSubmesh;
                mWaterRI.geometry = geo;
            }
        }

        void Water::copyDataToGPU(graphics::GraphicContext& context, ResourceManager& resourceManager)
        {
            graphics::dx12::Buffer& waterPassConstantsBufferUploader = resourceManager.getBuffer("Uploader_CB_WaterPassParams_Water_RenderPass");
            graphics::dx12::Buffer& waterPassConstantsBuffer = resourceManager.getBuffer("CB_WaterPassParams_Water_RenderPass");
            context.copyBuffer(waterPassConstantsBuffer, waterPassConstantsBufferUploader);

            graphics::dx12::Buffer& waterMeshVertexBufferUploader = resourceManager.getBuffer("Uploader_VB_WaterMesh_Water_RenderPass");
            graphics::dx12::Buffer& waterMeshVertexBuffer = resourceManager.getBuffer("VB_WaterMesh_Water_RenderPass");
            context.copyBuffer(waterMeshVertexBuffer, waterMeshVertexBufferUploader);

            graphics::dx12::Buffer& waterMeshIndexBufferUploader = resourceManager.getBuffer("Uploader_IB_WaterMesh_Water_RenderPass");
            graphics::dx12::Buffer& waterMeshIndexBuffer = resourceManager.getBuffer("IB_WaterMesh_Water_RenderPass");
            context.copyBuffer(waterMeshIndexBuffer, waterMeshIndexBufferUploader);

            mWaterSI.setResourceTable(mShaderResources);
        }

        void Water::freeGPUUploadData(ResourceManager& resourceManager)
        {
            resourceManager.releaseBuffer("Uploader_CB_WaterPassParams_Water_RenderPass", graphics::HeapWithAllocator::ResourceCategory::Upload);
            resourceManager.releaseBuffer("Uploader_VB_WaterMesh_Water_RenderPass", graphics::HeapWithAllocator::ResourceCategory::Upload);
            resourceManager.releaseBuffer("Uploader_IB_WaterMesh_Water_RenderPass", graphics::HeapWithAllocator::ResourceCategory::Upload);
        }

        void Water::resize(const unsigned int width, const unsigned int height, graphics::dx12::Device& device, ResourceManager& resourceManager)
        {
            mRenderTargetDescription.resource.width = width;
            mRenderTargetDescription.resource.height = height;

            resourceManager.destroyDescriptor("RT_Water_RenderPass");
            resourceManager.releaseRenderTarget("RT_Water_RenderPass", mRenderTargetDescription.category);
            resourceManager.createRenderTarget(mRenderTargetDescription);
            resourceManager.getRenderTarget("RT_Water_RenderPass").createRenderTargetView(device, mRTV);

            float fov = 0.25f * (float)M_PI;
            float aspectRatio = (float)width / (float)height;
            mNearFarZ.x = 0.01f;
            mNearFarZ.y = 1000.0f;
            mProjection = perspectiveProjection(fov, aspectRatio, mNearFarZ.x, mNearFarZ.y);

            mShaderResources.setResource(device, resourceManager.getBufferCBV("CB_CommonPassParams_Water_RenderPass"), 0, 0);
            mShaderResources.setResource(device, resourceManager.getBufferCBV("CB_WaterPassParams_Water_RenderPass"), 0, 1);
            mShaderResources.setResource(device, resourceManager.getTextureSRV("UA_Noise_Texture"), 1);
        }

        void Water::update(const double timeStep)
        {
            updateCamera();
        }

        void Water::getInputOutputs(graphics::GraphicContext& context, ResourceManager& resourceManager)
        {
            graphics::dx12::RenderTarget& renderTarget = resourceManager.getRenderTarget("RT_Water_RenderPass");
            context.transitionBarrier(renderTarget.getTexture(), graphics::GPUResourceStates::RenderTarget);

            graphics::dx12::Buffer& commonPassConstantsBuffer = resourceManager.getBuffer("CB_CommonPassParams_Water_RenderPass");
            context.transitionBarrier(commonPassConstantsBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer);

            graphics::dx12::Buffer& waterPassConstantsBuffer = resourceManager.getBuffer("CB_WaterPassParams_Water_RenderPass");
            context.transitionBarrier(waterPassConstantsBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer);

            graphics::dx12::Buffer& vertexBuffer = resourceManager.getBuffer("VB_WaterMesh_Water_RenderPass");
            context.transitionBarrier(vertexBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer);

            graphics::dx12::Buffer& indexBuffer = resourceManager.getBuffer("IB_WaterMesh_Water_RenderPass");
            context.transitionBarrier(indexBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer);

            graphics::dx12::Texture& noiseTexture = resourceManager.getTexture("UA_Noise_Texture");
            context.transitionBarrier(noiseTexture, graphics::GPUResourceStates::AllShaderResource);

            context.flushResourceBarriers();
        }

        void Water::render(graphics::GraphicContext& context, const FrameConstants& frameConstants)
        {
            context.setPipelineState(mWaterPSO);
            context.setRootSignature(mWaterSI.getRootSignature(context.getDevice()));

            mWaterSI.setConstants({ frameConstants.totalTime, frameConstants.deltaTime, 0x1deadb0b, 0x1deadb0b }, 0);
            PassConstants passConstants;
            passConstants.view = transpose(mView);
            passConstants.invView = inverse(mView);
            passConstants.eyePosition = mEyePos;
            passConstants.projection = transpose(mProjection);
            passConstants.invProjection = inverse(mProjection);
            passConstants.nearZ = mNearFarZ.x;
            passConstants.farZ = mNearFarZ.y;
            passConstants.viewProjection = mView * mProjection;
            passConstants.invViewProjection = inverse(passConstants.viewProjection);
            passConstants.renderTargetSize.x = (float)mRenderTargetDescription.resource.width;
            passConstants.renderTargetSize.y = (float)mRenderTargetDescription.resource.height;
            passConstants.invRenderTargetSize = 1 / passConstants.renderTargetSize;
            void* commonPassConstants = mCommonPassConstantsBuffer->map();
            memcpy(commonPassConstants, &passConstants, sizeof(PassConstants));
            mCommonPassConstantsBuffer->unmap();

            mWaterSI.finalize(context);

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

            graphics::dx12::BufferView vertexBufferView = mWaterRI.geometry.vertexBufferView();
            context.setVertexBuffers(0, &vertexBufferView);

            graphics::dx12::BufferView indexBufferView = mWaterRI.geometry.indexBufferView();
            context.setIndexBuffer(indexBufferView);

            context.setPrimitiveTopology(mWaterRI.primitiveTopology);

            const SubmeshGeometry drawData = mWaterRI.geometry.drawArgs.at("RI_Submesh_Geometry_Water_Mesh");
            context.drawIndexedInstanced(drawData.indexCount, 1, 0, 0, 0);
        }

        void Water::updateCamera()
        {
            vec3f pos = mEyePos;
            vec3f target(0.0f);
            vec3f up(0.0f, 1.0f, 0.0f);

            mView = lookAtLH<float>(pos, target, up);
        }

        graphics::dx12::GraphicsPipelineState Water::createPSO(graphics::dx12::Device& device)
        {
            graphics::dx12::GraphicsPipelineState::Description psoDesc;
            psoDesc.name = "Graphics_PSO_Water";
            psoDesc.inputLayout = {
                {"POSITION", graphics::DefaultFormats::R32G32B32_FLOAT, 0, graphics::InputClassification::PerVertex},
                {"NORMAL", graphics::DefaultFormats::R32G32B32_FLOAT, 12, graphics::InputClassification::PerVertex},
                {"TANGENT", graphics::DefaultFormats::R32G32B32_FLOAT, 24, graphics::InputClassification::PerVertex},
                {"TEXCOORD", graphics::DefaultFormats::R32G32_FLOAT, 36, graphics::InputClassification::PerVertex},
            };
            psoDesc.rootSignature = &(mWaterSI.getRootSignature(device));

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
            psoDesc.cullMode = graphics::CullMode::None;
            return graphics::dx12::GraphicsPipelineState(device, psoDesc);
        }
    }
}
