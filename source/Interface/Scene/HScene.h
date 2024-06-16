#pragma once

#include "HComponents.h"

#include <optional>
#include <list>

namespace harmony
{
	// Forward declarations
	class Drawable;
}

namespace harmony
{
	class Entity
	{
	public:
		friend class Scene;
		struct Description
		{
			const char* label = "default empty entity";
			
			std::optional<component::Transform> transform;
			std::optional<component::Visual> visual;
		};

		virtual ~Entity() = default;
		virtual const void* getID() const = 0;
		virtual const Description& getDescription() const = 0;

		virtual const Drawable* getDrawable() const = 0;
		
	private:
		virtual void initialize() = 0;
		virtual void deinitialize() = 0;
	};

	class Scene
	{
	public:
		virtual ~Scene() = default;

		struct Description
		{
			const char* label = "default empty scene";
		};

	public:
		virtual const Description& getDescription() const = 0;

		virtual Entity* createEntity(const Entity::Description& description) = 0;
		// virtual void addVisualComponent(const void* entityID, const harmony::component::Visual& component) = 0;

		virtual void initialize() = 0;
		virtual void deinitialize() = 0;

		virtual std::list<Entity*> getVisualEntities() const
		{
			return std::list<Entity*>();
		}
	};
}
