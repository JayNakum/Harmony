#pragma once

#include "Scene/HScene.h"

#include <entt.hpp>

#include <unordered_map>

namespace ENTT
{
	class Entity : public harmony::Entity
	{
	private:
		friend class Scene;
		Entity(const harmony::Entity::Description& description, const entt::entity& entity);
	public:
		~Entity();

		const void* getID() const override;
		const harmony::Entity::Description& getDescription() const override;
		const harmony::Drawable* getDrawable() const override;
		
	private:
		void initialize() override;
		void deinitialize() override;

	private:
		harmony::Entity::Description mDescription;
		entt::entity mEntity;

		harmony::Drawable* mDrawable;

	};

	class Scene : public harmony::Scene
	{
	public:
		Scene(const harmony::Scene::Description& description);
		~Scene();

	public:
		const harmony::Scene::Description& getDescription() const override;

		harmony::Entity* createEntity(const harmony::Entity::Description& description) override;
		void addVisualComponent(const void* entityID, const harmony::component::Visual& component);

		void initialize() override;
		void deinitialize() override;

		std::list<harmony::Entity*> getVisualEntities() const override;

	private:
		harmony::Scene::Description mDescription;
		entt::registry mRegistry;

		std::unordered_map<entt::entity, Entity*> mEntities;
	};
}
