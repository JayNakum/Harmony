#pragma once

#include "Scene/HScene.h"

#include <entt.hpp>

namespace ENTT
{
	class Entity : public harmony::Entity
	{
	public:
		Entity(const harmony::Entity::Description& description, entt::registry& registry);
		~Entity();

	public:
		void initialize() override;
		void deinitialize() override;

		const void* getID() const override;
		const harmony::Entity::Description& getDescription() const override;
		
	private:
		harmony::Entity::Description mDescription;
		entt::entity mEntity;
		entt::registry& mRegistry;
	};

	class Scene : public harmony::Scene
	{
	public:
		Scene(const harmony::Scene::Description& description);
		~Scene();

	public:
		void initialize() override;
		void deinitialize() override;

		const harmony::Scene::Description& getDescription() const override;

		harmony::Entity* createEntity(const harmony::Entity::Description& description) override;

	private:
		harmony::Scene::Description mDescription;
		entt::registry mRegistry;
	};
}