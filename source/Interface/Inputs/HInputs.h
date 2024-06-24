#pragma once

namespace harmony
{
    class Inputs
    {
    public:
        enum Key
        {
            eINVALID_KEY,

            eKEY_SPACE,
            eKEY_APOSTROPHE,
            eKEY_COMMA,
            eKEY_MINUS,
            eKEY_PERIOD,
            eKEY_SLASH,
            eKEY_0,
            eKEY_1,
            eKEY_2,
            eKEY_3,
            eKEY_4,
            eKEY_5,
            eKEY_6,
            eKEY_7,
            eKEY_8,
            eKEY_9,
            eKEY_SEMICOLON,
            eKEY_EQUAL,
            eKEY_A,
            eKEY_B,
            eKEY_C,
            eKEY_D,
            eKEY_E,
            eKEY_F,
            eKEY_G,
            eKEY_H,
            eKEY_I,
            eKEY_J,
            eKEY_K,
            eKEY_L,
            eKEY_M,
            eKEY_N,
            eKEY_O,
            eKEY_P,
            eKEY_Q,
            eKEY_R,
            eKEY_S,
            eKEY_T,
            eKEY_U,
            eKEY_V,
            eKEY_W,
            eKEY_X,
            eKEY_Y,
            eKEY_Z,
            eKEY_LEFT_BRACKET,
            eKEY_BACKSLASH,
            eKEY_RIGHT_BRACKET,
            eKEY_GRAVE_ACCENT,
            eKEY_WORLD_1,
            eKEY_WORLD_2,
            eKEY_ESCAPE,
            eKEY_ENTER,
            eKEY_TAB,
            eKEY_BACKSPACE,
            eKEY_INSERT,
            eKEY_DELETE,
            eKEY_ARROW_RIGHT,
            eKEY_ARROW_LEFT,
            eKEY_ARROW_DOWN,
            eKEY_ARROW_UP,
            eKEY_PAGE_UP,
            eKEY_PAGE_DOWN,
            eKEY_HOME,
            eKEY_END,
            eKEY_CAPS_LOCK,
            eKEY_SCROLL_LOCK,
            eKEY_NUM_LOCK,
            eKEY_PRINT_SCREEN,
            eKEY_PAUSE,
            eKEY_F1,
            eKEY_F2,
            eKEY_F3,
            eKEY_F4,
            eKEY_F5,
            eKEY_F6,
            eKEY_F7,
            eKEY_F8,
            eKEY_F9,
            eKEY_F10,
            eKEY_F11,
            eKEY_F12,
            eKEY_F13,
            eKEY_F14,
            eKEY_F15,
            eKEY_F16,
            eKEY_F17,
            eKEY_F18,
            eKEY_F19,
            eKEY_F20,
            eKEY_F21,
            eKEY_F22,
            eKEY_F23,
            eKEY_F24,
            eKEY_F25,
            eKEY_KP_0,
            eKEY_KP_1,
            eKEY_KP_2,
            eKEY_KP_3,
            eKEY_KP_4,
            eKEY_KP_5,
            eKEY_KP_6,
            eKEY_KP_7,
            eKEY_KP_8,
            eKEY_KP_9,
            eKEY_KP_DECIMAL,
            eKEY_KP_DIVIDE,
            eKEY_KP_MULTIPLY,
            eKEY_KP_SUBTRACT,
            eKEY_KP_ADD,
            eKEY_KP_ENTER,
            eKEY_KP_EQUAL,
            eKEY_LEFT_SHIFT,
            eKEY_LEFT_CONTROL,
            eKEY_LEFT_ALT,
            eKEY_LEFT_SUPER,
            eKEY_RIGHT_SHIFT,
            eKEY_RIGHT_CONTROL,
            eKEY_RIGHT_ALT,
            eKEY_RIGHT_SUPER,
            eKEY_MENU,
            
            eMOUSE_BUTTON_LEFT,
            eMOUSE_BUTTON_RIGHT,
            eMOUSE_BUTTON_MIDDLE,
            
            eMOUSE_RIGHT,
            eMOUSE_LEFT,
            eMOUSE_DOWN,
            eMOUSE_UP,
            
            eMAX_KEYS
        };

        enum Event
        {
            eJUST_PRESSED,
            ePRESSED,
            eJUST_RELEASED,
            eRELEASED
        };

        enum Action
        {
            eINVALID_ACTION,
            eEXIT,
            eMOVE_FORWARD,
            eMOVE_BACKWARD,
            eMOVE_RIGHT,
            eMOVE_LEFT,
            eMOVE_DOWN,
            eMOVE_UP,

            eACTIVATE_CAMERA,
            eYAW_RIGHT,
            eYAW_LEFT,
            ePITCH_DOWN,
            ePITCH_UP,
            eROLL_RIGHT,
            eROLL_LEFT,

            eMAX_ACTIONS
        };

        enum Type
        {
            eKEYBOARD,
            eCONTROLLER
        };

        struct Description
        {
            Type type = Type::eCONTROLLER;
        };

    public:
        virtual ~Inputs() = default;

        virtual const Description& getDescription() const = 0;

        virtual void initialize() = 0;
        virtual void update() = 0;
        virtual void reset() = 0;
        virtual void deinitialize() = 0;

        virtual void keyCallback(const Key key, const Event e) = 0;
        virtual void mouseCallback(const float x, const float y, const float dx = 0.0f, const float dy = 0.0f) = 0;

        virtual bool actionOccured(const Action action, const Event e) const = 0;
    };
}
