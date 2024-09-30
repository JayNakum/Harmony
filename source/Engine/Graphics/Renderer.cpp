#include "Renderer.h"

namespace harmony
{
	Renderer::Renderer()
	{}

	void Renderer::render()
	{
		HARMONY->getRenderer()->frameBegin();
		{
			// RENDER HERE!
		}
		HARMONY->getRenderer()->frameEnd();
	}
}

