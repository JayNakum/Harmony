#include "PassPresent.h"

namespace engine
{
	namespace renderPass
	{
		Present::Present(graphics::GraphicContext& context)
			: mSDRPipelineState(
				context.getDevice(),
				{
					"Present SDR",
					{},

				}
			)
		{}
	}
}

