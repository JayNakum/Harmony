#include "Engine.h"

namespace harmony
{
	void Engine::initialize()
	{
		harmony::getWindow()->initialize();
		harmony::getInputSystem()->initialize();
		harmony::getRenderer()->initialize();
	}

	void Engine::update()
	{
		harmony::getWindow()->update();
		harmony::getInputSystem()->update();
	}

	void Engine::render()
	{
		harmony::getRenderer()->render();
	}

	void Engine::deinitialize()
	{
		harmony::getRenderer()->deinitialize();
		harmony::getInputSystem()->deinitialize();
		harmony::getWindow()->deinitialize();
	}

	Engine::~Engine()
	{
		delete harmony::getRenderer();
		delete harmony::getInputSystem();
		delete harmony::getWindow();
	}
}
