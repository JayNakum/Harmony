#pragma once

#include <cstdint>

namespace harmony
{
	class Image;
	class Sampler;
}

namespace harmony
{
	namespace component
	{
		class Transform
		{
		public:
			const char* label = "default empty transform component";

			float scale[3];
			float rotation[3];
			float position[3];
		};

		class Visual
		{
		public:
			const char* label = "default empty visual component";

			struct Texture
			{
				const char* label = "default texture";
				Image* image = nullptr;
				Sampler* sampler = nullptr;
			};

			struct Mesh
			{
				const float* vertices = nullptr;
				size_t verticesSize = 0;
				size_t verticesStride = 0;

				const uint16_t* indices = nullptr;
				size_t indicesSize = 0;
			};

			Mesh mesh;
			Texture texture1;
		};
	}
}