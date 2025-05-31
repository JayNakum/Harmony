#include "WaterPass.h"

#include "ShaderInput.h"

namespace shaders
{
    static const char* WaterVS = R"###(
cbuffer cbFrame : register(b0)
{
    float totalTime;
    float deltaTime;
    uint cbFramePadding0;
    uint cbFramePadding1;
};

cbuffer cbPass : register(b1)
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
};

cbuffer cbObject : register(b2)
{
    float4x4 gWorld;
};

struct VSOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VSOut main(float4 position : POSITION, float4 color : COLOR)
{
    VSOut result;
    result.position = position;
    result.color = color;
    return result;
}
)###";

    static const char* WaterPS = R"###(
struct VSOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};
float4 main(VSOut input) : SV_TARGET
{
    return input.color;
}
)###";
}

namespace engine
{
    namespace renderPass
    {
        struct Vertex
        {
            float position[4];
            float color[4];
        };

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
            mWaterPSO(initialize(context))
        {
            Vertex triangleVertices[] =
            {
                { { 0.0f, 0.25f, 0.0f, 1.0f }, { 0.835f, 0.518f, 0.988f, 1.0f } },
                { { 0.25f, -0.25f, 0.0f, 1.0f }, { 0.694f, 0.392f, 0.804f, 1.0f } },
                { { -0.25f, -0.25f, 0.0f, 1.0f }, { 0.796f, 0.643f, 0.914f, 1.0f } }
            };

            size_t numBytes = sizeof(triangleVertices);

            mIntermediateVertexBuffer = new graphics::dx12::UploadBuffer(context.getDevice(), { "intermediate buffer", numBytes });
            {
                void* intermediateDst = mIntermediateVertexBuffer->map();
                memcpy(intermediateDst, triangleVertices, numBytes);
                mIntermediateVertexBuffer->unmap();
            }
            graphics::dx12::GPUResource::Description createDesc;
            createDesc.name = "water vertex buffer";
            createDesc.width = numBytes;
            createDesc.dimension = graphics::GPUResourceDimension::Buffer;
            mVertexBuffer = new graphics::dx12::ByteAddressBuffer(createDesc);

            mVertexBuffer->createCommitted(context.getDevice());
            context.transitionResource(*mIntermediateVertexBuffer, graphics::GPUResourceStates::CopySource, true);
            context.transitionResource(*mVertexBuffer, graphics::GPUResourceStates::CopyDestination, true);

            context.copyBufferRegion(*mVertexBuffer, 0, *mIntermediateVertexBuffer, 0, numBytes);

            context.transitionResource(*mVertexBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer, true);
        }

        void Water::freeUploadResources()
        {
            delete mIntermediateVertexBuffer;
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
                }

                context.transitionResource(mRenderTarget->getTexture(), graphics::GPUResourceStates::RenderTarget, true);
            }

            {// Render
                const graphics::ShaderInput::Parameter slot1{
                    graphics::ShaderVisibility::Vertex | graphics::ShaderVisibility::Pixel,
                    {{graphics::DescriptorRangeType::CBV, 1}}
                };
                const graphics::ShaderInput::Parameter slot2{
                    graphics::ShaderVisibility::Vertex,
                    {{graphics::DescriptorRangeType::CBV, 1}}
                };
                const graphics::ShaderInput::Parameter slot3{
                    graphics::ShaderVisibility::Vertex,
                    {{graphics::DescriptorRangeType::CBV, 1}}
                };
                const std::vector<graphics::ShaderInput::Parameter> parametersLayout = {slot1, slot2, slot3};

                graphics::ShaderInput waterSI(context, {/*no constants*/}, parametersLayout);
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

                context.setPrimitiveTopology(graphics::PrimitiveTopology::TriangleList);

                graphics::dx12::BufferView vertexBufferView = mVertexBuffer->vertexBufferView(sizeof(Vertex));

                context.setVertexBuffers(0, &vertexBufferView);
                context.drawInstanced(3, 1);
            }

            {// Post Render
                return Outputs{ mRenderTarget->getTexture() };
            }
        }

        Water::~Water()
        {
            delete mVertexBuffer;
            delete mRenderTarget;
        }

        graphics::dx12::GraphicsPipelineState Water::initialize(graphics::GraphicContext& context)
        {
            const graphics::ShaderInput::Parameter slot1{
                graphics::ShaderVisibility::Vertex | graphics::ShaderVisibility::Pixel,
                {{graphics::DescriptorRangeType::CBV, 1}}
            };
            const graphics::ShaderInput::Parameter slot2{
                graphics::ShaderVisibility::Vertex,
                {{graphics::DescriptorRangeType::CBV, 1}}
            };
            const graphics::ShaderInput::Parameter slot3{
                graphics::ShaderVisibility::Vertex,
                {{graphics::DescriptorRangeType::CBV, 1}}
            };
            const std::vector<graphics::ShaderInput::Parameter> parametersLayout = { slot1, slot2, slot3 };
            graphics::ShaderInput waterSI(context, {/*no constants*/}, parametersLayout);

            // Pipeline State
            graphics::dx12::GraphicsPipelineState::Description psoDesc;
            psoDesc.name = "water pso";
            psoDesc.inputLayout = {
                {"POSITION", graphics::DefaultFormats::R32G32B32A32_FLOAT, 0, graphics::InputClassification::PerVertex},
                {"COLOR", graphics::DefaultFormats::R32G32B32A32_FLOAT, 16, graphics::InputClassification::PerVertex},
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
    }
}
