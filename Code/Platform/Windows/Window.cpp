#include "Window.h"

#include "Log.h"

// secret messages
#define WM_UAHDESTROYWINDOW 0x0090
#define WM_UAHDRAWMENU 0x0091
#define WM_UAHDRAWMENUITEM 0x0092
#define WM_UAHINITMENU 0x0093
#define WM_UAHMEASUREMENUITEM 0x0094
#define WM_UAHNCPAINTMENUPOPUP 0x0095

#define REGISTER_MESSAGE(msg){msg,#msg}

namespace windows
{
#if defined(DEBUG)
    WindowsMessageMap::WindowsMessageMap()
        :
        map({
            REGISTER_MESSAGE(WM_CREATE),
            REGISTER_MESSAGE(WM_DESTROY),
            REGISTER_MESSAGE(WM_MOVE),
            REGISTER_MESSAGE(WM_SIZE),
            REGISTER_MESSAGE(WM_ACTIVATE),
            REGISTER_MESSAGE(WM_SETFOCUS),
            REGISTER_MESSAGE(WM_KILLFOCUS),
            REGISTER_MESSAGE(WM_ENABLE),
            REGISTER_MESSAGE(WM_SETREDRAW),
            REGISTER_MESSAGE(WM_SETTEXT),
            REGISTER_MESSAGE(WM_GETTEXT),
            REGISTER_MESSAGE(WM_GETTEXTLENGTH),
            REGISTER_MESSAGE(WM_PAINT),
            REGISTER_MESSAGE(WM_CLOSE),
            REGISTER_MESSAGE(WM_QUERYENDSESSION),
            REGISTER_MESSAGE(WM_QUIT),
            REGISTER_MESSAGE(WM_QUERYOPEN),
            REGISTER_MESSAGE(WM_ERASEBKGND),
            REGISTER_MESSAGE(WM_SYSCOLORCHANGE),
            REGISTER_MESSAGE(WM_ENDSESSION),
            REGISTER_MESSAGE(WM_SHOWWINDOW),
            REGISTER_MESSAGE(WM_CTLCOLORMSGBOX),
            REGISTER_MESSAGE(WM_CTLCOLOREDIT),
            REGISTER_MESSAGE(WM_CTLCOLORLISTBOX),
            REGISTER_MESSAGE(WM_CTLCOLORBTN),
            REGISTER_MESSAGE(WM_CTLCOLORDLG),
            REGISTER_MESSAGE(WM_CTLCOLORSCROLLBAR),
            REGISTER_MESSAGE(WM_CTLCOLORSTATIC),
            REGISTER_MESSAGE(WM_WININICHANGE),
            REGISTER_MESSAGE(WM_SETTINGCHANGE),
            REGISTER_MESSAGE(WM_DEVMODECHANGE),
            REGISTER_MESSAGE(WM_ACTIVATEAPP),
            REGISTER_MESSAGE(WM_FONTCHANGE),
            REGISTER_MESSAGE(WM_TIMECHANGE),
            REGISTER_MESSAGE(WM_CANCELMODE),
            REGISTER_MESSAGE(WM_SETCURSOR),
            REGISTER_MESSAGE(WM_MOUSEACTIVATE),
            REGISTER_MESSAGE(WM_CHILDACTIVATE),
            REGISTER_MESSAGE(WM_QUEUESYNC),
            REGISTER_MESSAGE(WM_GETMINMAXINFO),
            REGISTER_MESSAGE(WM_ICONERASEBKGND),
            REGISTER_MESSAGE(WM_NEXTDLGCTL),
            REGISTER_MESSAGE(WM_SPOOLERSTATUS),
            REGISTER_MESSAGE(WM_DRAWITEM),
            REGISTER_MESSAGE(WM_MEASUREITEM),
            REGISTER_MESSAGE(WM_DELETEITEM),
            REGISTER_MESSAGE(WM_VKEYTOITEM),
            REGISTER_MESSAGE(WM_CHARTOITEM),
            REGISTER_MESSAGE(WM_SETFONT),
            REGISTER_MESSAGE(WM_GETFONT),
            REGISTER_MESSAGE(WM_QUERYDRAGICON),
            REGISTER_MESSAGE(WM_COMPAREITEM),
            REGISTER_MESSAGE(WM_COMPACTING),
            REGISTER_MESSAGE(WM_NCCREATE),
            REGISTER_MESSAGE(WM_NCDESTROY),
            REGISTER_MESSAGE(WM_NCCALCSIZE),
            REGISTER_MESSAGE(WM_NCHITTEST),
            REGISTER_MESSAGE(WM_NCPAINT),
            REGISTER_MESSAGE(WM_NCACTIVATE),
            REGISTER_MESSAGE(WM_GETDLGCODE),
            REGISTER_MESSAGE(WM_NCMOUSEMOVE),
            REGISTER_MESSAGE(WM_NCLBUTTONDOWN),
            REGISTER_MESSAGE(WM_NCLBUTTONUP),
            REGISTER_MESSAGE(WM_NCLBUTTONDBLCLK),
            REGISTER_MESSAGE(WM_NCRBUTTONDOWN),
            REGISTER_MESSAGE(WM_NCRBUTTONUP),
            REGISTER_MESSAGE(WM_NCRBUTTONDBLCLK),
            REGISTER_MESSAGE(WM_NCMBUTTONDOWN),
            REGISTER_MESSAGE(WM_NCMBUTTONUP),
            REGISTER_MESSAGE(WM_NCMBUTTONDBLCLK),
            REGISTER_MESSAGE(WM_KEYDOWN),
            REGISTER_MESSAGE(WM_KEYUP),
            REGISTER_MESSAGE(WM_CHAR),
            REGISTER_MESSAGE(WM_DEADCHAR),
            REGISTER_MESSAGE(WM_SYSKEYDOWN),
            REGISTER_MESSAGE(WM_SYSKEYUP),
            REGISTER_MESSAGE(WM_SYSCHAR),
            REGISTER_MESSAGE(WM_SYSDEADCHAR),
            REGISTER_MESSAGE(WM_KEYLAST),
            REGISTER_MESSAGE(WM_INITDIALOG),
            REGISTER_MESSAGE(WM_COMMAND),
            REGISTER_MESSAGE(WM_SYSCOMMAND),
            REGISTER_MESSAGE(WM_TIMER),
            REGISTER_MESSAGE(WM_HSCROLL),
            REGISTER_MESSAGE(WM_VSCROLL),
            REGISTER_MESSAGE(WM_INITMENU),
            REGISTER_MESSAGE(WM_INITMENUPOPUP),
            REGISTER_MESSAGE(WM_MENUSELECT),
            REGISTER_MESSAGE(WM_MENUCHAR),
            REGISTER_MESSAGE(WM_ENTERIDLE),
            REGISTER_MESSAGE(WM_MOUSEWHEEL),
            REGISTER_MESSAGE(WM_MOUSEMOVE),
            REGISTER_MESSAGE(WM_LBUTTONDOWN),
            REGISTER_MESSAGE(WM_LBUTTONUP),
            REGISTER_MESSAGE(WM_LBUTTONDBLCLK),
            REGISTER_MESSAGE(WM_RBUTTONDOWN),
            REGISTER_MESSAGE(WM_RBUTTONUP),
            REGISTER_MESSAGE(WM_RBUTTONDBLCLK),
            REGISTER_MESSAGE(WM_MBUTTONDOWN),
            REGISTER_MESSAGE(WM_MBUTTONUP),
            REGISTER_MESSAGE(WM_MBUTTONDBLCLK),
            REGISTER_MESSAGE(WM_PARENTNOTIFY),
            REGISTER_MESSAGE(WM_MDICREATE),
            REGISTER_MESSAGE(WM_MDIDESTROY),
            REGISTER_MESSAGE(WM_MDIACTIVATE),
            REGISTER_MESSAGE(WM_MDIRESTORE),
            REGISTER_MESSAGE(WM_MDINEXT),
            REGISTER_MESSAGE(WM_MDIMAXIMIZE),
            REGISTER_MESSAGE(WM_MDITILE),
            REGISTER_MESSAGE(WM_MDICASCADE),
            REGISTER_MESSAGE(WM_MDIICONARRANGE),
            REGISTER_MESSAGE(WM_MDIGETACTIVE),
            REGISTER_MESSAGE(WM_MDISETMENU),
            REGISTER_MESSAGE(WM_CUT),
            REGISTER_MESSAGE(WM_COPYDATA),
            REGISTER_MESSAGE(WM_COPY),
            REGISTER_MESSAGE(WM_PASTE),
            REGISTER_MESSAGE(WM_CLEAR),
            REGISTER_MESSAGE(WM_UNDO),
            REGISTER_MESSAGE(WM_RENDERFORMAT),
            REGISTER_MESSAGE(WM_RENDERALLFORMATS),
            REGISTER_MESSAGE(WM_DESTROYCLIPBOARD),
            REGISTER_MESSAGE(WM_DRAWCLIPBOARD),
            REGISTER_MESSAGE(WM_PAINTCLIPBOARD),
            REGISTER_MESSAGE(WM_VSCROLLCLIPBOARD),
            REGISTER_MESSAGE(WM_SIZECLIPBOARD),
            REGISTER_MESSAGE(WM_ASKCBFORMATNAME),
            REGISTER_MESSAGE(WM_CHANGECBCHAIN),
            REGISTER_MESSAGE(WM_HSCROLLCLIPBOARD),
            REGISTER_MESSAGE(WM_QUERYNEWPALETTE),
            REGISTER_MESSAGE(WM_PALETTEISCHANGING),
            REGISTER_MESSAGE(WM_PALETTECHANGED),
            /*REGISTER_MESSAGE(WM_DDE_INITIATE),
            REGISTER_MESSAGE(WM_DDE_TERMINATE),
            REGISTER_MESSAGE(WM_DDE_ADVISE),
            REGISTER_MESSAGE(WM_DDE_UNADVISE),
            REGISTER_MESSAGE(WM_DDE_ACK),
            REGISTER_MESSAGE(WM_DDE_DATA),
            REGISTER_MESSAGE(WM_DDE_REQUEST),
            REGISTER_MESSAGE(WM_DDE_POKE),
            REGISTER_MESSAGE(WM_DDE_EXECUTE),*/
            REGISTER_MESSAGE(WM_DROPFILES),
            REGISTER_MESSAGE(WM_POWER),
            REGISTER_MESSAGE(WM_WINDOWPOSCHANGED),
            REGISTER_MESSAGE(WM_WINDOWPOSCHANGING),
            REGISTER_MESSAGE(WM_HELP),
            REGISTER_MESSAGE(WM_NOTIFY),
            REGISTER_MESSAGE(WM_CONTEXTMENU),
            REGISTER_MESSAGE(WM_TCARD),
            REGISTER_MESSAGE(WM_MDIREFRESHMENU),
            REGISTER_MESSAGE(WM_MOVING),
            REGISTER_MESSAGE(WM_STYLECHANGED),
            REGISTER_MESSAGE(WM_STYLECHANGING),
            REGISTER_MESSAGE(WM_SIZING),
            REGISTER_MESSAGE(WM_SETHOTKEY),
            REGISTER_MESSAGE(WM_PRINT),
            REGISTER_MESSAGE(WM_PRINTCLIENT),
            REGISTER_MESSAGE(WM_POWERBROADCAST),
            REGISTER_MESSAGE(WM_HOTKEY),
            REGISTER_MESSAGE(WM_GETICON),
            REGISTER_MESSAGE(WM_EXITMENULOOP),
            REGISTER_MESSAGE(WM_ENTERMENULOOP),
            REGISTER_MESSAGE(WM_DISPLAYCHANGE),
            REGISTER_MESSAGE(WM_STYLECHANGED),
            REGISTER_MESSAGE(WM_STYLECHANGING),
            REGISTER_MESSAGE(WM_GETICON),
            REGISTER_MESSAGE(WM_SETICON),
            REGISTER_MESSAGE(WM_SIZING),
            REGISTER_MESSAGE(WM_MOVING),
            REGISTER_MESSAGE(WM_CAPTURECHANGED),
            REGISTER_MESSAGE(WM_DEVICECHANGE),
            REGISTER_MESSAGE(WM_PRINT),
            REGISTER_MESSAGE(WM_PRINTCLIENT),
            REGISTER_MESSAGE(WM_IME_SETCONTEXT),
            REGISTER_MESSAGE(WM_IME_NOTIFY),
            REGISTER_MESSAGE(WM_NCMOUSELEAVE),
            REGISTER_MESSAGE(WM_EXITSIZEMOVE),
            REGISTER_MESSAGE(WM_UAHDESTROYWINDOW),
            REGISTER_MESSAGE(WM_DWMNCRENDERINGCHANGED),
            REGISTER_MESSAGE(WM_ENTERSIZEMOVE),
            })
    {}

