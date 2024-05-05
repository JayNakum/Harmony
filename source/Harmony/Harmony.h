#pragma once

#include "HLogger.h"

#include "HGraphics.h"

namespace harmony
{
	// harmony::Window* createWin32Window(const harmony::Window::Description& description);
	harmony::Logger* createLogger(const harmony::Logger::Description& description = {});

	harmony::Renderer* createSGRenderer();
	harmony::Drawable* createSGDrawable();
	harmony::Buffer* createSGBuffer(const harmony::Buffer::Description& description);
	harmony::Shader* createSGShader(const harmony::Shader::Description& description);
	harmony::Pipeline* createSGPipeline(const harmony::Pipeline::Description& description);
	harmony::RenderPass* createSGRenderPass(const harmony::RenderPass::Description& description);
}
