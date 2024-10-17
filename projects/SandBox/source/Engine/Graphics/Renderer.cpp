#include "Renderer.h"

namespace harmony
{
	void Renderer::initialize()
	{
		auto& windowDesc = HARMONY->getWindow()->getDescription();

		harmony::Swapchain::Description createDesc;
		createDesc.label = "dx12 swapchain";
		createDesc.width = windowDesc.width;
		createDesc.height = windowDesc.height;
		mSwapchain = HARMONY->createSwapchain(createDesc);

		mSwapchain->initialize();
	}

	void Renderer::render()
	{
		HARMONY->getRenderAPI()->frameBegin();
		{
			mSwapchain->prepare();

			harmony::RenderTarget::Action action;
			action.colors.value[0] = 1.0f;
			action.colors.value[1] = 0.0f;
			action.colors.value[2] = 1.0f;
			action.colors.value[3] = 1.0f;
			mSwapchain->getCurrentRenderTarget()->bind(action);


			// RENDER HERE!


			mSwapchain->getCurrentRenderTarget()->unbind();
		}
		HARMONY->getRenderAPI()->frameEnd();

		mSwapchain->present();

		HARMONY->getRenderAPI()->present();
	}

	void Renderer::deinitialize()
	{
		mSwapchain->deinitialize();
	}

	Renderer::~Renderer()
	{
		delete mSwapchain;
	}
}
