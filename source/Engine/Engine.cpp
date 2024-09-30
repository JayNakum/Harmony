#include "Engine.h"

namespace harmony
{
	void Engine::initialize()
	{
		HARMONY->getWindow()->initialize();
		HARMONY->getInputSystem()->initialize();
		HARMONY->getRenderer()->initialize();
	}

	void Engine::update()
	{
		HARMONY->getWindow()->update();
		HARMONY->getInputSystem()->update();
	}

	void Engine::render()
	{
		mRenderer.render();
	}

	void Engine::deinitialize()
	{
		HARMONY->getRenderer()->deinitialize();
		HARMONY->getInputSystem()->deinitialize();
		HARMONY->getWindow()->deinitialize();
	}

	Engine::~Engine()
	{
		delete HARMONY->getRenderer();
		delete HARMONY->getInputSystem();
		delete HARMONY->getWindow();
	}
}
