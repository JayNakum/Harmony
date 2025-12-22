#pragma once

#include "win.h"

#include <directx/d3d12.h>
#include <dxgi1_6.h>
#include <directx/d3dx12.h>
#ifdef HDEBUG
#include <dxgidebug.h>
#endif

#include <d3dcompiler.h>

inline static void setD3DName(ID3D12Object* object, const wchar_t* name)
{
#if defined(HRELEASE)
    (void)name;
    (void)object;
#else
    object->SetName(name);
#endif
}
