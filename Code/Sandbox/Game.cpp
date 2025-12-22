#include "common.h"

#include "Entrypoint.h"
#include "Window.h"
#include "Inputs.h"

#include "CommandAllocatorPool.h"
#include "CommandContext.h"
#include "CommandContextPool.h"
#include "CommandObject.h"
#include "CommandObjectEnums.h"
#include "DescriptorHeapEnums.h"
#include "DescriptorHeapWithAllocator.h"
#include "Format.h"
#include "GPUResourceEnums.h"
#include "HeapEnums.h"
#include "HeapWithAllocator.h"
#include "PipelineStateEnums.h"
#include "RootSignatureEnums.h"
#include "ShaderInput.h"

namespace sandbox
{
    class Application : public harmony::Application
    {
    public:
        Application()
            : mWindow({ L"Sandbox", 1280, 720 })
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
        windows::Window mWindow;
    };
}

CREATE_APPLICATION(sandbox::Application);
