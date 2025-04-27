#pragma once

#include "CommandContext.h"

namespace engine
{
	namespace renderPass
	{
		class Present
		{
		public:
			Present(graphics::GraphicContext& context);

			struct InputOutputs
			{
				graphics::dx12::DescriptorHandle inputRTV;

				graphics::dx12::DescriptorHandle outputRTV;
			};
			void render(graphics::GraphicContext& context, InputOutputs& inputOutputs, bool hdrSupport = false);

			~Present() = default;

		private:
			graphics::dx12::GraphicsPipelineState mSDRPipelineState;
			graphics::dx12::GraphicsPipelineState mHDRPipelineState;
		};
	}
}
