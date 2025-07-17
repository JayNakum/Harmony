#include "WhirlpoolHeightMapPass.h"

#include "GeometryGenerator.h"

#include "Maths.h"

namespace shaders
{
    static const char* WhirlpoolHeightMapShader = R"###(
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

static const uint NUM_WAVES = 3;
struct WaveParams
{
    float2 windDirection;
    float2 waveCenter; // RADIAL WAVES
    float amplitude;
    float steepness;
    float wavelength;
    uint _waveParamsPadding0;
};

static const uint NUM_WHIRLPOOLS = 1;
struct WhirlpoolParams
{
    float2 whirlpoolCenter;
    float swirlRadius;
    float wrappedSwirlAngle;

    float sinkRadius;
    float sinkDepth;
    uint _whirlpoolParamsPaddingFinal[2];
};

cbuffer cbWhirlpoolHeightMapParams : register(b2)
{
    WaveParams gWaveParams[NUM_WAVES];

    WhirlpoolParams gWhirlpoolParams[NUM_WHIRLPOOLS];

    float gWavesMaxHeight;
    uint _whirlpoolParamsPaddingFinal[3];
};

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

    float normalizedHeight : TEXCOORD0;
};

float calculateWavePhase(float waveLength, float distance, float t)
{
    float k = TWO_PI / waveLength;
    float angularFreq = sqrt(G * k);
    float phase = (k * distance) - (angularFreq * t);

    return phase;
}

float3 calculateWaveOffsets(float3 vertex, float2 direction, float amplitude, float steepness, float waveLength, float t)
{
    float distance = (direction.x * vertex.x + direction.y * vertex.z);

    // RADIAL WAVES
    //float distance = length(radial);
    //if (distance > 0.0001f)
    //{
    //    direction /= distance; // Normalize
    //}
    //else
    //{
    //    direction /= max(distance, 0.0001f); // float2(1, 0); // Fallback if exactly at center
    //}

    float phase = calculateWavePhase(waveLength, distance, t);
    float phaseCosine = cos(phase);
    float phaseSine = sin(phase);

    float dx = (steepness * amplitude * direction.x * phaseCosine);
    float dy = amplitude * phaseSine;
    float dz = (steepness * amplitude * direction.y * phaseCosine);

    return float3(dx, dy, dz);
}

float3 simulateWaves(float3 vertex)
{
    // RADIAL WAVES
    //const float2 center = float2(0.0f, 0.0f);

    float3 waveOffset = float3(0.0f, 0.0f, 0.0f);
    [unroll]
    for(uint i = 0; i < NUM_WAVES; ++i)
    {
        float2 direction = gWaveParams[i].windDirection;

        // RADIAL WAVES
        //float2 direction = vertex.xz - center;

        waveOffset += calculateWaveOffsets(
                            vertex,
                            direction,
                            gWaveParams[i].amplitude,
                            gWaveParams[i].steepness,
                            gWaveParams[i].wavelength,
                            gTotalTime
                        );
    }

    return float3(
        vertex.x + waveOffset.x,
        waveOffset.y,
        vertex.z + waveOffset.z
    );
}

float3 simulateWhirlpool(float3 vertex)
{
    float2 centeredXZ = vertex.xz - gWhirlpoolParams[0].whirlpoolCenter;
    float radius = length(centeredXZ);

    float factor = saturate(1.0f - (radius / gWhirlpoolParams[0].swirlRadius));
    float swirlAngle = gWhirlpoolParams[0].wrappedSwirlAngle * factor;

    float swirlAngleCosine = cos(swirlAngle);
    float swirlAngleSine = sin(swirlAngle);
    float2 rotatedXZ = float2(
        centeredXZ.x * swirlAngleCosine - centeredXZ.y * swirlAngleSine,
        centeredXZ.x * swirlAngleSine + centeredXZ.y * swirlAngleCosine
    );

    float sinkFactor = saturate(1.0f - (radius / gWhirlpoolParams[0].sinkRadius));
    float dySink = gWhirlpoolParams[0].sinkDepth * sinkFactor;

    return float3(
        gWhirlpoolParams[0].whirlpoolCenter.x + rotatedXZ.x,
        vertex.y - dySink, // sink down
        gWhirlpoolParams[0].whirlpoolCenter.y + rotatedXZ.y
    );
}

