#pragma once

#include "Win.h"
#include "Window/HWindow.h"

#include <unordered_map>
#include <string>

namespace harmony
{
	class Inputs;
}

namespace win32
{
	class WindowsMessageMap
	{
	public:
		WindowsMessageMap();
		std::string operator()(DWORD msg, LPARAM lp, WPARAM wp) const;
	private:
		std::unordered_map<DWORD, std::string> map;
	};


	class Window : public harmony::Window
	{
	private:
		class WindowClass
		{
		public:
			static const char* getName() noexcept;
			static HINSTANCE getInstance() noexcept;

		private:
			WindowClass() noexcept;
			~WindowClass();
			WindowClass(const WindowClass& copy) = delete;
			WindowClass& operator=(const WindowClass& copy) = delete;
			static constexpr const char* sWndClassName = "Harmony Window";
			static WindowClass sWndClass;
			HINSTANCE mHInstance;
		};

	public:
		Window(const harmony::Window::Description& description);
		~Window();

		const harmony::Window::Description& getDescription() const override;
		void* getHandle() override;
		bool isRunning() const override;
		
		void initialize() override;
		void resize(unsigned int width, unsigned int height) override;
		void update() override;
		void deinitialize() override;

	private:
		harmony::Window::Description mDescription;

		harmony::Inputs* mInputSystem;

		bool mIsRunning;
		HWND mHandle;
		MSG mMsgs;
		static LRESULT CALLBACK HandleMsgsSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		static LRESULT CALLBACK HandleMsgs(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	};
}
