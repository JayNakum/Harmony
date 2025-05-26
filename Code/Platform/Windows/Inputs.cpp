#include "Inputs.h"

#include "Utility.h"
#include "Log.h"

namespace windows
{
    Inputs::Inputs(const HWND windowHandle)
        : mWindowHandle(windowHandle)
    {
        std::memset(mButtons, 0, sizeof(mButtons));
        std::memset(mHoldDuration, 0, sizeof(mHoldDuration));
        std::memset(mAnalogs, 0, sizeof(mAnalogs));
        std::memset(mAnalogsTC, 0, sizeof(mAnalogsTC));
        kbmInitialize();
    }

    Inputs::~Inputs()
    {
        kbmShutdown();
    }

    bool Inputs::isAnyPressed() const
    {
        for (int i = 0; i < NumDigitalInputs; ++i)
        {
            if (mButtons[0][i])
            {
                return true;
            }
        }
        return false;
    }

    bool Inputs::isPressed(Digital di) const
    {
        return mButtons[0][enum_cast(di)];
    }

    bool Inputs::isFirstPressed(Digital di) const
    {
        int idx = enum_cast(di);
        return mButtons[0][idx] && !mButtons[1][idx];
    }

    bool Inputs::isReleased(Digital di) const
    {
        return !mButtons[0][enum_cast(di)];
    }

    bool Inputs::isFirstReleased(Digital di) const
    {
        int idx = enum_cast(di);
        return !mButtons[0][idx] && mButtons[1][idx];
    }

    float Inputs::getDurationPressed(Digital di) const
    {
        return mHoldDuration[enum_cast(di)];
    }

    float Inputs::getAnalogInput(Analog ai) const
    {
        return mAnalogs[enum_cast(ai)];
    }

    float Inputs::getTimeCorrectedAnalogInput(Analog ai) const
    {
        return mAnalogsTC[enum_cast(ai)];
    }

    void Inputs::update(float frameDelta)
    {
        // shift previous state → index 1
        std::memcpy(mButtons[1], mButtons[0], sizeof(mButtons[0]));
        std::memset(mButtons[0], 0, sizeof(mButtons[0]));
        std::memset(mAnalogs, 0, sizeof(mAnalogs));

        // poll gamepad & KBM
        pollGamepad();
        pollKeyboardAndMouse();

        // update hold durations
        for (int i = 0; i < NumDigitalInputs; ++i)
        {
            if (mButtons[0][i])
            {
                mHoldDuration[i] = mButtons[1][i] ? (mHoldDuration[i] + frameDelta) : 0.0f;
            }
        }

        // compute time‐corrected analogs
        for (int i = 0; i < NumAnalogInputs; ++i)
        {
            mAnalogsTC[i] = mAnalogs[i] * frameDelta;
        }
    }

