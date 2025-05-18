#include "PassPresent.h"

#include "ShaderInput.h"

namespace shaders
{
static constexpr char* ScreenQuadVS = R"###(
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

static constexpr char* BufferCopyPS = R"###(
Texture2D colorTex : register(t0);

float4 main(float4 position : SV_Position) : SV_Target0
{
    return colorTex[(int2)position.xy];
}
)###";
}

namespace engine
{
    namespace renderPass
    {
        void Present::render(graphics::GraphicContext& context, InputOutputs& inputOutputs)
        {
            const graphics::ShaderInput::Parameter slot1{ graphics::ShaderVisibility::Pixel,
                {{graphics::DescriptorRangeType::SRV, 1}}
            };
            const std::vector<graphics::ShaderInput::Parameter> parametersLayout = { slot1 };

            graphics::ShaderInput presentSI(context, {/*no constants*/}, parametersLayout);
            presentSI.setResource(inputOutputs.inSRV, 0);

            graphics::dx12::GraphicsPipelineState::Description psoDesc;
            psoDesc.name = "present pso";
            psoDesc.rootSignature = &(presentSI.getRootSignature());
            psoDesc.vertexShader = graphics::GraphicContext::compileShader(
                "ScreenQuadVS",
                shaders::ScreenQuadVS,
                "vs_5_0"
            );
            psoDesc.pixelShader = graphics::GraphicContext::compileShader(
                "BufferCopyPS",
                shaders::BufferCopyPS,
                "ps_5_0"
            );
            graphics::dx12::GraphicsPipelineState presentPSO(context.getDevice(), psoDesc);

            context.setPipelineState(presentPSO);

            context.transitionResource(inputOutputs.inRenderTarget.getTexture(), graphics::GPUResourceStates::RenderTarget);
            context.setRenderTargets(1, inputOutputs.inRTV);

            graphics::ViewPort viewport{};
            viewport.x = 0;
            viewport.y = 0;
            viewport.width = inputOutputs.inRenderTarget.getTexture().describeResource().width;
            viewport.height = inputOutputs.inRenderTarget.getTexture().describeResource().height;
            context.setViewport(viewport);

            graphics::Box scissor{};
            scissor.left = viewport.x;
            scissor.top = viewport.y;
            scissor.right = viewport.x + viewport.width;
            scissor.bottom = viewport.y + viewport.height;
            context.setScissor(scissor);

            context.setPrimitiveTopology(graphics::PrimitiveTopology::TriangleList);
            context.draw(3);
            context.transitionResource(inputOutputs.inRenderTarget.getTexture(), graphics::GPUResourceStates::Present);
        }
    }
}
