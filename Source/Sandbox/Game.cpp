#include "Entrypoint.h"

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

        ~Application()
        {

        }
    };
}

CREATE_APPLICATION(sandbox::Application);
