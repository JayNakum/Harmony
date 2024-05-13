#pragma once

#include "Window/HWindow.h"

namespace sokol
{
	class Window : public harmony::Window
	{
	public:
		Window(const harmony::Window::Description& description);
		~Window();

	public:
		const Description& getDescription() const override;

		void* getHandle() override;

		bool isRunning() const override;

		void initialize() override;
		void deinitialize() override;
		void update() override;

	private:
		harmony::Window::Description mDescription;
	};
}