    std::string WindowsMessageMap::operator()(DWORD msg, LPARAM lp, WPARAM wp) const
    {
        constexpr int firstColWidth = 25;
        const auto i = map.find(msg);

        std::ostringstream oss;
        if (i != map.end())
        {
            oss << std::left << std::setw(firstColWidth) << i->second << std::right;
        }
        else
        {
            std::ostringstream padss;
            padss << "Unknown message: 0x" << std::hex << msg;
            oss << std::left << std::setw(firstColWidth) << padss.str() << std::right;
        }
        oss << "   LP: 0x" << std::hex << std::setfill('0') << std::setw(8) << lp;
        oss << "   WP: 0x" << std::hex << std::setfill('0') << std::setw(8) << wp << std::endl;

        return oss.str();
    }
#endif

    Window::WindowClass Window::WindowClass::sWndClass;

    const wchar_t* Window::WindowClass::getName() noexcept
    {
        return sWndClassName;
    }

    HINSTANCE Window::WindowClass::getInstance() noexcept
    {
        return sWndClass.mHInstance;
    }

    Window::WindowClass::WindowClass() noexcept
        : mHInstance(GetModuleHandle(nullptr))
    {
        WNDCLASSEX wc = { 0 };
        wc.cbSize = sizeof(wc);
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = HandleMsgsSetup;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = getInstance();
        wc.hIcon = NULL;
        wc.hIconSm = NULL;
        wc.hCursor = nullptr;
        wc.hbrBackground = nullptr;
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = getName();
        RegisterClassEx(&wc);
    }

