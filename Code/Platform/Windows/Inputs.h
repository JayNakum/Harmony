#pragma once

#include "common.h"
#include "win.h"

#include "InputSystem.h"

namespace windows
{
    class Inputs : public core::Inputs
    {
    public:
        Inputs(const HWND windowHandle);
        ~Inputs();

        // Input interface
        bool isAnyPressed() const override;
        bool isPressed(Digital di) const override;
        bool isFirstPressed(Digital di) const override;
        bool isReleased(Digital di) const override;
        bool isFirstReleased(Digital di) const override;
        float getDurationPressed(Digital di) const override;
        float getAnalogInput(Analog ai) const override;
        float getTimeCorrectedAnalogInput(Analog ai) const override;
        void update(float frameDelta) override;

    private:
        // Convenient constants
        static constexpr int NumDigitalInputs = static_cast<int>(Digital::Mouse7) + 1;
        static constexpr int NumAnalogInputs = static_cast<int>(Analog::Analog_MouseScroll) + 1;
        static constexpr int NumKeys = static_cast<int>(Digital::Key_apps) + 1;

        // State arrays
        bool   mButtons[2][NumDigitalInputs];
        float  mHoldDuration[NumDigitalInputs];
        float  mAnalogs[NumAnalogInputs];
        float  mAnalogsTC[NumAnalogInputs];

        // DirectInput / XInput handles
        const HWND mWindowHandle;
        IDirectInput8A* mDInput = nullptr;
        IDirectInputDevice8A* mKeyboard = nullptr;
        IDirectInputDevice8A* mMouse = nullptr;
        DIMOUSESTATE2          mMouseState{};
        unsigned char          mKeybuffer[256]{};
        unsigned char          mDxKeyMapping[NumKeys]{};

        // Initialization / shutdown
        void buildKeyMapping();
        void zeroInputs();
        void kbmInitialize();
        void kbmShutdown();

        // Polling helpers
        void pollGamepad();
        void pollKeyboardAndMouse();

        inline float filterAnalogInput(int val, int deadZone) const;
    };
}