VSOutput vsMain(VSInput input)
{
    VSOutput result;

    float3 displacedPosition = simulateWaves(input.position);
    float3 finalPosition = simulateWhirlpool(displacedPosition);

    float4x4 mvp = mul(gView, gProj);
    result.position = mul(float4(finalPosition, 1.0f), mvp);

    float maxAmplitude = 0.0f;
    for (int i = 0; i < NUM_WAVES; ++i)
    {
        maxAmplitude += gWaveParams[i].amplitude;
    }
    result.normalizedHeight = saturate(displacedPosition.y / maxAmplitude);

    result.normal = input.normal;
    return result;
}

float4 psMain(VSOutput input) : SV_TARGET
{
    float3 deepColor = float3(0.0f, 0.3f, 0.5f);
    float3 peakColor = float3(1.0f, 1.0f, 1.0f);

    float3 finalColor = lerp(deepColor, peakColor, input.normalizedHeight);

    return float4(finalColor, 1.0f);
}
)###";
}

namespace engine
{
    namespace renderPass
    {
        WhirlpoolHeightMap::WhirlpoolHeightMap(graphics::dx12::Device& device, ResourceManager& resourceManager)
            :
            mStaticShaderResourceTableLayout{
                graphics::ShaderVisibility::Vertex,
                {
                    {graphics::DescriptorRangeType::CBV, 1},
                    {graphics::DescriptorRangeType::CBV, 1}
                }
            },
            mShaderResources(resourceManager.createResourceTable("WhirlpoolHeightMapShader_ResourceTable", mStaticShaderResourceTableLayout, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::StaticGPU)),
            mWhirlpoolHeightMapSI(
                { {graphics::ShaderVisibility::Vertex, 4} },
                { mStaticShaderResourceTableLayout }
            ),
            mWhirlpoolHeightMapPSO(createPSO(device)),
            mRTV(resourceManager.allocateDescriptors(1, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::RenderTargetCPU)),
            mRenderTargetDescription{
                graphics::HeapWithAllocator::ResourceCategory::RenderTarget,
                {
                    "RT_WhirlpoolHeightMap_RenderPass",
                    4096, 2160,
                    1, 1,
                    graphics::GPUResourceDimension::Texture2D,
                    graphics::DefaultFormats::R16G16B16A16_UNORM,
                    graphics::GPUResourceFlags::AllowRenderTarget,
                }
            },
            mWhirlpoolHeightMapRI()
        {
            {// RenderTargets
                resourceManager.createRenderTarget(mRenderTargetDescription);
                resourceManager.getRenderTarget("RT_WhirlpoolHeightMap_RenderPass").createRenderTargetView(device, mRTV);
            }

            {// WhirlpoolHeightMapParams

                WhirlpoolHeightMapParams waterParams;
                {
                    waterParams.waveParams[0].direction = vec2f(1.0f, 0.0f);
                    waterParams.waveParams[0].center = vec2f(0.0f, 0.0f);
                    waterParams.waveParams[0].amplitude = 0.2f;
                    waterParams.waveParams[0].steepness = 0.80f;
                    waterParams.waveParams[0].wavelength = 10.0f;

                    waterParams.waveParams[1].direction = vec2f(-0.5f, 0.866f);
                    waterParams.waveParams[1].center = vec2f(0.0f, 0.0f);
                    waterParams.waveParams[1].amplitude = 0.1f;
                    waterParams.waveParams[1].steepness = 0.60f;
                    waterParams.waveParams[1].wavelength = 6.0f;

                    waterParams.waveParams[2].direction = vec2f(-0.5f, -0.866f);
                    waterParams.waveParams[2].center = vec2f(0.0f, 0.0f);
                    waterParams.waveParams[2].amplitude = 0.15f;
                    waterParams.waveParams[2].steepness = 0.40f;
                    waterParams.waveParams[2].wavelength = 3.0f;

                    waterParams.wavesTotalAmplitude = 0.0f;
                    for (int i = 0; i < WhirlpoolHeightMapParams::NUM_WAVES; ++i)
                    {
                        waterParams.wavesTotalAmplitude += waterParams.waveParams[i].amplitude;
                    }
                }

                {// ConstantBuffers
                    graphics::HeapWithAllocator::ResourceDescription commonPassConstantsBuffer{
                                        graphics::HeapWithAllocator::ResourceCategory::Upload,
                                        { "CB_CommonPassParams_WhirlpoolHeightMap_RenderPass", sizeof(PassConstants) }
                    };
                    resourceManager.createBuffer(commonPassConstantsBuffer);
                    mCommonPassConstantsBuffer = &resourceManager.getBuffer("CB_CommonPassParams_WhirlpoolHeightMap_RenderPass");

                    graphics::HeapWithAllocator::ResourceDescription waterPassConstantsBufferUploader{
                        graphics::HeapWithAllocator::ResourceCategory::Upload,
                        { "Uploader_CB_WhirlpoolHeightMapPassParams_WhirlpoolHeightMap_RenderPass", sizeof(WhirlpoolHeightMapParams) }
                    };
                    resourceManager.createBuffer(waterPassConstantsBufferUploader);

                    graphics::HeapWithAllocator::ResourceDescription waterPassConstantsBuffer{
                                        graphics::HeapWithAllocator::ResourceCategory::Constants,
                                        { "CB_WhirlpoolHeightMapPassParams_WhirlpoolHeightMap_RenderPass", sizeof(WhirlpoolHeightMapParams) }
                    };
                    resourceManager.createBuffer(waterPassConstantsBuffer);
                }

                graphics::dx12::Buffer& waterPassConstantsBufferUploader = resourceManager.getBuffer("Uploader_CB_WhirlpoolHeightMapPassParams_WhirlpoolHeightMap_RenderPass");
                void* waterPassConstants = waterPassConstantsBufferUploader.map();
                memcpy(waterPassConstants, &waterParams, sizeof(WhirlpoolHeightMapParams));
                waterPassConstantsBufferUploader.unmap();
            }

            {// RenderItems
                GeometryGenerator geoGen;
                GeometryGenerator::MeshData grid = geoGen.createGrid(30.0f, 30.0f, 256, 256);

                const size_t vbByteSize = grid.vertices.size() * sizeof(GeometryGenerator::Vertex);
                const size_t ibByteSize = grid.getIndices16().size() * sizeof(uint16_t);

                {// MeshBuffers

                    graphics::HeapWithAllocator::ResourceDescription waterMeshVertexBufferUploader{
                        graphics::HeapWithAllocator::ResourceCategory::Upload,
                        { "Uploader_VB_WhirlpoolHeightMapMesh_WhirlpoolHeightMap_RenderPass", vbByteSize }
                    };
                    resourceManager.createBuffer(waterMeshVertexBufferUploader);
                    graphics::HeapWithAllocator::ResourceDescription waterMeshVertexBuffer{
                        graphics::HeapWithAllocator::ResourceCategory::Mesh,
                        { "VB_WhirlpoolHeightMapMesh_WhirlpoolHeightMap_RenderPass", vbByteSize }
                    };
                    resourceManager.createBuffer(waterMeshVertexBuffer);

                    graphics::HeapWithAllocator::ResourceDescription waterMeshIndexBufferUploader{
                        graphics::HeapWithAllocator::ResourceCategory::Upload,
                        { "Uploader_IB_WhirlpoolHeightMapMesh_WhirlpoolHeightMap_RenderPass", ibByteSize }
                    };
                    resourceManager.createBuffer(waterMeshIndexBufferUploader);
                    graphics::HeapWithAllocator::ResourceDescription waterMeshIndexBuffer{
                        graphics::HeapWithAllocator::ResourceCategory::Mesh,
                        { "IB_WhirlpoolHeightMapMesh_WhirlpoolHeightMap_RenderPass", ibByteSize }
                    };
                    resourceManager.createBuffer(waterMeshIndexBuffer);
                }

                graphics::dx12::Buffer& waterMeshVertexBufferUploader = resourceManager.getBuffer("Uploader_VB_WhirlpoolHeightMapMesh_WhirlpoolHeightMap_RenderPass");
                {
                    void* destination = waterMeshVertexBufferUploader.map();
                    memcpy(destination, grid.vertices.data(), vbByteSize);
                    waterMeshVertexBufferUploader.unmap();
                }

                graphics::dx12::Buffer& waterMeshIndexBufferUploader = resourceManager.getBuffer("Uploader_IB_WhirlpoolHeightMapMesh_WhirlpoolHeightMap_RenderPass");
                {
                    void* destination = waterMeshIndexBufferUploader.map();
                    memcpy(destination, grid.getIndices16().data(), ibByteSize);
                    waterMeshIndexBufferUploader.unmap();
                }

                MeshGeometry geo;
                SubmeshGeometry waterSubmesh;
                waterSubmesh.indexCount = (unsigned int)grid.getIndices16().size();

                geo.name = "RI_Geometry_WhirlpoolHeightMap_Mesh";
                geo.vertexBuffer = &resourceManager.getBuffer("VB_WhirlpoolHeightMapMesh_WhirlpoolHeightMap_RenderPass");
                geo.indexBuffer = &resourceManager.getBuffer("IB_WhirlpoolHeightMapMesh_WhirlpoolHeightMap_RenderPass");
                geo.vertexByteStride = sizeof(GeometryGenerator::Vertex);
                geo.drawArgs["RI_Submesh_Geometry_WhirlpoolHeightMap_Mesh"] = waterSubmesh;
                mWhirlpoolHeightMapRI.geometry = geo;
            }
        }

