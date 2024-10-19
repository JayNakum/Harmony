#pragma once

#include "Harmony.h"

#include "Graphics/Renderer.h"

#include "Helpers/HLog.h"

namespace harmony
{
	class Engine
	{
	public:
		void initialize();
		void update();
		void render(const harmony::Scene* gameScene);
		void deinitialize();

		~Engine();

	private:
		Renderer mRenderer;
	};
}
