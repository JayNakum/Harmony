#include "Engine.h"

namespace harmony
{
	void Engine::initialize()
	{
		HARMONY->getWindow()->initialize();
		HARMONY->getInputSystem()->initialize();
		HARMONY->getRenderAPI()->initialize();

		mRenderer.initialize();
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
		mRenderer.deinitialize();

		HARMONY->getRenderAPI()->deinitialize();
		HARMONY->getInputSystem()->deinitialize();
		HARMONY->getWindow()->deinitialize();
	}

	Engine::~Engine()
	{
		delete HARMONY->getRenderAPI();
		delete HARMONY->getInputSystem();
		delete HARMONY->getWindow();
	}
}
