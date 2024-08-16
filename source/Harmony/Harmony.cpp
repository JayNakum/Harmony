#include "Harmony.h"

#include <iostream>

//#include "Win32_Window.h"
#include "SG_Window.h"

#include "Inputs.h"

//#include "DX12_Renderer.h"
#include "SG_Renderer.h"

#include "ENTT_Scene.h"
#include "PhysX_Scene.h"

#include "Logger.h"

static harmony::Window* gMainWindow = nullptr;
static harmony::Inputs* gInputSystem = nullptr;
static harmony::Renderer* gRenderer = nullptr;

namespace harmony
{
	void createWindow(const harmony::Window::Description& windowDescription)
	{
		delete gMainWindow;
		gMainWindow = new sokol::Window(windowDescription);
	}

	harmony::Logger* createLogger(const harmony::Logger::Description& loggerDescription)
	{
		return new ::Logger(loggerDescription);
	}

	harmony::Window* getWindow()
	{
		if (gMainWindow == nullptr)
		{
			harmony::Window::Description defaultDescription;
			createWindow(defaultDescription);
		}
		return gMainWindow;
	}

	void createInputSystem(const harmony::Inputs::Description& inputSysDescription)
	{
		delete gInputSystem;
		gInputSystem = new ::Inputs(inputSysDescription);
	}

	harmony::Inputs* getInputSystem()
	{
		if (gInputSystem == nullptr)
		{
			harmony::Inputs::Description defaultDescription;
			createInputSystem(defaultDescription);
		}
		return gInputSystem;
	}

	void createRenderer(const harmony::Renderer::Description& rendererDescription)
	{
		delete gRenderer;
		gRenderer = new sokol::Renderer(rendererDescription);
	}
	harmony::Renderer* getRenderer()
	{
		if (gRenderer == nullptr)
		{
			harmony::Renderer::Description defaultDescription;
			createRenderer(defaultDescription);
		}
		return gRenderer;
	}

	harmony::Scene* createScene(const harmony::Scene::Description& description)
	{
		return new ENTT::Scene(description);
	}

	harmony::Scene* createPhysicsScene(const harmony::Scene::Description& description)
	{
		return new physx::Scene(description);
	}

	harmony::Buffer* createBuffer(const harmony::Buffer::Description& description)
	{
		return new sokol::Buffer(description);
	}
	
	harmony::Drawable* createDrawable(const harmony::Drawable::Description& description)
	{
		return new sokol::Drawable(description);
	}
	
	harmony::Image* createImage(const harmony::Image::Description& description)
	{
		return new sokol::Image(description);
	}
	harmony::Sampler* createSampler(const harmony::Sampler::Description& description)
	{
		return new sokol::Sampler(description);
	}
	harmony::Shader* createShader(const harmony::Shader::Description& description)
	{
		return new sokol::Shader(description);
	}
	harmony::Pipeline* createPipeline(const harmony::Pipeline::Description& description)
	{
		return new sokol::Pipeline(description);
	}
	harmony::RenderPass* createRenderPass(const harmony::RenderPass::Description& description)
	{
		return new sokol::RenderPass(description);
	}
	
}
