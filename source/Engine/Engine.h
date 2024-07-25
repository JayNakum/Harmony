#pragma once

#define GLM_FORCE_SSE2
#define GLM_FORCE_INLINE
#define GLM_FORCE_ALIGNED
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Scene/HScene.h"

#include "Helpers/HLog.h"

namespace harmony
{
	class Renderer;
}

namespace harmony
{
	class Engine
	{
	public:
		Engine();

		void initialize();
		void update();
		void render();
		void deinitialize();

		~Engine();

	private:
		harmony::Scene* scene = nullptr;

		harmony::Pipeline* pipeline = nullptr;
		harmony::RenderPass* pass = nullptr;
	};
}

namespace harmony
{
	class Application
	{
	public:
		Application();

		virtual void initialize();
		virtual void update();
		virtual void deinitialize();

		~Application();
	};
}