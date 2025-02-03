#pragma once

#include "common.h"
#include "win.h"

#include "GFXCommandList.h"

/*
* A union for all the DWord params.
* Prevents unnecessary function overloads.
*/
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
        /*
        * Command context is only ever meant to be created, initialized or reset via the Context Manager
        */
        CommandContext(CommandListType type);
        void reset();

    public:
        ~CommandContext();


    private:
        CommandListType mType;

    };

    /*
    * Context Manager manages the memory for creating and destroying Command Contexts
    * It has a pool of contexts and manages its allocation and deallocation.
    */
    class ContextPool
    {
    public:
        /*
        * Allocates a new context for the given command list type.
        * If an available context exists, it is reset and reused. 
        * Otherwise, a new one is created and initialized.
        */
        CommandContext* allocateContext(CommandListType type);

        /*
        * Puts the context in available queue for future use.
        * This doesn't delete or reset the context. The context is reset first when requested via allocateContext().
        */
        void freeContext(CommandContext* context);

        /*
        * Destroys all command contexts in the available queue and clears the context pool.
        */
        void destroyAllContexts();

    private:
        /*
        * The main context pool containing all the contexts of each type.
        */
        static std::vector<CommandContext*> msContextPool[CommandListType::CommandListTypeCount];

        /*
        * A queue of contexts of each type that are available for reuse.
        */
        static std::queue<CommandContext*> msAvailableContexts[CommandListType::CommandListTypeCount];

        /*
        * Prevents incorrect reading and writing of context pool and available queue.
        */
        static std::mutex msContextAllocationMutex;
    };
}
