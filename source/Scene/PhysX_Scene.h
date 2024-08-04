#pragma once

#include "Scene/HScene.h"

#include <PxPhysicsAPI.h>

namespace physx
{
	class RigidBody : public harmony::Entity
	{
	private:
		friend class Scene;
		RigidBody(const harmony::Entity::Description& description, const entt::entity& entity);
	public:
		~RigidBody();

		const void* getID() const override;
		const harmony::Entity::Description& getDescription() const override;

	private:
		void initialize() override;
		void deinitialize() override;

	private:
		harmony::Entity::Description mDescription;
		

		harmony::Drawable* mDrawable;
	};

	class Scene : public harmony::Scene
	{

	};
}