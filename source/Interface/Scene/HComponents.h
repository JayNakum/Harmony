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
		class Physics
		{
		public:
			enum Shape
			{
				eBOX,
				eSPHERE,
				eCAPSULE
			};

			enum Type
			{
				eSTATIC,
				eKINEMATIC,
				eDYNAMIC
			};

			Type type;
			Shape shape;
			bool gravity = true;
			float mass = 1.0f;
			float staticFriction = 0.5f;
			float dynamicFriction = 0.5f;
			float restitution = 0.5f;
			float linearDamping = 0.0f;
			float angularDamping = 0.05f;
		};

		class Transform
		{
		public:
			const char* label = "default empty transform component";

			float scale[3] = { 1.0f, 1.0f, 1.0f };
			float rotation[3] = { 0.0f, 0.0f, 0.0f };
			float position[3] = { 0.0f, 0.0f, 0.0f };
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
				unsigned int numElements = 0;

				const uint16_t* indices = nullptr;
				size_t indicesSize = 0;
			};

			Mesh mesh;
			Texture texture1;
		};
	}
}