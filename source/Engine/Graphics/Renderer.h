#pragma once

#include "Harmony.h"

#include "Helpers/HLog.h"

namespace harmony
{
	class Renderer
	{
	public:
		Renderer();

		void render();

	private:
		harmony::Swapchain* mSwapchain;

	};
}