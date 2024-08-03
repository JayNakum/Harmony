#pragma once

#include "Harmony.h"

#include "Helpers/HLog.h"

namespace harmony
{
	class Engine
	{
	public:
		void initialize();
		void update();
		void deinitialize();

		~Engine();
	};
}
