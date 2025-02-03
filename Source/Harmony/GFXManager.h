#pragma once

#include "GFXDevice.h"

namespace graphics
{
	/*
	* A singleton class that manages the entire graphics system.
	*/
	class Manager
	{
	private:
		Manager();

		static Manager* msInstance;
	public:
		static Manager* getInstance();

	public:
		inline const graphics::Device getDevice() const { return mDevice; }

	private:
		Device mDevice;
	};
}

#define GFXManager graphics::Manager::getInstance()
#define GFXDevice graphics::Manager::getInstance()->getDevice()