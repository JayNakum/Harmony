#pragma once

#include "Harmony.h"

#include "Helpers/HLog.h"

namespace harmony
{
	class Renderer
	{
	public:
		void initialize();
		void render(const harmony::Scene* gameScene);
		void deinitialize();
		~Renderer();

	private:
		harmony::Swapchain* mSwapchain;

		harmony::Pipeline* pipeline = nullptr;
	};
}