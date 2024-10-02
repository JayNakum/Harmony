#include "Renderer.h"

namespace harmony
{
	Renderer::Renderer()
	{}

	void Renderer::render()
	{
		HARMONY->getRenderAPI()->frameBegin();
		{
			// RENDER HERE!
		}
		HARMONY->getRenderAPI()->frameEnd();
	}
}

