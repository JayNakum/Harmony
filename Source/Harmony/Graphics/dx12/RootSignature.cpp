#include "RootSignature.h"


#include "Renderer.h"

namespace graphics
{
    namespace dx12
    {
        void RootSignature::initStaticSampler(unsigned int shaderRegister, const StaticSamplerDescription& samplerDesc, unsigned int registerSpace)
        {
            HASSERT(initializedSamplers < mStaticSamplers.size(), "Trying to initialize more than %d samplers. Which was the initial specified size.");
            D3D12_STATIC_SAMPLER_DESC& staticSampler = mStaticSamplers.at(initializedSamplers++);


        }

        void RootSignature::finalize()
        {
            // create and/or serialize
        }
    }
}
