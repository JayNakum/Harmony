#pragma once

#include "Window/HWindow.h"
#include "Inputs/HInputs.h"

#include "Graphics/HBuffer.h"
#include "Graphics/HDrawable.h"
#include "Graphics/HTexture.h"
#include "Graphics/HShader.h"
#include "Graphics/HPipeline.h"
#include "Graphics/HRenderPass.h"
#include "Graphics/HRenderer.h"

#include "Scene/HScene.h"

#include "HLogger.h"

#define HARMONY Harmony::getInstance()

class Harmony
{
private:
	Harmony() = default;
	static Harmony* mInstance;

public:
	~Harmony() = default;


	struct Description
	{
		harmony::Window::Description	windowDesc;
		harmony::Inputs::Description	inputSysDesc;
		harmony::Renderer::Description	rendererDesc;
	};

	static Harmony*		getInstance();
	static void			initializeSystem(const Harmony::Description description = {});

	harmony::Window*	createNewWindow(const harmony::Window::Description& description);
	harmony::Inputs*	createNewInputSystem(const harmony::Inputs::Description& description);
	harmony::Renderer*	createNewRenderer(const harmony::Renderer::Description& description);
	harmony::Logger*	createNewLogger(const harmony::Logger::Description& description = {});

	inline harmony::Window*		getWindow(unsigned int i = 0) const { return mMainWindow; }
	inline harmony::Inputs*		getInputSystem(unsigned int i = 0) const { return mInputSystem; }
	inline harmony::Renderer*	getRenderer(unsigned int i = 0) const { return mRenderer; }

	inline void setWindow(harmony::Window* const& window) { mMainWindow = window; };
	inline void setInputSystem(harmony::Inputs* const& inputSys) { mInputSystem = inputSys; }
	inline void setRenderer(harmony::Renderer* const& renderer) { mRenderer = renderer; }
	

	harmony::Scene*			createECSScene(const harmony::Scene::Description& description) const;
	harmony::Scene*			createPhysicsScene(const harmony::Scene::Description& description) const;
	harmony::Buffer*		createBuffer(const harmony::Buffer::Description& description) const;
	harmony::Drawable*		createDrawable(const harmony::Drawable::Description& description) const;
	harmony::Shader*		createShader(const harmony::Shader::Description& description) const;
	harmony::Pipeline*		createPipeline(const harmony::Pipeline::Description& description) const;
	//harmony::RenderPass*	createRenderPass(const harmony::RenderPass::Description& description) const;

	//harmony::Image::Description	getImageDescription(const char* imagePath) const;
	//harmony::Image*				createImage(const harmony::Image::Description& description) const;
	//harmony::Sampler*				createSampler(const harmony::Sampler::Description& description) const;

private:
	harmony::Window*	mMainWindow = nullptr;
	harmony::Inputs*	mInputSystem = nullptr;
	harmony::Renderer*	mRenderer = nullptr;
};
