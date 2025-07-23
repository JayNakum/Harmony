#include "NoisePass.h"

namespace engine
{
    namespace renderPass
    {
        Noise::Noise(graphics::dx12::Device& device, ResourceManager& resourceManager)
        {

        }

        void Noise::copyDataToGPU(graphics::ComputeContext& context, ResourceManager& resourceManager)
        {
        }

        void Noise::freeGPUUploadData(ResourceManager& resourceManager)
        {
        }

        void Noise::resize(const unsigned int width, const unsigned int height, graphics::dx12::Device& device, ResourceManager& resourceManager)
        {
        }

        void Noise::update(const double timeStep)
        {
        }

        void Noise::getInputOutputs(graphics::ComputeContext& context, ResourceManager& resourceManager)
        {
        }

        void Noise::render(graphics::ComputeContext& context, const FrameConstants& frameConstants)
        {
        }
    }
}
