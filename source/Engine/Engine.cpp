#include <iostream>

#include "Harmony.h"

#include <sokol_app.h>

#include <functional>

namespace sokol
{
	static void init()
	{
		harmony::getRenderer()->initialize();
	}

	static void frame()
	{
		harmony::getRenderer()->render();
	}

	static void cleanup()
	{
		delete harmony::getRenderer();
		delete harmony::getWindow();
	}
}

sapp_desc sokol_main(int argc, char* argv[])
{
	{
		harmony::Window::Description windowDescription;
		windowDescription.width = 600;
		windowDescription.height = 400;
		windowDescription.isFullScreen = false;
		harmony::createWindow(windowDescription);
	}

	sapp_desc appDesc = { 0 };
	{
		const auto& windowDesc = harmony::getWindow()->getDescription();
		appDesc.window_title = windowDesc.title;
		appDesc.width = windowDesc.width;
		appDesc.height = windowDesc.height;
		appDesc.fullscreen = windowDesc.isFullScreen;

		appDesc.init_cb = sokol::init;
		appDesc.frame_cb = sokol::frame;
		appDesc.cleanup_cb = sokol::cleanup;
	}
	return appDesc;
}
