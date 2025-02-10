#pragma once

#include "GFXDevice.h"

namespace graphics
{
    /*
    * A singleton class that manages the entire graphics system.
    * All the other systems will access anything inside the graphics project via Manager.
    * // TODO: Create a separate visual studio project for GFX* files.
    */
    class Manager
    {
    private:
        Manager();

        static Manager* msInstance;
    public:
        static Manager* getInstance();

    public:
        inline const graphics::dx12::Device getDevice() const { return mDevice; }

    private:
        dx12::Device mDevice;
    };
}

/*
* Bunch of inline helper functions.
* Keeping these outside of any namespace with Gfx prefix to make the code a bit more clean.
*/

inline const graphics::Manager* GfxManager()
{
    return graphics::Manager::getInstance();
}

inline const graphics::dx12::Device GfxDevice()
{
    return graphics::Manager::getInstance()->getDevice();
}
