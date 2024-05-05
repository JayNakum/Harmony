#include "Harmony.h"

#include <iostream>

#include "Window.h"
#include "Logger.h"

#include "SG_Graphics.h"

namespace harmony
{
	harmony::Window* createWin32Window(const harmony::Window::Description& windowDescription)
	{
		return new ::Window(windowDescription);
	}

	harmony::Logger* createLogger(const harmony::Logger::Description& loggerDescription)
	{
		return new ::Logger(loggerDescription);
	}

	harmony::Renderer* createSGRenderer()
	{
		return new Sokol::Renderer();
	}
	
	harmony::Drawable* createSGDrawable()
	{
		return new Sokol::Drawable();
	}

	harmony::Buffer* createSGBuffer(const harmony::Buffer::Description& description)
	{
		return new Sokol::Buffer(description);
	}
	
	harmony::Shader* createSGShader(const harmony::Shader::Description& description)
	{
		return new Sokol::Shader(description);
	}

	harmony::Pipeline* createSGPipeline(const harmony::Pipeline::Description& description)
	{
		return new Sokol::Pipeline(description);
	}

	harmony::RenderPass* createSGRenderPass(const harmony::RenderPass::Description& description)
	{
		return new Sokol::RenderPass(description);
	}
}