        void WhirlpoolHeightMap::copyDataToGPU(graphics::GraphicContext& context, ResourceManager& resourceManager)
        {
            graphics::dx12::Buffer& waterPassConstantsBufferUploader = resourceManager.getBuffer("Uploader_CB_WhirlpoolHeightMapPassParams_WhirlpoolHeightMap_RenderPass");
            graphics::dx12::Buffer& waterPassConstantsBuffer = resourceManager.getBuffer("CB_WhirlpoolHeightMapPassParams_WhirlpoolHeightMap_RenderPass");
            context.copyBuffer(waterPassConstantsBuffer, waterPassConstantsBufferUploader);

            graphics::dx12::Buffer& waterMeshVertexBufferUploader = resourceManager.getBuffer("Uploader_VB_WhirlpoolHeightMapMesh_WhirlpoolHeightMap_RenderPass");
            graphics::dx12::Buffer& waterMeshVertexBuffer = resourceManager.getBuffer("VB_WhirlpoolHeightMapMesh_WhirlpoolHeightMap_RenderPass");
            context.copyBuffer(waterMeshVertexBuffer, waterMeshVertexBufferUploader);

            graphics::dx12::Buffer& waterMeshIndexBufferUploader = resourceManager.getBuffer("Uploader_IB_WhirlpoolHeightMapMesh_WhirlpoolHeightMap_RenderPass");
            graphics::dx12::Buffer& waterMeshIndexBuffer = resourceManager.getBuffer("IB_WhirlpoolHeightMapMesh_WhirlpoolHeightMap_RenderPass");
            context.copyBuffer(waterMeshIndexBuffer, waterMeshIndexBufferUploader);

            mWhirlpoolHeightMapSI.setResourceTable(mShaderResources);
        }

