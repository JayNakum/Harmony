#include "SG_Window.h"

namespace sokol
{
	Window::Window(const harmony::Window::Description& description)
		: mDescription(description)
	{
		if (mDescription.width == 0)
			mDescription.width = 1024;
		if (mDescription.height == 0)
			mDescription.height = 576;
	}

	Window::~Window()
	{
		mDescription = { 0 };
	}

	const harmony::Window::Description& Window::getDescription() const
	{
		return mDescription;
	}

	void* Window::getHandle()
	{
		return nullptr;
	}

	bool Window::isRunning() const
	{
		return true;
	}

	void Window::initialize() {}

	void Window::deinitialize() {}

	void Window::update() {}

	void Window::resize(unsigned int width, unsigned int height)
	{
		mDescription.width = width;
		mDescription.height = height;
	}


}