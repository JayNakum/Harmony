#include "WaterPass.h"

#include "GeometryGenerator.h"

#include "Maths.h"

namespace shaders
{
    static const char* GerstnerWaveWaterShader = R"###(
static const uint NUM_WAVES = 3;
static const float PI = 3.14159265358979323826264;
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
    float amplitude;
    float2 windDirection;
    float steepness;
    float wavelength;
    float _waterParamsPadding0;
    float _waterParamsPadding1;
    float _waterParamsPadding2;
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

    float waveHeight : TEXCOORD0;
};

float calculatePhase(float3 vertex, float waveLength, float2 direction, float t)
{
    float k = (2 * PI) / waveLength;
    float angularFreq = sqrt(G * k);
    float phase = (k * (direction.x * vertex.x + direction.y * vertex.z)) - (angularFreq * t);

    return phase;
}

float3 calculateOffsets(float3 vertex, float amplitude, float2 direction, float steepness, float waveLength, float t)
{
    float phase = calculatePhase(vertex, waveLength, direction, t);
    float dx = (steepness * amplitude * direction.x * cos(phase));
    float dy = amplitude * sin(phase);
    float dz = (steepness * amplitude * direction.y * cos(phase));

    return float3(dx, dy, dz);
}

VSOutput vsMain(VSInput input)
{
    VSOutput result;
    float4x4 mvp = mul(gView, gProj);

    float3 waveOffset = float3(0.0f, 0.0f, 0.0f);
    [unroll]
    for(uint i = 0; i < NUM_WAVES; ++i)
    {
        waveOffset += calculateOffsets(
                        input.position,
                        gWaterParams[i].amplitude,
                        gWaterParams[i].windDirection,
                        gWaterParams[i].steepness,
                        gWaterParams[i].wavelength, gTotalTime);
    }

    float3 wave = input.position;
    wave.x -= waveOffset.x;
    wave.y = waveOffset.y;
    wave.z -= waveOffset.z;
    result.position = mul(float4(wave, 1.0f), mvp);
    result.waveHeight = wave.y;
    result.normal = input.normal;
    return result;
}

float4 psMain(VSOutput input) : SV_TARGET
{
    float3 deepColor = float3(0.0f, 0.3f, 0.5f);
    float3 peakColor = float3(1.0f, 1.0f, 1.0f);

    // compute wave min max
    float maxAmplitude = 0.0f;
    for (int i = 0; i < NUM_WAVES; ++i)
    {
        maxAmplitude += gWaterParams[i].amplitude;
    }

    float normalizedHeight = saturate(input.waveHeight / maxAmplitude);

    float3 finalColor = lerp(deepColor, peakColor, normalizedHeight);

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
                    { 0.2f, 0.1f, 0.15f }, // amplitude
                    { vec2f(1,0), vec2f(-0.5f,0.866f), vec2f(-0.5f,-0.866f) }, // direction
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

                const float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
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
                constants.push_back(params.amplitudes[i]);
                constants.push_back(params.windDirections[i].x);
                constants.push_back(params.windDirections[i].y);
                constants.push_back(params.steepnesses[i]);
                constants.push_back(params.wavelengths[i]);
                constants.push_back(params._waterParamsPadding0[i]);
                constants.push_back(params._waterParamsPadding1[i]);
                constants.push_back(params._waterParamsPadding2[i]);
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