    void Inputs::buildKeyMapping()
    {
        mDxKeyMapping[enum_cast(Digital::Key_escape)] = 1;
        mDxKeyMapping[enum_cast(Digital::Key_1)] = 2;
        mDxKeyMapping[enum_cast(Digital::Key_2)] = 3;
        mDxKeyMapping[enum_cast(Digital::Key_3)] = 4;
        mDxKeyMapping[enum_cast(Digital::Key_4)] = 5;
        mDxKeyMapping[enum_cast(Digital::Key_5)] = 6;
        mDxKeyMapping[enum_cast(Digital::Key_6)] = 7;
        mDxKeyMapping[enum_cast(Digital::Key_7)] = 8;
        mDxKeyMapping[enum_cast(Digital::Key_8)] = 9;
        mDxKeyMapping[enum_cast(Digital::Key_9)] = 10;
        mDxKeyMapping[enum_cast(Digital::Key_0)] = 11;
        mDxKeyMapping[enum_cast(Digital::Key_minus)] = 12;
        mDxKeyMapping[enum_cast(Digital::Key_equals)] = 13;
        mDxKeyMapping[enum_cast(Digital::Key_back)] = 14;
        mDxKeyMapping[enum_cast(Digital::Key_tab)] = 15;
        mDxKeyMapping[enum_cast(Digital::Key_q)] = 16;
        mDxKeyMapping[enum_cast(Digital::Key_w)] = 17;
        mDxKeyMapping[enum_cast(Digital::Key_e)] = 18;
        mDxKeyMapping[enum_cast(Digital::Key_r)] = 19;
        mDxKeyMapping[enum_cast(Digital::Key_t)] = 20;
        mDxKeyMapping[enum_cast(Digital::Key_y)] = 21;
        mDxKeyMapping[enum_cast(Digital::Key_u)] = 22;
        mDxKeyMapping[enum_cast(Digital::Key_i)] = 23;
        mDxKeyMapping[enum_cast(Digital::Key_o)] = 24;
        mDxKeyMapping[enum_cast(Digital::Key_p)] = 25;
        mDxKeyMapping[enum_cast(Digital::Key_lbracket)] = 26;
        mDxKeyMapping[enum_cast(Digital::Key_rbracket)] = 27;
        mDxKeyMapping[enum_cast(Digital::Key_return)] = 28;
        mDxKeyMapping[enum_cast(Digital::Key_lcontrol)] = 29;
        mDxKeyMapping[enum_cast(Digital::Key_a)] = 30;
        mDxKeyMapping[enum_cast(Digital::Key_s)] = 31;
        mDxKeyMapping[enum_cast(Digital::Key_d)] = 32;
        mDxKeyMapping[enum_cast(Digital::Key_f)] = 33;
        mDxKeyMapping[enum_cast(Digital::Key_g)] = 34;
        mDxKeyMapping[enum_cast(Digital::Key_h)] = 35;
        mDxKeyMapping[enum_cast(Digital::Key_j)] = 36;
        mDxKeyMapping[enum_cast(Digital::Key_k)] = 37;
        mDxKeyMapping[enum_cast(Digital::Key_l)] = 38;
        mDxKeyMapping[enum_cast(Digital::Key_semicolon)] = 39;
        mDxKeyMapping[enum_cast(Digital::Key_apostrophe)] = 40;
        mDxKeyMapping[enum_cast(Digital::Key_grave)] = 41;
        mDxKeyMapping[enum_cast(Digital::Key_lshift)] = 42;
        mDxKeyMapping[enum_cast(Digital::Key_backslash)] = 43;
        mDxKeyMapping[enum_cast(Digital::Key_z)] = 44;
        mDxKeyMapping[enum_cast(Digital::Key_x)] = 45;
        mDxKeyMapping[enum_cast(Digital::Key_c)] = 46;
        mDxKeyMapping[enum_cast(Digital::Key_v)] = 47;
        mDxKeyMapping[enum_cast(Digital::Key_b)] = 48;
        mDxKeyMapping[enum_cast(Digital::Key_n)] = 49;
        mDxKeyMapping[enum_cast(Digital::Key_m)] = 50;
        mDxKeyMapping[enum_cast(Digital::Key_comma)] = 51;
        mDxKeyMapping[enum_cast(Digital::Key_period)] = 52;
        mDxKeyMapping[enum_cast(Digital::Key_slash)] = 53;
        mDxKeyMapping[enum_cast(Digital::Key_rshift)] = 54;
        mDxKeyMapping[enum_cast(Digital::Key_multiply)] = 55;
        mDxKeyMapping[enum_cast(Digital::Key_lalt)] = 56;
        mDxKeyMapping[enum_cast(Digital::Key_space)] = 57;
        mDxKeyMapping[enum_cast(Digital::Key_capital)] = 58;
        mDxKeyMapping[enum_cast(Digital::Key_f1)] = 59;
        mDxKeyMapping[enum_cast(Digital::Key_f2)] = 60;
        mDxKeyMapping[enum_cast(Digital::Key_f3)] = 61;
        mDxKeyMapping[enum_cast(Digital::Key_f4)] = 62;
        mDxKeyMapping[enum_cast(Digital::Key_f5)] = 63;
        mDxKeyMapping[enum_cast(Digital::Key_f6)] = 64;
        mDxKeyMapping[enum_cast(Digital::Key_f7)] = 65;
        mDxKeyMapping[enum_cast(Digital::Key_f8)] = 66;
        mDxKeyMapping[enum_cast(Digital::Key_f9)] = 67;
        mDxKeyMapping[enum_cast(Digital::Key_f10)] = 68;
        mDxKeyMapping[enum_cast(Digital::Key_numlock)] = 69;
        mDxKeyMapping[enum_cast(Digital::Key_scroll)] = 70;
        mDxKeyMapping[enum_cast(Digital::Key_numpad7)] = 71;
        mDxKeyMapping[enum_cast(Digital::Key_numpad8)] = 72;
        mDxKeyMapping[enum_cast(Digital::Key_numpad9)] = 73;
        mDxKeyMapping[enum_cast(Digital::Key_subtract)] = 74;
        mDxKeyMapping[enum_cast(Digital::Key_numpad4)] = 75;
        mDxKeyMapping[enum_cast(Digital::Key_numpad5)] = 76;
        mDxKeyMapping[enum_cast(Digital::Key_numpad6)] = 77;
        mDxKeyMapping[enum_cast(Digital::Key_add)] = 78;
        mDxKeyMapping[enum_cast(Digital::Key_numpad1)] = 79;
        mDxKeyMapping[enum_cast(Digital::Key_numpad2)] = 80;
        mDxKeyMapping[enum_cast(Digital::Key_numpad3)] = 81;
        mDxKeyMapping[enum_cast(Digital::Key_numpad0)] = 82;
        mDxKeyMapping[enum_cast(Digital::Key_decimal)] = 83;
        mDxKeyMapping[enum_cast(Digital::Key_f11)] = 87;
        mDxKeyMapping[enum_cast(Digital::Key_f12)] = 88;
        mDxKeyMapping[enum_cast(Digital::Key_numpadenter)] = 156;
        mDxKeyMapping[enum_cast(Digital::Key_rcontrol)] = 157;
        mDxKeyMapping[enum_cast(Digital::Key_divide)] = 181;
        mDxKeyMapping[enum_cast(Digital::Key_sysrq)] = 183;
        mDxKeyMapping[enum_cast(Digital::Key_ralt)] = 184;
        mDxKeyMapping[enum_cast(Digital::Key_pause)] = 197;
        mDxKeyMapping[enum_cast(Digital::Key_home)] = 199;
        mDxKeyMapping[enum_cast(Digital::Key_up)] = 200;
        mDxKeyMapping[enum_cast(Digital::Key_pgup)] = 201;
        mDxKeyMapping[enum_cast(Digital::Key_left)] = 203;
        mDxKeyMapping[enum_cast(Digital::Key_right)] = 205;
        mDxKeyMapping[enum_cast(Digital::Key_end)] = 207;
        mDxKeyMapping[enum_cast(Digital::Key_down)] = 208;
        mDxKeyMapping[enum_cast(Digital::Key_pgdn)] = 209;
        mDxKeyMapping[enum_cast(Digital::Key_insert)] = 210;
        mDxKeyMapping[enum_cast(Digital::Key_delete)] = 211;
        mDxKeyMapping[enum_cast(Digital::Key_lwin)] = 219;
        mDxKeyMapping[enum_cast(Digital::Key_rwin)] = 220;
        mDxKeyMapping[enum_cast(Digital::Key_apps)] = 221;
    }

