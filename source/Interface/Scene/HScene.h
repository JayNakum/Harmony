#pragma once

#include <optional>
#include <list>

namespace harmony
{
	// Forward declarations
	class Drawable;
}

namespace harmony
{
	namespace component
	{
		class Visual
		{
		public:
			struct Mesh
			{
				const float* vertices = nullptr;
				size_t verticesSize = 0;

				const uint16_t* indices = nullptr;
				size_t indicesSize = 0;
			};

			Mesh mesh;
		};
	}

	class Entity
	{
	public:
		struct Description
		{
			const char* label = "default entity";
			std::optional<component::Visual> visual;
		};

		virtual ~Entity() = default;

	public:
		virtual void initialize() = 0;
		virtual void deinitialize() = 0;
		virtual const void* getID() const = 0;
		virtual const Description& getDescription() const = 0;
	};

	class Scene
	{
	public:
		virtual ~Scene() = default;

		struct Description
		{
			const char* label = "default scene";
		};

	public:
		virtual void initialize() = 0;
		virtual void deinitialize() = 0;

		virtual const Description& getDescription() const = 0;

		virtual Entity* createEntity(const Entity::Description& description) = 0;

		virtual std::list<const Drawable*> getDrawables()
		{
			return std::list<const Drawable*>();
		}
	};
}