    Window::WindowClass::~WindowClass()
    {
        UnregisterClass(sWndClassName, getInstance());
    }


    Window::Window(const Description& description)
        : mDescription(description), mIsRunning(true), mHandle(NULL), mMsgs(), mDidResize(true)
    {
        if (mDescription.width == 0)
            mDescription.width = GetSystemMetrics(SM_CXMAXIMIZED);
        if (mDescription.height == 0)
            mDescription.height = GetSystemMetrics(SM_CYMAXIMIZED);

        RECT wr = { 0 };
        wr.left = 0;
        wr.right = mDescription.width + wr.left;
        wr.top = 0;
        wr.bottom = mDescription.height + wr.top;
        AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

        mDescription.width = wr.right - wr.left;
        mDescription.height = wr.bottom - wr.top;
        mDescription.width -= mDescription.width / 6;
        mDescription.height -= mDescription.height / 4;

        const unsigned int x = (wr.right - mDescription.width) / 2;
        const unsigned int y = (wr.bottom - mDescription.height) / 2;

        mHandle = CreateWindow(WindowClass::getName(), mDescription.title.c_str(),
                               WS_OVERLAPPEDWINDOW,
                               x, y, mDescription.width, mDescription.height,
                               nullptr, nullptr, WindowClass::getInstance(), this);

        HASSERT(mHandle != NULL, "Unable to create a window.");

        ShowWindow(mHandle, SW_SHOW);

        PeekMessage(&mMsgs, NULL, 0, 0, PM_NOREMOVE);
    }