    void Inputs::zeroInputs()
    {
        std::memset(&mMouseState, 0, sizeof(mMouseState));
        std::memset(mKeybuffer, 0, sizeof(mKeybuffer));
    }

    void Inputs::kbmInitialize()
    {
        buildKeyMapping();

        HRESULT hResult = S_OK;

        hResult = DirectInput8Create(GetModuleHandle(nullptr),
                                     DIRECTINPUT_VERSION,
                                     IID_IDirectInput8A,
                                     (void**)&mDInput, nullptr);

        HASSERT(SUCCEEDED(hResult), "DirectInput8 initialization failed.\n%s", utility::translateErrorCode(hResult));

        // Keyboard
        hResult = mDInput->CreateDevice(GUID_SysKeyboard, &mKeyboard, nullptr);
        HASSERT(SUCCEEDED(hResult), "Keyboard CreateDevice failed.\n%s", utility::translateErrorCode(hResult));

        hResult = mKeyboard->SetDataFormat(&c_dfDIKeyboard);
        HASSERT(SUCCEEDED(hResult), "Keyboard SetDataFormat failed.\n%s", utility::translateErrorCode(hResult));

        hResult = mKeyboard->SetCooperativeLevel(mWindowHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
        HASSERT(SUCCEEDED(hResult), "Keyboard SetCooperativeLevel failed.\n%s", utility::translateErrorCode(hResult));

        DIPROPDWORD dipdw{};
        dipdw.diph.dwSize = sizeof(dipdw);
        dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
        dipdw.diph.dwObj = 0;
        dipdw.diph.dwHow = DIPH_DEVICE;
        dipdw.dwData = 10;
        mKeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

        // Mouse
        hResult = mDInput->CreateDevice(GUID_SysMouse, &mMouse, nullptr);
        HASSERT(SUCCEEDED(hResult), "Mouse CreateDevice failed.\n%s", utility::translateErrorCode(hResult));

        hResult = mMouse->SetDataFormat(&c_dfDIMouse2);
        HASSERT(SUCCEEDED(hResult), "Mouse SetDataFormat failed.\n%s", utility::translateErrorCode(hResult));

        hResult = mMouse->SetCooperativeLevel(mWindowHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
        HASSERT(SUCCEEDED(hResult), "Mouse SetCooperativeLevel failed.\n%s", utility::translateErrorCode(hResult));

        zeroInputs();
    }

    void Inputs::kbmShutdown()
    {
        if (mKeyboard)
        {
            mKeyboard->Unacquire();
            mKeyboard->Release();
            mKeyboard = nullptr;
        }
        if (mMouse)
        {
            mMouse->Unacquire();
            mMouse->Release();
            mMouse = nullptr;
        }
        if (mDInput)
        {
            mDInput->Release();
            mDInput = nullptr;
        }
    }

    void Inputs::pollGamepad()
    {
        XINPUT_STATE state = {};
        if (ERROR_SUCCESS == XInputGetState(0, &state))
        {
            auto BUTTON = [&](Digital d, WORD mask)
                {
                    mButtons[0][enum_cast(d)] = (state.Gamepad.wButtons & mask) != 0;
                };
            BUTTON(Digital::DPad_Up, XINPUT_GAMEPAD_DPAD_UP);
            BUTTON(Digital::DPad_Down, XINPUT_GAMEPAD_DPAD_DOWN);
            BUTTON(Digital::DPad_Left, XINPUT_GAMEPAD_DPAD_LEFT);
            BUTTON(Digital::DPad_Right, XINPUT_GAMEPAD_DPAD_RIGHT);
            BUTTON(Digital::DPad_StartButton, XINPUT_GAMEPAD_START);
            BUTTON(Digital::DPad_BackButton, XINPUT_GAMEPAD_BACK);
            BUTTON(Digital::DPad_L3, XINPUT_GAMEPAD_LEFT_THUMB);
            BUTTON(Digital::DPad_R3, XINPUT_GAMEPAD_RIGHT_THUMB);
            BUTTON(Digital::DPad_L1, XINPUT_GAMEPAD_LEFT_SHOULDER);
            BUTTON(Digital::DPad_R1, XINPUT_GAMEPAD_RIGHT_SHOULDER);
            BUTTON(Digital::DPad_AButton, XINPUT_GAMEPAD_A);
            BUTTON(Digital::DPad_BButton, XINPUT_GAMEPAD_B);
            BUTTON(Digital::DPad_XButton, XINPUT_GAMEPAD_X);
            BUTTON(Digital::DPad_YButton, XINPUT_GAMEPAD_Y);

            mAnalogs[enum_cast(Analog::Analog_L2)] = state.Gamepad.bLeftTrigger / 255.0f;
            mAnalogs[enum_cast(Analog::Analog_R2)] = state.Gamepad.bRightTrigger / 255.0f;
            mAnalogs[enum_cast(Analog::Analog_LeftStickX)] = filterAnalogInput(state.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
            mAnalogs[enum_cast(Analog::Analog_LeftStickY)] = filterAnalogInput(state.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
            mAnalogs[enum_cast(Analog::Analog_RightStickX)] = filterAnalogInput(state.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
            mAnalogs[enum_cast(Analog::Analog_RightStickY)] = filterAnalogInput(state.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        }
    }

    void Inputs::pollKeyboardAndMouse()
    {
        // Acquire & read
        if (GetForegroundWindow() == mWindowHandle && IsWindowVisible(mWindowHandle))
        {
            mMouse->Acquire();
            mMouse->GetDeviceState(sizeof(mMouseState), &mMouseState);
            mKeyboard->Acquire();
            mKeyboard->GetDeviceState(sizeof(mKeybuffer), mKeybuffer);
        }
        else
        {
            zeroInputs();
        }

        // Map keyboard
        for (int i = 0; i < NumKeys; ++i)
        {
            mButtons[0][i] = (mKeybuffer[mDxKeyMapping[i]] & 0x80) != 0;

        }

        // Mouse buttons
        for (int i = 0; i < 8; ++i)
        {
            if (mMouseState.rgbButtons[i] > 0)
            {
                mButtons[0][enum_cast(Digital::Mouse0) + i] = true;
            }
        }

        // Mouse motion
        mAnalogs[enum_cast(Analog::Analog_MouseX)] = mMouseState.lX * 0.0018f;
        mAnalogs[enum_cast(Analog::Analog_MouseY)] = mMouseState.lY * -0.0018f;
        mAnalogs[enum_cast(Analog::Analog_MouseScroll)] = (mMouseState.lZ > 0 ? 1.0f : (mMouseState.lZ < 0 ? -1.0f : 0.0f));
    }

    inline float Inputs::filterAnalogInput(int val, int deadZone) const
    {
        if (val < 0)
        {
            return (val > -deadZone) ? 0.0f : float(val + deadZone) / float(32768 - deadZone);
        }
        else
        {
            return (val < deadZone) ? 0.0f : float(val - deadZone) / float(32767 - deadZone);
        }
    }
}
