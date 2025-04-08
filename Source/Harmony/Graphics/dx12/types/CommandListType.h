#pragma once

#include "common.h"
#include "directx.h"

#include "CommandObjectEnums.h"

namespace graphics
{
    namespace dx12
    {
        inline static const D3D12_COMMAND_LIST_TYPE toD3DCommandListType(const CommandListType type)
        {
            switch (type)
            {
            case CommandListType::Direct:
                return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
            case CommandListType::Compute:
                return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE;
            case CommandListType::Copy:
                return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COPY;
            }
        }

    }
}
