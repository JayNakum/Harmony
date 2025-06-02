#pragma once

#include "common.h"
#include "directx.h"

namespace graphics
{
    namespace dx12
    {
        // A union for all the DWord params.
        // Prevents unnecessary function overloads.
        struct DWParam
        {
            DWParam()
                : floatValue(0.0f), uintValue(0), intValue(0) {}

            DWParam(FLOAT value) : floatValue(value) {}
            DWParam(UINT value) : uintValue(value) {}
            DWParam(INT value) : intValue(value) {}

            DWParam(const DWParam& copy)
                : floatValue(copy.floatValue), uintValue(copy.uintValue), intValue(copy.intValue) {}

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
}
