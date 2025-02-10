#pragma once

#include "common.h"
#include "win.h"

#include "GFXCommandList.h"


// A union for all the DWord params.
// Prevents unnecessary function overloads.
struct DWParam
{
    DWParam(FLOAT f) : floatValue(f) {}
    DWParam(UINT u) : uintValue(u) {}
    DWParam(INT i) : intValue(i) {}

    void operator= (FLOAT f) { floatValue = f; }
    void operator= (UINT u) { uintValue = u; }
    void operator= (INT i) { intValue = i; }

    union
    {
        FLOAT floatValue;
        UINT uintValue;
        INT intValue;
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
    class CommandContext : NonCopyable
    {
        friend class ContextPool;
    private:
        // Command context is only ever meant to be created, initialized or reset via the Context Manager
        CommandContext(CommandListType type);

        void reset();

    public:
        ~CommandContext();


    private:
        CommandListType mType;

    };


}
