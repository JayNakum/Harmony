#pragma once

namespace harmony
{
	class RenderTarget;
}

namespace harmony
{
	class Swapchain
	{
	public:
		struct Description
		{
			const char* label = "default swapchain";
			unsigned int width = 0;
			unsigned int height = 0;
		};

		virtual const void* getID() const = 0;
		virtual const Description& getDescription() const = 0;

		virtual RenderTarget* getCurrentRenderTarget() const = 0;

		virtual void initialize() = 0;
		virtual void prepare() = 0;
		virtual void resize(unsigned int width, unsigned int height) = 0;
		virtual void present() = 0;
		virtual void deinitialize() = 0;
	};
}
