#include "PassPresent.h"

#include "ShaderInput.h"

namespace engine
{
    namespace renderPass
    {
        Present::Present(graphics::GraphicContext& context)
        {}

        void Present::render(graphics::GraphicContext& context, InputOutputs& inputOutputs)
        {
            const graphics::ShaderInput::Parameter slot1{ graphics::ShaderVisibility::Pixel,
                {{graphics::DescriptorRangeType::SRV, 1}}
            };

            const std::vector<graphics::ShaderInput::Parameter> layout = { slot1 };
            graphics::ShaderInput presentSI(context, layout);

            presentSI.setResource(inputOutputs.inputSRV, 0);

            graphics::dx12::Device& device = context.getDevice();

            graphics::dx12::GraphicsPipelineState pso(device, mPSODesc);
        }
    }
}
