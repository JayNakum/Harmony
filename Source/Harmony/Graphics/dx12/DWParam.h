#pragma once

#include "common.h"
#include "directx.h"

namespace dx12
{
    // A union for all the DWord params.
    // Prevents unnecessary function overloads.
    struct DWParam
    {
        DWParam(FLOAT value) : floatValue(value) {}
        DWParam(UINT value) : uintValue(value) {}
        DWParam(INT value) : intValue(value) {}

        void operator= (FLOAT value) { floatValue = value; }
        void operator= (UINT value) { uintValue = value; }
        void operator= (INT value) { intValue = value; }

        union
        {
            FLOAT floatValue;
            UINT uintValue;
            INT intValue;
        };
    };
}
