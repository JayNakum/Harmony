#pragma once

#include "Window/HWindow.h"
#include "Inputs/HInputs.h"

#include "Graphics/HBuffer.h"
#include "Graphics/HDrawable.h"
#include "Graphics/HShader.h"
#include "Graphics/HPipeline.h"
#include "Graphics/HRenderPass.h"
#include "Graphics/HRenderer.h"

#include "Scene/HScene.h"

#include "HLogger.h"

namespace harmony
{
	harmony::Logger* createLogger(const harmony::Logger::Description& description = {});

	void createWindow(const harmony::Window::Description& description);
	harmony::Window* getWindow();

	void createInputSystem(const harmony::Inputs::Description& inputSysDescription);
	harmony::Inputs* getInputSystem();

	/*void createRenderer();
	harmony::Renderer* getRenderer();*/

	harmony::Scene* createScene(const harmony::Scene::Description& description);

	/*harmony::Buffer* createBuffer(const harmony::Buffer::Description& description);
	harmony::Drawable* createDrawable(const harmony::Drawable::Description& description);

	harmony::Shader* createShader(const harmony::Shader::Description& description);
	harmony::Pipeline* createPipeline(const harmony::Pipeline::Description& description);

	harmony::RenderPass* createRenderPass(const harmony::RenderPass::Description& description);*/
}
