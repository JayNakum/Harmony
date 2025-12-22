#pragma once

#include "common.h"
#include "Log.h"
#include "win.h"

namespace windows
{
#if defined(HDEBUG)
    class WindowsMessageMap
    {
    public:
        WindowsMessageMap();
        std::string operator()(DWORD msg, LPARAM lp, WPARAM wp) const;
    private:
        std::unordered_map<DWORD, std::string> map;
    };
#endif

    class Window
    {
    private:
        class WindowClass
        {
        public:
            static const wchar_t* getName() noexcept;
            static HINSTANCE getInstance() noexcept;

        private:
            WindowClass() noexcept;
            WindowClass(const WindowClass& copy) = delete;
            WindowClass& operator=(const WindowClass& copy) = delete;
            static constexpr const wchar_t* sWndClassName = L"Harmony Window";
            HINSTANCE mHInstance;
        };

    public:
        struct Description
        {
            const std::wstring title = L"Harmony";
            unsigned int width = 0;
            unsigned int height = 0;
            bool isFullScreen = false;
            float dpiScale = 1.0f;
        };
        Window(const Description& description);

        inline const Description& getDescription() const { return mDescription; }
        inline HWND getHandle() const { return mHandle; }

        // Return two when the window is resized OR
        // the display of the window is changed
        inline bool didResize()
        {
            bool result = mDidResize;
            mDidResize = false;
            return result;
        }
        bool pollEvents();

        void appendWindowText(const std::string& text);

        ~Window();

    private:
        inline void onResize(const unsigned int width, const unsigned int height)
        {
            mDescription.width = width;
            mDescription.height = height;
            mDidResize = true;
        }

        inline void onMonitorChanged()
        {
            mDidResize = true;
        }

    private:
        Description mDescription;
        bool mDidResize;
        bool mIsRunning;
        HWND mHandle;
        MSG mMsgs;
        static LRESULT CALLBACK HandleMsgsSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
        static LRESULT CALLBACK HandleMsgs(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
        LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    };
}
