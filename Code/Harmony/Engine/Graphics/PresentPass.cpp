#include "PresentPass.h"

namespace shaders
{
    static const char* ScreenQuadVS = R"###(
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
    namespace renderPass
    {
        Present::Present(graphics::GraphicContext& context)
            : mPresentPSO(initialize(context))
        {}

        void Present::render(graphics::GraphicContext& context, Inputs& passInputs)
        {
            {// Pre Render

                mFinalSRV = context.allocateDescriptors(graphics::DescriptorHeapType::ShaderViews);

                passInputs.inFinalOutput.createShaderResourceView(context.getDevice(), mFinalSRV,
                    graphics::dx12::GPUTexture::ShaderResourceViewInfo::getSRVInfo(
                        passInputs.inFinalOutput.getDescription()
                    )
                );

                context.transitionResource(passInputs.inFinalOutput, graphics::GPUResourceStates::PixelShaderResource);

                context.transitionResource(passInputs.inSwapchainBackbuffer, graphics::GPUResourceStates::RenderTarget, true);
            }

            {// Render
                const graphics::ShaderInput::Parameter slot1{ graphics::ShaderVisibility::Pixel,
                    {{graphics::DescriptorRangeType::SRV, 1}}
                };
                const std::vector<graphics::ShaderInput::Parameter> parametersLayout = { slot1 };

                graphics::ShaderInput presentSI(context, {/*no constants*/}, parametersLayout);
                presentSI.setResource(mFinalSRV, 0);
                presentSI.finalize();
                context.setPipelineState(mPresentPSO);

                graphics::ViewPort viewport{};
                viewport.x = 0;
                viewport.y = 0;
                viewport.width = (float)passInputs.width;
                viewport.height = (float)passInputs.height;
                context.setViewport(viewport);

                graphics::Box scissor{};
                scissor.left = (unsigned int)viewport.x;
                scissor.top = (unsigned int)viewport.y;
                scissor.right = (unsigned int)viewport.x + (unsigned int)viewport.width;
                scissor.bottom = (unsigned int)viewport.y + (unsigned int)viewport.height;
                context.setScissor(scissor);

                context.setRenderTargets(passInputs.inSwapchainRTV);
                const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
                context.clearColor(passInputs.inSwapchainRTV, clearColor);
                context.setPrimitiveTopology(graphics::PrimitiveTopology::TriangleList);
                context.draw(3);
            }

            {// Post Render
                context.transitionResource(passInputs.inSwapchainBackbuffer, graphics::GPUResourceStates::Present, true);
            }
        }

        graphics::dx12::GraphicsPipelineState Present::initialize(graphics::GraphicContext& context)
        {
            const graphics::ShaderInput::Parameter slot1{ graphics::ShaderVisibility::Pixel,
                {{graphics::DescriptorRangeType::SRV, 1}}
            };
            const std::vector<graphics::ShaderInput::Parameter> parametersLayout = { slot1 };

            graphics::ShaderInput presentSI(context, {/*no constants*/ }, parametersLayout);

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
            psoDesc.numRenderTargets = 1;
            psoDesc.rtvFormats[0] = graphics::DefaultFormats::R8G8B8A8_UNORM;

            return graphics::dx12::GraphicsPipelineState(context.getDevice(), psoDesc);
        }
    }
}
