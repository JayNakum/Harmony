#pragma once

namespace harmony
{
	class Image
	{
	public:
		struct Description
		{
			const char* imagePath = "default empty image";
			void* memory = nullptr;
			size_t size = 0;
			unsigned int width = 0;
			unsigned int height = 0;
		};

		virtual ~Image() = default;

	public:
		virtual const void* getID() const = 0;
		virtual const Description& getDescription() const = 0;

		virtual void initialize() = 0;
		virtual void deinitialize() = 0;
	};

	class Sampler
	{
	public:
		enum Filter
		{
			eNEAREST,
			eLINEAR
		};

		struct Description
		{
			const char* label = "default empty sampler";
			Filter minFilter = Filter::eLINEAR;
			Filter magFilter = Filter::eLINEAR;
		};

		virtual ~Sampler() = default;

	public:
		virtual const void* getID() const = 0;
		virtual const Description& getDescription() const = 0;

		virtual void initialize() = 0;
		virtual void deinitialize() = 0;
	};
}