        void WhirlpoolHeightMap::freeGPUUploadData(ResourceManager& resourceManager)
        {
            resourceManager.releaseBuffer("Uploader_CB_WhirlpoolHeightMapPassParams_WhirlpoolHeightMap_RenderPass", graphics::HeapWithAllocator::ResourceCategory::Upload);
            resourceManager.releaseBuffer("Uploader_VB_WhirlpoolHeightMapMesh_WhirlpoolHeightMap_RenderPass", graphics::HeapWithAllocator::ResourceCategory::Upload);
            resourceManager.releaseBuffer("Uploader_IB_WhirlpoolHeightMapMesh_WhirlpoolHeightMap_RenderPass", graphics::HeapWithAllocator::ResourceCategory::Upload);
        }

        void WhirlpoolHeightMap::resize(const unsigned int width, const unsigned int height, graphics::dx12::Device& device, ResourceManager& resourceManager)
        {
            mRenderTargetDescription.resource.width = width;
            mRenderTargetDescription.resource.height = height;

            resourceManager.releaseRenderTarget("RT_WhirlpoolHeightMap_RenderPass", mRenderTargetDescription.category);
            resourceManager.createRenderTarget(mRenderTargetDescription);
            resourceManager.getRenderTarget("RT_WhirlpoolHeightMap_RenderPass").createRenderTargetView(device, mRTV);

            float fov = 0.25f * (float)M_PI;
            float aspectRatio = (float)width / (float)height;
            mNearFarZ.x = 0.01f;
            mNearFarZ.y = 1000.0f;
            mProjection = perspectiveProjection(fov, aspectRatio, mNearFarZ.x, mNearFarZ.y);

            mShaderResources.setResource(device, resourceManager.getBuffer("CB_CommonPassParams_WhirlpoolHeightMap_RenderPass"), 0);
            mShaderResources.setResource(device, resourceManager.getBuffer("CB_WhirlpoolHeightMapPassParams_WhirlpoolHeightMap_RenderPass"), 1);
        }

