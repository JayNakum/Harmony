#pragma once

#include "Scene/HScene.h"

#include <PxConfig.h>
#include <PxPhysicsAPI.h>

namespace physx
{
	class RigidBody : public harmony::Entity
	{
	private:
		friend class Scene;
		RigidBody(const harmony::Entity::Description& description);
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
		physx::PxMaterial* mMaterial;
	};

	class Scene : public harmony::Scene
	{
	public:
		Scene(const harmony::Scene::Description& description);
		~Scene();

	public:
		const harmony::Scene::Description& getDescription() const override;

		harmony::Entity* createEntity(const harmony::Entity::Description& description) override;
		
		void initialize() override;
		void update() override;
		void deinitialize() override;

	private:
		harmony::Scene::Description mDescription;
		physx::PxScene* mScene;
	};
}
