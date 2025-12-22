#include "common.h"

#include "File.h"
#include "memory/BlockAllocator.h"
#include "memory/LinearAllocator.h"
#include "memory/RingAllocator.h"
#include "memory/SuballocationAllocator.h"

#include "Entrypoint.h"
#include "Window.h"
#include "Engine.h"

namespace sandbox
{
    class Application : public harmony::Application
    {
    public:
        Application()
        {
        }

        void initialize() override
        {

        }

        void run() override
        {
        }

        void deinitialize() override
        {
        }

        ~Application() override
        {
        }

    private:
    };
}

CREATE_APPLICATION(sandbox::Application);
