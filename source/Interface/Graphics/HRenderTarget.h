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

		virtual const void* getID() const = 0;
		virtual void initialize() = 0;
		virtual void bind(const Action& action) = 0;
		virtual void deinitialize() = 0;
	};
}