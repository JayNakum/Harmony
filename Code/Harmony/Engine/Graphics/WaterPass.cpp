#include "WaterPass.h"

#include "GeometryGenerator.h"

#include "Maths.h"

namespace shaders
{
    static const char* GerstnerWaveWaterShader = R"###(
static const uint NUM_WAVES = 3;
static const float PI = 3.14159265358979323826264f;
static const float G = 9.81f;

cbuffer cbFrame : register(b0)
{
    float gTotalTime;
    float gDeltaTime;
    uint _framePadding0;
    uint _framePadding1;
};

struct WaterParams
{
    float2 windDirection;
    float2 waveCenter; // RADIAL WAVES
    float amplitude;
    float steepness;
    float wavelength;
    uint _waterParamsPadding0;
};

cbuffer cbWaterParams : register(b1)
{
    WaterParams gWaterParams[NUM_WAVES];
};

cbuffer cbPass : register(b2)
{
    float4x4 gView;
    float4x4 gInvView;

    float3 gEyePosW;
    uint cbPassPadding0;

    float4x4 gProj;
    float4x4 gInvProj;

    float gNearZ;
    float gFarZ;
    uint cbPassPadding1;
    uint cbPassPadding2;

    float4x4 gViewProj;
    float4x4 gInvViewProj;

    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    uint _cbPassPaddingFinal[20];
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

float calculateWavePhase(float3 vertex, float waveLength, float distance, float t)
{
    float k = (2.0f * PI) / waveLength;
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

    float phase = calculateWavePhase(vertex, waveLength, distance, t);
    float dx = (steepness * amplitude * direction.x * cos(phase));
    float dy = amplitude * sin(phase);
    float dz = (steepness * amplitude * direction.y * cos(phase));

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
        float2 direction = gWaterParams[i].windDirection;

        // RADIAL WAVES
        //float2 direction = vertex.xz - center;

        waveOffset += calculateWaveOffsets(
                            vertex,
                            direction,
                            gWaterParams[i].amplitude,
                            gWaterParams[i].steepness,
                            gWaterParams[i].wavelength,
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
    const float2 center = float2(0.0f, 0.0f);
    const float swirlSpeed = 3.0f;
    const float swirlFallOff = 10.0f;

    float2 centeredXZ = vertex.xz - center;
    float r = length(centeredXZ);

    float factor = saturate(1.0f - (r / swirlFallOff));
    float rawAngle = swirlSpeed * gTotalTime;
    float wrapped = fmod(rawAngle, 4.0f * PI);
    float swirlAngle = wrapped * factor;

    float s = sin(swirlAngle);
    float c = cos(swirlAngle);
    float2 v = centeredXZ;
    float2 rotatedXZ = float2(
        v.x * c - v.y * s,
        v.x * s + v.y * c
    );

    return float3(
        center.x + rotatedXZ.x,
        vertex.y - (swirlSpeed * factor), // sink down
        center.y + rotatedXZ.y
    );
}

VSOutput vsMain(VSInput input)
{
    VSOutput result;

    float3 displacedPosition = simulateWaves(input.position);
    float3 finalPosition = simulateWhirlpool(displacedPosition);

    float4x4 mvp = mul(gView, gProj);
    result.position = mul(float4(finalPosition, 1.0f), mvp);

    // compute wave min max
    float maxAmplitude = 0.0f;
    for (int i = 0; i < NUM_WAVES; ++i)
    {
        maxAmplitude += gWaterParams[i].amplitude;
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

                updateCamera();
                updateConstantBuffers(context, passInputs.frame);
            }

            {// Render
                graphics::ShaderInput waterSI(context, mConstantsLayout, mParametersLayout);
                context.setPipelineState(mWaterPSO);

                waterSI.setConstants({passInputs.frame.totalTime, passInputs.frame.deltaTime, 0x1deadb0b, 0x1deadb0b }, 0);

                WaterParams params
                {
                    { vec2f(1.0f,0.0f), vec2f(-0.5f,0.866f), vec2f(-0.5f,-0.866f) }, // direction
                    { vec2f(0.0f,0.0f), vec2f(0.0f,0.0f), vec2f(0.0f,0.0f) }, // center
                    { 0.2f, 0.1f, 0.15f }, // amplitude
                    { 0.80f, 0.60f, 0.40f }, // steepness
                    { 10.0f, 6.0f, 3.0f } // wavelength
                };
                setWaterParams(params, waterSI, 1);
                waterSI.setResource(*mPassCB, 0);
                waterSI.finalize();

                context.setRenderTargets(mRTV);

                graphics::ViewPort viewport{};
                viewport.x = 0;
                viewport.y = 0;
                viewport.width = (float)mRenderTarget->getTexture().describeResource().width;
                viewport.height = (float)mRenderTarget->getTexture().describeResource().height;
                context.setViewport(viewport);

                graphics::Box scissor{};
                scissor.left = (unsigned int)viewport.x;
                scissor.top = (unsigned int)viewport.y;
                scissor.right = (unsigned int)viewport.x + (unsigned int)viewport.width;
                scissor.bottom = (unsigned int)viewport.y + (unsigned int)viewport.height;
                context.setScissor(scissor);

                const float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
                context.clearColor(mRTV, clearColor);

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

            delete mPassCB;

            delete mRenderTarget;
        }

        void Water::setWaterParams(const WaterParams& params, graphics::ShaderInput& waterSI, const unsigned int constantsLayoutIndex) const
        {
            graphics::ShaderInput::RootConstants constants;

            for (unsigned int i = 0; i < WaterParams::NUM_WAVES; ++i)
            {
                constants.push_back(params.directions[i].x);
                constants.push_back(params.directions[i].y);
                constants.push_back(params.centers[i].x);
                constants.push_back(params.centers[i].y);
                constants.push_back(params.amplitudes[i]);
                constants.push_back(params.steepnesses[i]);
                constants.push_back(params.wavelengths[i]);
                constants.push_back(params._waterParamsPadding0[i]);
            }
            waterSI.setConstants(constants, constantsLayoutIndex);
        }

        graphics::dx12::GraphicsPipelineState Water::initialize(graphics::GraphicContext& context)
        {
            const graphics::ShaderInput::Constants slot1
            {
                graphics::ShaderVisibility::Vertex | graphics::ShaderVisibility::Pixel,
                4
            };
            const graphics::ShaderInput::Constants slot2
            {
                graphics::ShaderVisibility::Vertex | graphics::ShaderVisibility::Pixel,
                28
            };
            mConstantsLayout.emplace_back(slot1);
            mConstantsLayout.emplace_back(slot2);

            const graphics::ShaderInput::Parameter slot3{
                graphics::ShaderVisibility::Vertex,
                {{graphics::DescriptorRangeType::CBV, 1}}
            };
            mParametersLayout.emplace_back(slot3);
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
            graphics::dx12::UploadBuffer* vertexBufferUploader = new graphics::dx12::UploadBuffer(context.getDevice(), { "water vb uploader", vbByteSize });
            {
                void* intermediateDst = vertexBufferUploader->map();
                memcpy(intermediateDst, grid.vertices.data(), vbByteSize);
                vertexBufferUploader->unmap();
            }

            graphics::dx12::GPUResource::Description createVBDesc;
            createVBDesc.name = "water vertex buffer";
            createVBDesc.width = vbByteSize;
            createVBDesc.dimension = graphics::GPUResourceDimension::Buffer;
            graphics::dx12::ByteAddressBuffer* vertexBuffer = new graphics::dx12::ByteAddressBuffer(createVBDesc);
            vertexBuffer->createCommitted(context.getDevice());

            context.transitionResource(*vertexBufferUploader, graphics::GPUResourceStates::CopySource, true);
            context.transitionResource(*vertexBuffer, graphics::GPUResourceStates::CopyDestination, true);
            context.copyBufferRegion(*vertexBuffer, 0, *vertexBufferUploader, 0, vbByteSize);
            context.transitionResource(*vertexBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer, true);


            // Index Buffer
            const size_t ibByteSize = grid.getIndices16().size() * sizeof(uint16_t);

            graphics::dx12::UploadBuffer* indexBufferUploader = new graphics::dx12::UploadBuffer(context.getDevice(), { "vater ib uploader", ibByteSize });
            {
                void* intermediateDst = indexBufferUploader->map();
                memcpy(intermediateDst, grid.getIndices16().data(), ibByteSize);
                indexBufferUploader->unmap();
            }

            graphics::dx12::GPUResource::Description createIBDesc;
            createIBDesc.name = "water index buffer";
            createIBDesc.width = ibByteSize;
            createIBDesc.dimension = graphics::GPUResourceDimension::Buffer;
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

        void Water::updateCamera()
        {
            vec3f pos = mEyePos;
            vec3f target(0.0f);
            vec3f up(0.0f, 1.0f, 0.0f);

            mView = lookAtLH<float>(pos, target, up);
        }

        void Water::updateConstantBuffers(graphics::GraphicContext& context, const FrameConstants& frame)
        {
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

            mPassCB = new graphics::dx12::UploadBuffer(context.getDevice(), { "water pass cbuffer", sizeof(PassConstants) });
            {
                void* bufferDest = mPassCB->map();
                memcpy(bufferDest, &mPass, sizeof(PassConstants));
                mPassCB->unmap();
            }
        }
    }
}
