#pragma once

#include "Window/HWindow.h"

#include "Graphics/HRenderer.h"

#include "HLogger.h"

namespace harmony
{
	harmony::Logger* createLogger(const harmony::Logger::Description& description = {});

	void createWindow(const harmony::Window::Description& description);
	harmony::Window* getWindow();

	void createRenderer();
	harmony::Renderer* getRenderer();
}
