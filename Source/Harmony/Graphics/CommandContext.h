#pragma once

#include "common.h"

#include "CommandObject.h"

// A union for all the DWord params.
// Prevents unnecessary function overloads.
struct DWParam
{
    DWParam(float value) : floatValue(value) {}
    DWParam(uint32_t value) : uintValue(value) {}
    DWParam(int value) : intValue(value) {}

    void operator= (float value) { floatValue = value; }
    void operator= (uint32_t value) { uintValue = value; }
    void operator= (int value) { intValue = value; }

    union
    {
        float floatValue;
        uint32_t uintValue;
        int intValue;
    };
};

namespace graphics
{
    /*
    * Deletes the copy constructor and = operator.
    * Used to make the code clean.
    */
    struct NonCopyable
    {
        NonCopyable() = default;
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;
    };

    /*
    * Contains the state and resources needed to record and execute commands on the GPU.
    * This parent class is inherited by GraphicsContext class and // TODO: ComputeContext class.
    */
    class CommandContext : NonCopyable, public CommandObject
    {
    public:
        // Command context is only ever meant to be created, initialized or reset via the Context Manager
        CommandContext(CommandListType type);

        void reset();

    public:
        ~CommandContext();
    };
}
