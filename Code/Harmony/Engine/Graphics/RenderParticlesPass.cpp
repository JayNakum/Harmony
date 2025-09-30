#include "RenderParticlesPass.h"

namespace shaders
{
    static const char* InstancedBillboardParticlesShader = R"###(

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

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 textureCoord : TEXCOORD0;
};

VSOutput vsMain(
    uint vertexID : SV_VertexID,
    uint instanceID : SV_InstanceID
)
{
    uint x_bit = (vertexID >> 1) & 1;
    uint y_bit = vertexID & 1;

    float2 quadPos = float2(
        x_bit ? 0.5f : -0.5f,
        y_bit ? 0.5f : -0.5f
    );

    const float particleSize = 0.1f;
    float2 scaledQuadPos = quadPos * particleSize;

    float3 cameraRight = gView._11_21_31;
    float3 cameraUp = gView._12_22_32;

    uint gridSize = 100;
    uint gridX = instanceID % gridSize;
    uint gridY = instanceID / gridSize;

    // Position particles in a grid with spacing
    const float spacing = 2.0f; // Adjust this to change grid density
    float3 particlePos = float3(
        (gridX - gridSize * 0.5f + 0.5f) * spacing,
        0.0f,
        (gridY - gridSize * 0.5f + 0.5f) * spacing
    );

    // Billboarding
    float3 worldPos = particlePos + scaledQuadPos.x * cameraRight + scaledQuadPos.y * cameraUp;

    VSOutput output;

    output.position = mul(float4(worldPos, 1.0f), gViewProj);
    output.textureCoord = float2(x_bit, 1.0f - y_bit);

    return output;
}

float4 psMain(VSOutput input) : SV_TARGET
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}

)###";
}

namespace engine
{
    RenderParticlesPassData::RenderParticlesPassData()
    {
        reset();
    }

    void RenderParticlesPassData::debugUIRender()
    {
        ImGui::Begin("Render Particles Pass Data");

        debugUICommonParams();

        ImGui::End();
    }

    void RenderParticlesPassData::reset()
    {
        setEyePosition({ 5.0f, 10.0f, -25.0f });

        setProjectionMatrix(mat4f(), 0.01f, 1000.0f);

        setRenderTargetSize(4096, 2160);
    }
}

namespace engine
{
    RenderParticlesPass::RenderParticlesPass(graphics::Device& device, ResourceManager& resourceManager)
        : mRenderParticlesPassData(),
        mStaticShaderResourceTableLayout{
            graphics::ShaderVisibility::Vertex,
            {
                {graphics::DescriptorRangeType::CBV, 1}, // Pass Params Buffer
                // {graphics::DescriptorRangeType::SRV, 1}, // Instance Data Buffer
            }
        },
        mShaderResources(resourceManager.createResourceTable("RenderParticlesShader_ResourceTable", mStaticShaderResourceTableLayout, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::StaticGPU)),
        mRenderParticlesPassSI(
            { {graphics::ShaderVisibility::Vertex, 4} }, // Frame Constants
            { mStaticShaderResourceTableLayout }
        ),
        mRenderParticlesPassPSO(createPSO(device)),
        mRTV(),
        mRenderTargetDescription{
            graphics::HeapWithAllocator::ResourceCategory::RenderTargetData,
            {
                "RT_Particles_RenderPass",
                (size_t)mRenderParticlesPassData.getPassConstants().renderTargetSize.x,
                (size_t)mRenderParticlesPassData.getPassConstants().renderTargetSize.y,
                1, 1,
                graphics::GPUResourceDimension::Texture2D,
                graphics::DefaultFormats::R16G16B16A16_UNORM,
                graphics::GPUResourceFlags::AllowRenderTarget,
            }
        },
        mRenderParticlesRI()
    {
        // RenderTargets
        resourceManager.createRenderTarget(mRenderTargetDescription);
        mRTV = resourceManager.getRenderTargetRTV("RT_Particles_RenderPass");

        // ConstantBuffers
        graphics::HeapWithAllocator::ResourceDescription commonPassConstantsBuffer{
            graphics::HeapWithAllocator::ResourceCategory::UploadPersistentData,
            { "CB_CommonPassParams_Particles_RenderPass", maths::alignUp(sizeof(PassConstants), 256) }
        };
        resourceManager.createBuffer(commonPassConstantsBuffer);
        mCommonPassConstantsBuffer = &resourceManager.getBuffer("CB_CommonPassParams_Particles_RenderPass");

        mRenderParticlesPassSI.setResourceTable(mShaderResources);
    }