        void WhirlpoolHeightMap::update(const double timeStep)
        {
            updateCamera();
        }

        void WhirlpoolHeightMap::getInputOutputs(graphics::GraphicContext& context, ResourceManager& resourceManager)
        {
            graphics::dx12::RenderTarget& renderTarget = resourceManager.getRenderTarget("RT_WhirlpoolHeightMap_RenderPass");
            context.transitionResource(renderTarget.getTexture(), graphics::GPUResourceStates::RenderTarget);

            graphics::dx12::Buffer& commonPassConstantsBuffer = resourceManager.getBuffer("CB_CommonPassParams_WhirlpoolHeightMap_RenderPass");
            context.transitionResource(commonPassConstantsBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer);

            graphics::dx12::Buffer& waterPassConstantsBuffer = resourceManager.getBuffer("CB_WhirlpoolHeightMapPassParams_WhirlpoolHeightMap_RenderPass");
            context.transitionResource(waterPassConstantsBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer);

            graphics::dx12::Buffer& vertexBuffer = resourceManager.getBuffer("VB_WhirlpoolHeightMapMesh_WhirlpoolHeightMap_RenderPass");
            context.transitionResource(vertexBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer);

            graphics::dx12::Buffer& indexBuffer = resourceManager.getBuffer("IB_WhirlpoolHeightMapMesh_WhirlpoolHeightMap_RenderPass");
            context.transitionResource(indexBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer);

            context.flushResourceBarriers();
        }

        void WhirlpoolHeightMap::render(graphics::GraphicContext& context, const FrameConstants& frameConstants)
        {
            context.setPipelineState(mWhirlpoolHeightMapPSO);
            context.setRootSignature(mWhirlpoolHeightMapSI.getRootSignature(context.getDevice()));

            mWhirlpoolHeightMapSI.setConstants({ frameConstants.totalTime, frameConstants.deltaTime, 0x1deadb0b, 0x1deadb0b }, 0);
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

            mWhirlpoolHeightMapSI.finalize(context);

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

            const float color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
            context.clearColor(mRTV, color);

            graphics::dx12::BufferView vertexBufferView = mWhirlpoolHeightMapRI.geometry.vertexBufferView();
            context.setVertexBuffers(0, &vertexBufferView);

            graphics::dx12::BufferView indexBufferView = mWhirlpoolHeightMapRI.geometry.indexBufferView();
            context.setIndexBuffer(indexBufferView);

            context.setPrimitiveTopology(mWhirlpoolHeightMapRI.primitiveTopology);

            const SubmeshGeometry drawData = mWhirlpoolHeightMapRI.geometry.drawArgs.at("RI_Submesh_Geometry_WhirlpoolHeightMap_Mesh");
            context.drawIndexedInstanced(drawData.indexCount, 1, 0, 0, 0);
        }

        void WhirlpoolHeightMap::updateCamera()
        {
            vec3f pos = mEyePos;
            vec3f target(0.0f);
            vec3f up(0.0f, 1.0f, 0.0f);

            mView = lookAtLH<float>(pos, target, up);
        }

        graphics::dx12::GraphicsPipelineState WhirlpoolHeightMap::createPSO(graphics::dx12::Device& device)
        {
            // Pipeline State
            graphics::dx12::GraphicsPipelineState::Description psoDesc;
            psoDesc.name = "water pso";
            psoDesc.inputLayout = {
                {"POSITION", graphics::DefaultFormats::R32G32B32_FLOAT, 0, graphics::InputClassification::PerVertex},
                {"NORMAL", graphics::DefaultFormats::R32G32B32_FLOAT, 12, graphics::InputClassification::PerVertex},
                {"TANGENT", graphics::DefaultFormats::R32G32B32_FLOAT, 24, graphics::InputClassification::PerVertex},
                {"TEXCOORD", graphics::DefaultFormats::R32G32_FLOAT, 36, graphics::InputClassification::PerVertex},
            };
            psoDesc.rootSignature = &(mWhirlpoolHeightMapSI.getRootSignature(device));

            psoDesc.vertexShader = graphics::GraphicContext::compileShader(
                "WhirlpoolHeightMapVS",
                shaders::WhirlpoolHeightMapShader,
                "vs_5_0",
                "vsMain"
            );
            psoDesc.pixelShader = graphics::GraphicContext::compileShader(
                "WhirlpoolHeightMapPS",
                shaders::WhirlpoolHeightMapShader,
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
