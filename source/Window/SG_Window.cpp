#include "SG_Window.h"

namespace sokol
{
	Window::Window(const harmony::Window::Description& description)
		: mDescription(description) {}

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
		return false;
	}

	void Window::initialize() {}

	void Window::deinitialize()
	{}

	void Window::update()
	{}


}