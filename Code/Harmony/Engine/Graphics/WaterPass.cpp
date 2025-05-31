#include "WaterPass.h"

#include "GeometryGenerator.h"

#include "Maths.h"

namespace shaders
{
    static const char* WaterVS = R"###(
cbuffer cbPass : register(b0)
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
};

VSOutput main(VSInput input)
{
    VSOutput result;
    float4x4 mvp = mul(gView, gProj);

    result.position = mul(float4(input.position, 1.0f), mvp);
    return result;
}
)###";

    static const char* WaterPS = R"###(
struct VSOutput
{
    float4 position : SV_POSITION;
};
float4 main(VSOutput input) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
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

                    float fov = 0.25f * M_PI;
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
                graphics::ShaderInput waterSI(context, {/*no constants*/}, mParametersLayout);
                waterSI.setResource(*mPassCB, 0);
                waterSI.finalize();

                context.setPipelineState(mWaterPSO);

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

                const float clearColor[4] = { 0.118f, 0.106f, 0.180f, 1.0f };
                //const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
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

        graphics::dx12::GraphicsPipelineState Water::initialize(graphics::GraphicContext& context)
        {
            const graphics::ShaderInput::Parameter slot1{
                graphics::ShaderVisibility::Vertex,
                {{graphics::DescriptorRangeType::CBV, 1}}
            };
            mParametersLayout.emplace_back(slot1);
            graphics::ShaderInput waterSI(context, {/*no constants*/}, mParametersLayout);

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
                shaders::WaterVS,
                "vs_5_0"
            );
            psoDesc.pixelShader = graphics::GraphicContext::compileShader(
                "WaterPS",
                shaders::WaterPS,
                "ps_5_0"
            );
            psoDesc.numRenderTargets = 1;
            psoDesc.rtvFormats[0] = graphics::DefaultFormats::R16G16B16A16_UNORM;
            psoDesc.cullMode = graphics::CullMode::None;
            return graphics::dx12::GraphicsPipelineState(context.getDevice(), psoDesc);
        }

        void Water::prepareRenderItem(graphics::GraphicContext& context)
        {
            GeometryGenerator geoGen;
            GeometryGenerator::MeshData grid = geoGen.createGrid(20.0f, 30.0f, 60, 40);

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
            mPass.renderTargetSize.x = mRTDescription.textureDescription.width;
            mPass.renderTargetSize.y = mRTDescription.textureDescription.height;
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