    void RenderParticlesPass::resize(const unsigned int width, const unsigned int height, graphics::Device& device, ResourceManager& resourceManager)
    {
        mRenderParticlesPassData.setRenderTargetSize(width, height);
        mRenderTargetDescription.resource.width = width;
        mRenderTargetDescription.resource.height = height;

        resourceManager.destroyDescriptor("RT_Particles_RenderPass");
        resourceManager.releaseRenderTarget("RT_Particles_RenderPass", mRenderTargetDescription.category);
        resourceManager.createRenderTarget(mRenderTargetDescription);
        mRTV = resourceManager.getRenderTargetRTV("RT_Particles_RenderPass");

        const float fov = 0.25f * (float)M_PI;
        const float aspectRatio = (float)width / (float)height;
        const float nearZ = mRenderParticlesPassData.getPassConstants().nearZ;
        const float farZ = mRenderParticlesPassData.getPassConstants().farZ;
        const mat4f newProjectionMatrix = perspectiveProjection(fov, aspectRatio, nearZ, farZ);
        mRenderParticlesPassData.setProjectionMatrix(newProjectionMatrix, nearZ, farZ);

        mShaderResources.setResource(device, resourceManager.getBufferCBV("CB_CommonPassParams_Particles_RenderPass"));
    }

    void RenderParticlesPass::update(const double timeStep)
    {
        const vec3f pos = mRenderParticlesPassData.getPassConstants().eyePosition;
        const vec3f target(0.0f);
        const vec3f up(0.0f, 1.0f, 0.0f);

        mRenderParticlesPassData.setViewMatrix(lookAtLH<float>(pos, target, up));
    }

    void RenderParticlesPass::getInputOutputs(graphics::GraphicContext& context, ResourceManager& resourceManager)
    {
        graphics::RenderTarget& renderTarget = resourceManager.getRenderTarget("RT_Particles_RenderPass");
        context.transitionBarrier(renderTarget.getTexture(), graphics::GPUResourceStates::RenderTarget);

        graphics::Buffer& commonPassConstantsBuffer = resourceManager.getBuffer("CB_CommonPassParams_Particles_RenderPass");
        context.transitionBarrier(commonPassConstantsBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer);
    }

    void RenderParticlesPass::render(graphics::GraphicContext& context, const FrameConstants& frameConstants)
    {
        context.setPipelineState(mRenderParticlesPassPSO);
        context.setRootSignature(mRenderParticlesPassSI.getRootSignature(context.getDevice()));

        mRenderParticlesPassSI.setConstants({ frameConstants.totalTime, frameConstants.deltaTime, 0x1deadb0b, 0x1deadb0b }, 0);
        void* commonPassConstants = mCommonPassConstantsBuffer->map(context.getDevice());
        const PassConstants& passConstants = mRenderParticlesPassData.getPassConstants();
        memcpy(commonPassConstants, &passConstants, sizeof(PassConstants));
        mCommonPassConstantsBuffer->unmap();

        mRenderParticlesPassSI.finalize(context);

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

        context.setPrimitiveTopology(graphics::PrimitiveTopology::TriangleStrip);
        context.drawInstanced(4, 10000);
    }

    graphics::GraphicsPipelineState RenderParticlesPass::createPSO(graphics::Device& device)
    {
        graphics::GraphicsPipelineState::Description psoDesc;
        psoDesc.name = "Graphics_PSO_Particles";
        psoDesc.rootSignature = &(mRenderParticlesPassSI.getRootSignature(device));
        psoDesc.vertexShader = graphics::CommandContext::compileShader(
            "RenderParticlesVS",
            shaders::InstancedBillboardParticlesShader,
            "vs_5_0",
            "vsMain"
        );
        psoDesc.pixelShader = graphics::CommandContext::compileShader(
            "RenderParticlesPS",
            shaders::InstancedBillboardParticlesShader,
            "ps_5_0",
            "psMain"
        );
        psoDesc.numRenderTargets = 1;
        psoDesc.rtvFormats[0] = graphics::DefaultFormats::R16G16B16A16_UNORM;

        psoDesc.cullMode = graphics::CullMode::None;

        return graphics::GraphicsPipelineState(device, psoDesc);
    }
}
