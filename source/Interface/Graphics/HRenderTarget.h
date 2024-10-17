#pragma once

namespace harmony
{
	class RenderTarget
	{
	public:
		struct Action
		{
			struct Attachment
			{
				float value[4];
			};

			Attachment colors;
		};

		struct Description
		{
			const char* label = "default render target";
		};

		virtual const Description& getDescription() const = 0;
		virtual const void* getID() const = 0;
		virtual void initialize() = 0;
		virtual void bind(const Action& action) = 0;
		virtual void unbind() = 0;
		virtual void deinitialize() = 0;
	};
}
