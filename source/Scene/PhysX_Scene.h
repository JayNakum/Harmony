#pragma once

#include "Scene/HScene.h"

#include <PxPhysicsAPI.h>

namespace physx
{
	class RigidBody : public harmony::Entity
	{
	private:
		friend class Scene;
		RigidBody(const harmony::Entity::Description& description, physx::PxRigidActor* const& actor);
	public:
		~RigidBody();

		const void* getID() const override;
		const harmony::Entity::Description& getDescription() const override;
		void setTransform(const harmony::component::Transform& transform) override;

	private:
		void initialize() override;
		void deinitialize() override;

	private:
		harmony::Entity::Description mDescription;
		physx::PxRigidActor* mActor;
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
		physx::PxScene* mScene;
	};
}