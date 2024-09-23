#pragma once

#include "Scene/HScene.h"

#include "box2d/box2d.h"

#include <list>

namespace box2d
{
	class RigidBody : public harmony::Entity
	{
	private:
		friend class Scene;
		RigidBody(const harmony::Entity::Description& description, const b2BodyId& actor);
	public:
		~RigidBody();

		const void* getID() const override;
		const harmony::Entity::Description& getDescription() const override;
		void setTransform(const harmony::component::Transform& transform) override;

	private:
		void initialize() override;
		void update();
		void deinitialize() override;

	private:
		harmony::Entity::Description mDescription;
		b2BodyId mActor;
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
		b2WorldId mScene;

		std::list<RigidBody*> mRigidBodies;
	};
}