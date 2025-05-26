#include "PassWater.h"

#include "ShaderInput.h"

namespace shaders
{
    static const char* TriangleVS = R"###(
struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput main(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;
    result.position = position;
    result.color = color;
    return result;
}
)###";

    static const char* TrianglePS = R"###(
struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};
float4 main(PSInput input) : SV_TARGET
{
    return input.color;
}
)###";
}

namespace engine
{
    namespace renderPass
    {
        Water::Water(graphics::GraphicContext& context)
            : mTrianglePSO(initialize(context)) {}

        void Water::render(graphics::GraphicContext& context, InputOutputs& inputOutputs)
        {
            graphics::ShaderInput triangleSI(context, {/*no constants*/ }, {/*no parameters*/ });
            triangleSI.finalize();

            context.setPipelineState(mTrianglePSO);

            context.setRenderTargets(inputOutputs.inRTV);

            graphics::ViewPort viewport{};
            viewport.x = 0;
            viewport.y = 0;
            viewport.width = (float)inputOutputs.width;
            viewport.height = (float)inputOutputs.height;
            context.setViewport(viewport);

            graphics::Box scissor{};
            scissor.left = (unsigned int)viewport.x;
            scissor.top = (unsigned int)viewport.y;
            scissor.right = (unsigned int)viewport.x + (unsigned int)viewport.width;
            scissor.bottom = (unsigned int)viewport.y + (unsigned int)viewport.height;
            context.setScissor(scissor);

            const float clearColor[4] = { 0.118f, 0.106f, 0.180f, 1.0f };
            //const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
            context.clearColor(inputOutputs.inRTV, clearColor);

            context.setPrimitiveTopology(graphics::PrimitiveTopology::TriangleList);
            context.setVertexBuffer(0, &inputOutputs.inVertexBuffer);
            context.drawInstanced(3, 1);
        }

        graphics::dx12::GraphicsPipelineState Water::initialize(graphics::GraphicContext& context)
        {
            graphics::ShaderInput triangleSI(context, {/*no constants*/ }, {/*no parameters*/ });

            // Pipeline State
            graphics::dx12::GraphicsPipelineState::Description psoDesc;
            psoDesc.name = "triangle pso";
            psoDesc.inputLayout = {
                {"POSITION", graphics::DefaultFormats::R32G32B32A32_FLOAT, 0, graphics::InputClassification::PerVertex},
                {"COLOR", graphics::DefaultFormats::R32G32B32A32_FLOAT, 16, graphics::InputClassification::PerVertex},
            };
            psoDesc.rootSignature = &(triangleSI.getRootSignature());

            psoDesc.vertexShader = graphics::GraphicContext::compileShader(
                "TriangleVS",
                shaders::TriangleVS,
                "vs_5_0"
            );
            psoDesc.pixelShader = graphics::GraphicContext::compileShader(
                "TrianglePS",
                shaders::TrianglePS,
                "ps_5_0"
            );
            psoDesc.numRenderTargets = 1;
            psoDesc.rtvFormats[0] = graphics::DefaultFormats::R8G8B8A8_UNORM;
            psoDesc.cullMode = graphics::CullMode::None;
            return graphics::dx12::GraphicsPipelineState(context.getDevice(), psoDesc);
        }
    }
}
