#pragma once

namespace harmony
{
	class Window
	{
	public:
		struct Description
		{
			const char*		title			= "Harmony";
			unsigned int	width			= 0;
			unsigned int	height			= 0;
			unsigned int	sampleCount		= 0;
			bool			isFullScreen	= false;
		};
		virtual ~Window() = default;

	public:
		virtual const Description& getDescription() const = 0;

		virtual void* getHandle() = 0;

		virtual bool isRunning() const = 0;

		virtual void initialize() = 0;
		virtual void deinitialize() = 0;
		virtual void update() = 0;
	};
}
