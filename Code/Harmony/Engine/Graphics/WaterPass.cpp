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

cbuffer cbWaterParams : register(b2)
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
        Water::Water(graphics::GraphicContext& context)
            : mRTDescription{
                graphics::dx12::RenderTarget::Type::Color,
                {
                    "water render target",
                    0, 0,
                    1, 1,
                    graphics::GPUResourceDimension::Texture2D,
                    graphics::DefaultFormats::R16G16B16A16_UNORM,
                    graphics::GPUResourceFlags::AllowRenderTarget,
                }
            },
            mRenderTarget(nullptr),
            mWaterPSO(initialize(context)),
            mPassCB(nullptr),
            mWaterParamsCB(nullptr),
            mWaterRI(nullptr)
        {
            prepareRenderItem(context);
        }

        void Water::freeUploadResources()
        {
            mWaterRI->geometry.disposeUploaders();
        }

        Water::Outputs Water::render(graphics::GraphicContext& context, Inputs& passInputs)
        {
            {// Pre Render
                if (mRTDescription.textureDescription.width != passInputs.width ||
                    mRTDescription.textureDescription.height != passInputs.height)
                {
                    mRTDescription.textureDescription.width = passInputs.width;
                    mRTDescription.textureDescription.height = passInputs.height;

                    if (mRenderTarget != nullptr)
                    {
                        delete mRenderTarget;
                    }
                    mRenderTarget = new graphics::dx12::RenderTarget(mRTDescription);
                    mRenderTarget->getTexture().createCommitted(context.getDevice());

                    mRTV = context.allocateDescriptors(graphics::DescriptorHeapType::RenderTarget);
                    mRenderTarget->createRenderTargetView(context.getDevice(), mRTV);

                    float fov = 0.25f * (float)M_PI;
                    float aspectRatio = (float)passInputs.width / (float)passInputs.height;
                    mPass.nearZ = 0.01f;
                    mPass.farZ = 1000.0f;
                    mProjection = perspectiveProjection(fov, aspectRatio, mPass.nearZ, mPass.farZ);
                }

                context.transitionResource(mRenderTarget->getTexture(), graphics::GPUResourceStates::RenderTarget, true);
            }

            {// Render
                graphics::ShaderInput waterSI(context, mConstantsLayout, mParametersLayout);
                context.setPipelineState(mWaterPSO);

                updateCamera();
                setCommonParams(context, waterSI, passInputs.frame);
                setWaterParams(context, waterSI, passInputs.frame, 1);

                waterSI.finalize();

                context.setRenderTargets(mRTV);

                graphics::ViewPort viewport{};
                viewport.x = 0;
                viewport.y = 0;
                viewport.width = (float)mRenderTarget->getTexture().getDescription().width;
                viewport.height = (float)mRenderTarget->getTexture().getDescription().height;
                context.setViewport(viewport);

                graphics::Box scissor{};
                scissor.left = (unsigned int)viewport.x;
                scissor.top = (unsigned int)viewport.y;
                scissor.right = (unsigned int)viewport.x + (unsigned int)viewport.width;
                scissor.bottom = (unsigned int)viewport.y + (unsigned int)viewport.height;
                context.setScissor(scissor);

                const float color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
                context.clearColor(mRTV, color);

                graphics::dx12::BufferView vertexBufferView = mWaterRI->geometry.vertexBufferView();
                context.setVertexBuffers(0, &vertexBufferView);

                graphics::dx12::BufferView indexBufferView = mWaterRI->geometry.indexBufferView();
                context.setIndexBuffer(indexBufferView);

                context.setPrimitiveTopology(mWaterRI->primitiveTopology);

                const SubmeshGeometry drawData = mWaterRI->geometry.drawArgs.at("water submesh");
                context.drawIndexedInstanced(drawData.indexCount, 1, 0, 0, 0);
            }

            {// Post Render
                return Outputs{ mRenderTarget->getTexture() };
            }
        }

        Water::~Water()
        {
            delete mWaterRI->geometry.vertexBuffer;
            delete mWaterRI->geometry.indexBuffer;
            delete mWaterRI;

            delete mWaterParamsCB;
            delete mPassCB;

            delete mRenderTarget;
        }

        void Water::updateCamera()
        {
            vec3f pos = mEyePos;
            vec3f target(0.0f);
            vec3f up(0.0f, 1.0f, 0.0f);

            mView = lookAtLH<float>(pos, target, up);
        }

        void Water::setCommonParams(graphics::GraphicContext& context, graphics::ShaderInput& waterSI, const FrameConstants& frame)
        {
            waterSI.setConstants({ frame.totalTime, frame.deltaTime, 0x1deadb0b, 0x1deadb0b }, 0);

            if (mPassCB)
            {
                delete mPassCB;
            }

            mPass.view = transpose(mView);
            mPass.invView = inverse(mView);
            mPass.eyePosition = mEyePos;
            mPass.projection = transpose(mProjection);
            mPass.invProjection = inverse(mProjection);
            mPass.viewProjection = mView * mProjection;
            mPass.invViewProjection = inverse(mPass.viewProjection);
            mPass.renderTargetSize.x = (float)mRTDescription.textureDescription.width;
            mPass.renderTargetSize.y = (float)mRTDescription.textureDescription.height;
            mPass.invRenderTargetSize = 1 / mPass.renderTargetSize;

            graphics::dx12::GPUResource::Description createCBDesc { "water pass cbuffer", sizeof(PassConstants) };
            createCBDesc.dimension = graphics::GPUResourceDimension::Buffer;
            mPassCB = new graphics::dx12::UploadBuffer(context.getDevice(), createCBDesc);
            mPassCB->createCommitted(context.getDevice(), graphics::HeapType::Upload);
            {
                void* bufferDest = mPassCB->map();
                memcpy(bufferDest, &mPass, sizeof(PassConstants));
                mPassCB->unmap();
            }

            waterSI.setResource(*mPassCB, 0);
        }

        void Water::setWaterParams(graphics::GraphicContext& context, graphics::ShaderInput& waterSI, const FrameConstants& frame, const unsigned int paramsLayoutIndex)
        {
            if (mWaterParamsCB)
            {
                delete mWaterParamsCB;
            }

            WaterParams waterParams;
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
            }
            {
                const float swirlSpeed = 5.0f;

                mCurrentSwirlAngle += swirlSpeed * frame.deltaTime;

                if (mCurrentSwirlAngle > 40.0f)
                {
                    mCurrentSwirlAngle = std::fmod(mCurrentSwirlAngle, (M_PI));
                }

                waterParams.whirlpoolParams[0].center = vec2f(0.0f, 0.0f);
                waterParams.whirlpoolParams[0].swirlRadius = 10.0f;
                waterParams.whirlpoolParams[0].wrappedSwirlAngle = (float)mCurrentSwirlAngle;
                waterParams.whirlpoolParams[0].sinkRadius = 8.0f;
                waterParams.whirlpoolParams[0].sinkDepth = 2.0f;
            }
            {
                float maxAmplitude = 0.0f;
                for (int i = 0; i < WaterParams::NUM_WAVES; ++i)
                {
                    maxAmplitude += waterParams.waveParams[i].amplitude;
                }
                waterParams.wavesMaxHeight = maxAmplitude;
            }

            graphics::dx12::GPUResource::Description createCBDesc{ "water params cbuffer", sizeof(WaterParams) };
            createCBDesc.dimension = graphics::GPUResourceDimension::Buffer;
            mWaterParamsCB = new graphics::dx12::UploadBuffer(context.getDevice(), createCBDesc);
            mWaterParamsCB->createCommitted(context.getDevice(), graphics::HeapType::Upload);
            {
                void* bufferDest = mWaterParamsCB->map();
                memcpy(bufferDest, &waterParams, sizeof(WaterParams));
                mWaterParamsCB->unmap();
            }

            waterSI.setResource(*mWaterParamsCB, paramsLayoutIndex);
        }

        graphics::dx12::GraphicsPipelineState Water::initialize(graphics::GraphicContext& context)
        {
            const graphics::ShaderInput::Constants frameConstants
            {
                graphics::ShaderVisibility::Vertex,
                4
            };
            mConstantsLayout.emplace_back(frameConstants); // slot 0

            const graphics::ShaderInput::Parameter passConstants{
                graphics::ShaderVisibility::Vertex,
                {{graphics::DescriptorRangeType::CBV, 1}}
            };
            const graphics::ShaderInput::Parameter waterParams{
                graphics::ShaderVisibility::Vertex,
                {{graphics::DescriptorRangeType::CBV, 1}}
            };
            mParametersLayout.emplace_back(passConstants); // slot 1
            mParametersLayout.emplace_back(waterParams); // slot 2

            graphics::ShaderInput waterSI(context, mConstantsLayout, mParametersLayout);

            // Pipeline State
            graphics::dx12::GraphicsPipelineState::Description psoDesc;
            psoDesc.name = "water pso";
            psoDesc.inputLayout = {
                {"POSITION", graphics::DefaultFormats::R32G32B32_FLOAT, 0, graphics::InputClassification::PerVertex},
                {"NORMAL", graphics::DefaultFormats::R32G32B32_FLOAT, 12, graphics::InputClassification::PerVertex},
                {"TANGENT", graphics::DefaultFormats::R32G32B32_FLOAT, 24, graphics::InputClassification::PerVertex},
                {"TEXCOORD", graphics::DefaultFormats::R32G32_FLOAT, 36, graphics::InputClassification::PerVertex},
            };
            psoDesc.rootSignature = &(waterSI.getRootSignature());

            psoDesc.vertexShader = graphics::GraphicContext::compileShader(
                "WaterVS",
                shaders::GerstnerWaveWaterShader,
                "vs_5_0",
                "vsMain"
            );
            psoDesc.pixelShader = graphics::GraphicContext::compileShader(
                "WaterPS",
                shaders::GerstnerWaveWaterShader,
                "ps_5_0",
                "psMain"
            );
            psoDesc.numRenderTargets = 1;
            psoDesc.rtvFormats[0] = graphics::DefaultFormats::R16G16B16A16_UNORM;
            psoDesc.cullMode = graphics::CullMode::None;
            return graphics::dx12::GraphicsPipelineState(context.getDevice(), psoDesc);
        }

        void Water::prepareRenderItem(graphics::GraphicContext& context)
        {
            GeometryGenerator geoGen;
            GeometryGenerator::MeshData grid = geoGen.createGrid(30.0f, 30.0f, 256, 256);

            SubmeshGeometry waterSubmesh;
            waterSubmesh.indexCount = (unsigned int)grid.getIndices16().size();

            // Vertex Buffer
            const size_t vbByteSize = grid.vertices.size() * sizeof(GeometryGenerator::Vertex);
            graphics::dx12::GPUResource::Description createVBDesc;
            createVBDesc.name = "water vertex buffer uploader";
            createVBDesc.width = vbByteSize;
            createVBDesc.dimension = graphics::GPUResourceDimension::Buffer;
            graphics::dx12::UploadBuffer* vertexBufferUploader = new graphics::dx12::UploadBuffer(context.getDevice(), createVBDesc);
            vertexBufferUploader->createCommitted(context.getDevice(), graphics::HeapType::Upload);
            {
                void* intermediateDst = vertexBufferUploader->map();
                memcpy(intermediateDst, grid.vertices.data(), vbByteSize);
                vertexBufferUploader->unmap();
            }

            createVBDesc.name = "water vertex buffer";
            graphics::dx12::ByteAddressBuffer* vertexBuffer = new graphics::dx12::ByteAddressBuffer(createVBDesc);
            vertexBuffer->createCommitted(context.getDevice());

            context.transitionResource(*vertexBufferUploader, graphics::GPUResourceStates::CopySource, true);
            context.transitionResource(*vertexBuffer, graphics::GPUResourceStates::CopyDestination, true);
            context.copyBufferRegion(*vertexBuffer, 0, *vertexBufferUploader, 0, vbByteSize);
            context.transitionResource(*vertexBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer, true);

            // Index Buffer
            const size_t ibByteSize = grid.getIndices16().size() * sizeof(uint16_t);
            graphics::dx12::GPUResource::Description createIBDesc;
            createIBDesc.name = "water index buffer uploader";
            createIBDesc.width = ibByteSize;
            createIBDesc.dimension = graphics::GPUResourceDimension::Buffer;

            graphics::dx12::UploadBuffer* indexBufferUploader = new graphics::dx12::UploadBuffer(context.getDevice(), createIBDesc);
            indexBufferUploader->createCommitted(context.getDevice(), graphics::HeapType::Upload);
            {
                void* intermediateDst = indexBufferUploader->map();
                memcpy(intermediateDst, grid.getIndices16().data(), ibByteSize);
                indexBufferUploader->unmap();
            }

            createIBDesc.name = "water index buffer";
            graphics::dx12::ByteAddressBuffer* indexBuffer = new graphics::dx12::ByteAddressBuffer(createIBDesc);
            indexBuffer->createCommitted(context.getDevice());

            context.transitionResource(*indexBufferUploader, graphics::GPUResourceStates::CopySource, true);
            context.transitionResource(*indexBuffer, graphics::GPUResourceStates::CopyDestination, true);
            context.copyBufferRegion(*indexBuffer, 0, *indexBufferUploader, 0, ibByteSize);
            context.transitionResource(*indexBuffer, graphics::GPUResourceStates::IndexBuffer, true);

            // Water Mesh Geometry
            MeshGeometry geo;
            geo.name = "water mesh geometry";
            geo.vertexBufferUploader = vertexBufferUploader;
            geo.vertexBuffer = vertexBuffer;
            geo.indexBufferUploader = indexBufferUploader;
            geo.indexBuffer = indexBuffer;
            geo.vertexByteStride = sizeof(GeometryGenerator::Vertex);
            geo.drawArgs["water submesh"] = waterSubmesh;

            mWaterRI = new RenderItem(geo);
        }
    }
}