    bool Window::pollEvents()
    {
        while ((PeekMessage(&mMsgs, NULL, 0, 0, PM_REMOVE)))
        {
            if (mMsgs.message == WM_QUIT)
            {
                mIsRunning = false;
                break;
            }
            TranslateMessage(&mMsgs);
            DispatchMessage(&mMsgs);
        }
        return mIsRunning;
    }

    void Window::appendWindowText(const std::string& text)
    {
        std::wstring windowText = mDescription.title;
        windowText += utility::toWideString(text);

        SetWindowText(mHandle, windowText.c_str());
    }

    Window::~Window()
    {
        DestroyWindow(mHandle);

        mHandle = NULL;
    }


    LRESULT CALLBACK Window::HandleMsgsSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
    {
        if (msg == WM_NCCREATE)
        {
            const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
            Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
            SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgs));

            pWnd->HandleMsgs(hWnd, msg, wParam, lParam);
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    LRESULT CALLBACK Window::HandleMsgs(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
    {
        Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
    }

    LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
    {
#if defined(DEBUG)
        //static WindowsMessageMap mm;
        //OutputDebugString(utility::toWideString(mm(msg, lParam, wParam)).c_str());
#endif
        static HMONITOR lastMonitor = nullptr;
        switch (msg)
        {
        case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }
		case WM_DISPLAYCHANGE:
        case WM_SIZE:
        {
			UINT clientWidth = LOWORD(lParam);
			UINT clientHeight = HIWORD(lParam);
            onResize(clientWidth, clientHeight);
        }
		case WM_WINDOWPOSCHANGED:
		{
			HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
			if (hMon != lastMonitor)
			{
				lastMonitor = hMon;
                onMonitorChanged();
			}
		}
		break;
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}
