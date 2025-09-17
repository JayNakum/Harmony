#include "PresentPass.h"

namespace shaders
{
    static const char* ScreenQuadVS = R"###(
cbuffer cbFrame : register(b0)
{
    float gTotalTime;
    float gDeltaTime;
    uint _framePadding0;
    uint _framePadding1;
};

void main(
    in uint vertexID : SV_VertexID,
    out float4 position : SV_Position,
    out float2 textureCoord : TexCoord0
)
{
    // Texture coordinates range [0, 2], but only [0, 1] appears on screen.
    textureCoord = float2(uint2(vertexID, vertexID << 1) & 2);
    position = float4(lerp(float2(-1, 1), float2(1, -1), textureCoord), 0, 1);
}
)###";

    static const char* BufferCopyPS = R"###(
Texture2D colorTex : register(t0);

float4 main(float4 position : SV_Position) : SV_Target0
{
    return colorTex[(int2)position.xy];
}
)###";
}

namespace engine
{
    PresentPass::PresentPass(graphics::Device& device, ResourceManager& resourceManager)
        :
        mStaticShaderResourceTableLayout{
            graphics::ShaderVisibility::Pixel,
            { {graphics::DescriptorRangeType::SRV, 1} }
        },
        mShaderResources(resourceManager.createResourceTable("BufferCopyPS_ResourceTable", mStaticShaderResourceTableLayout, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::StaticGPU)),
        mPresentSI(
            { {graphics::ShaderVisibility::Vertex, 4} },
            { mStaticShaderResourceTableLayout }
        ),
        mPresentPSO(createPSO(device)),
        mSwapchainWidth(0), mSwapchainHeight(0)
    {
        mPresentSI.setResourceTable(mShaderResources);
    }

    void PresentPass::copyDataToGPU(graphics::GraphicContext& context, ResourceManager& resourceManager)
    {
    }

    void PresentPass::resize(const unsigned int width, const unsigned int height, graphics::Device& device, ResourceManager& resourceManager, const bool isHDRSupported)
    {
        mSwapchainWidth = width;
        mSwapchainHeight = height;

        resourceManager.destroyDescriptor("SR_Water_RenderPass");
        mShaderResources.setResource(device, resourceManager.getTextureSRV("RT_Water_RenderPass", "SR_Water_RenderPass"));
    }

    void PresentPass::getInputOutputs(graphics::GraphicContext& context, ResourceManager& resourceManager)
    {
        context.transitionBarrier(resourceManager.getRenderTarget("RT_Water_RenderPass").getTexture(), graphics::GPUResourceStates::PixelShaderResource);

        context.flushResourceBarriers();
    }

    void PresentPass::render(graphics::GraphicContext& context, const FrameConstants& frameConstants, graphics::DescriptorHandle swapchainRTV)
    {
        context.setPipelineState(mPresentPSO);
        context.setRootSignature(mPresentSI.getRootSignature(context.getDevice()));

        mPresentSI.setConstants({ frameConstants.totalTime, frameConstants.deltaTime, 0x1deadb0b, 0x1deadb0b }, 0);
        mPresentSI.finalize(context);

        context.setRenderTargets(swapchainRTV);
        graphics::ViewPort viewport{};
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = (float)mSwapchainWidth;
        viewport.height = (float)mSwapchainHeight;
        context.setViewport(viewport);

        graphics::Box scissor{};
        scissor.left = (unsigned int)viewport.x;
        scissor.top = (unsigned int)viewport.y;
        scissor.right = (unsigned int)viewport.x + (unsigned int)viewport.width;
        scissor.bottom = (unsigned int)viewport.y + (unsigned int)viewport.height;
        context.setScissor(scissor);

        const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        context.clearColor(swapchainRTV, clearColor);
        context.setPrimitiveTopology(graphics::PrimitiveTopology::TriangleList);
        context.draw(3);
    }

    graphics::GraphicsPipelineState PresentPass::createPSO(graphics::Device& device)
    {
        graphics::GraphicsPipelineState::Description psoDesc;
        psoDesc.name = "Graphics_PSO_Present";
        psoDesc.rootSignature = &(mPresentSI.getRootSignature(device));
        psoDesc.vertexShader = graphics::CommandContext::compileShader(
            "ScreenQuadVS",
            shaders::ScreenQuadVS,
            "vs_5_0"
        );
        psoDesc.pixelShader = graphics::CommandContext::compileShader(
            "BufferCopyPS",
            shaders::BufferCopyPS,
            "ps_5_0"
        );
        psoDesc.numRenderTargets = 1;
        psoDesc.rtvFormats[0] = graphics::DefaultFormats::R8G8B8A8_UNORM;

        return graphics::GraphicsPipelineState(device, psoDesc);
    }
}
