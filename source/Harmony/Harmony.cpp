#include "Harmony.h"

#ifdef SOKOL

#include "SG_Window.h"
#include "SG_RenderAPI.h"

using namespace sokol;

#elif DX12

#include "Win32_Window.h"
#include "DX12_RenderAPI.h"
using namespace win32;
using namespace dx12;

#endif

#include "Inputs.h"

#include "ENTT_Scene.h"
//#include "Box2D_Scene.h"

#include "Logger.h"

#include "Helpers/HLog.h"

#include <iostream>

Harmony* Harmony::mInstance = nullptr;
Harmony* Harmony::getInstance()
{
	if (mInstance == nullptr)
	{
		mInstance = new Harmony();
	}
	return mInstance;
}

void Harmony::initializeSystem(const Harmony::Description description)
{
	Harmony::getInstance()->setWindow(Harmony::getInstance()->createNewWindow(description.windowDesc));

	Harmony::getInstance()->setInputSystem(Harmony::getInstance()->createNewInputSystem(description.inputSysDesc));

	Harmony::getInstance()->setRenderAPI(Harmony::getInstance()->createNewRenderAPI(description.rendererDesc));
}

harmony::Window* Harmony::createNewWindow(const harmony::Window::Description& description)
{
	return new Window(description);
}

harmony::Inputs* Harmony::createNewInputSystem(const harmony::Inputs::Description& description)
{
	return new Inputs(description);
}

harmony::RenderAPI* Harmony::createNewRenderAPI(const harmony::RenderAPI::Description& description)
{
	return new RenderAPI(description);
}

harmony::Logger* Harmony::createNewLogger(const harmony::Logger::Description& description)
{
	return new Logger(description);
}

harmony::Scene* Harmony::createECSScene(const harmony::Scene::Description& description) const
{
	return new ENTT::Scene(description);
}

harmony::Scene* Harmony::createPhysicsScene(const harmony::Scene::Description& description) const
{
	return nullptr; // new box2d::Scene(description);
}

harmony::Buffer* Harmony::createBuffer(const harmony::Buffer::Description& description) const
{
	return new Buffer(description);
}

harmony::Drawable* Harmony::createDrawable(const harmony::Drawable::Description& description) const
{
	return new Drawable(description);
}

harmony::Image::Description Harmony::getImageDescription(const char* imagePath) const
{
	return harmony::Image::Description();
}

harmony::Image* Harmony::createImage(const harmony::Image::Description& description) const
{
	return new Image(description);
}

harmony::Sampler* Harmony::createSampler(const harmony::Sampler::Description& description) const
{
	return new Sampler(description);
}

harmony::Shader* Harmony::createShader(const harmony::Shader::Description& description) const
{
	return new Shader(description);
}

harmony::Pipeline* Harmony::createPipeline(const harmony::Pipeline::Description& description) const
{
	return new Pipeline(description);
}

harmony::Swapchain* Harmony::createSwapchain(const harmony::Swapchain::Description& description) const
{
	return new Swapchain(description);
}
