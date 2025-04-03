#pragma once

#include "common.h"
#include "directx.h"

#include "CommandObjectEnums.h"

namespace graphics
{
    namespace dx12
    {
        static const std::unordered_map<graphics::CommandListType, D3D12_COMMAND_LIST_TYPE> commandListType = {
            { graphics::CommandListType::Direct, D3D12_COMMAND_LIST_TYPE_DIRECT },
            { graphics::CommandListType::Compute, D3D12_COMMAND_LIST_TYPE_COMPUTE },
            { graphics::CommandListType::Copy, D3D12_COMMAND_LIST_TYPE_COPY }
        };
    }
}
