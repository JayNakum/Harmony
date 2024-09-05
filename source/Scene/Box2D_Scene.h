#pragma once

#include "Scene/HScene.h"

#include "box2d/box2d.h"

#include <unordered_map>

namespace box2d
{
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
		b2WorldDef mScene;

		//std::unordered_map<b2BodyId, RigidBody*> mRigidBodies;
	};
}