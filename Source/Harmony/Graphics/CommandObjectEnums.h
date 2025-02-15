#pragma once

namespace graphics
{
    enum CommandListType
    {
        Direct,
        Compute,
        Copy,
        // TODO: add support for Bundle type command lists
    
        CommandListTypeCount
    };
}
