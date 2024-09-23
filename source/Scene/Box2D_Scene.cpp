#include "Box2D_Scene.h"

#include "Helpers/HLog.h"

#include <cmath>

namespace helper
{
	static inline void toTransform(const b2Transform& b2transform, harmony::component::Transform& transform)
	{
		transform.position[0] = b2transform.p.x;
		transform.position[1] = b2transform.p.y;
		transform.position[2] = 0.0f;

		transform.rotation[0] = 0.0f;
		transform.rotation[1] = 0.0f;
		transform.rotation[2] = std::atan2f(b2transform.q.s, b2transform.q.c);
	}

	static inline void fromTransform(const harmony::component::Transform& transform, b2Transform& b2transform)
	{
		b2transform.p.x = transform.position[0];
		b2transform.p.y = transform.position[1];

		float yaw = transform.rotation[2];
		b2transform.q.c = std::cosf(yaw);
		b2transform.q.s = std::sinf(yaw);
	}
}

namespace box2d
{
	RigidBody::RigidBody(const harmony::Entity::Description& description, const b2BodyId& actor)
		: mDescription(description), mActor(actor)
	{
		HLOG_INFO("Created %s", mDescription.label);
	}

	const void* RigidBody::getID() const
	{
		return &mActor;
	}

	const harmony::Entity::Description& RigidBody::getDescription() const
	{
		return mDescription;
	}

	void RigidBody::setTransform(const harmony::component::Transform& transform)
	{
		mDescription.transform = transform;
	}

	void RigidBody::initialize()
	{
		HASSERT(mDescription.physics.has_value(), "Physics component is missing for %s", mDescription.label);

	}

	void RigidBody::update()
	{
		helper::toTransform(b2Body_GetTransform(mActor), mDescription.transform.value());
	}

	void RigidBody::deinitialize()
	{}

	RigidBody::~RigidBody()
	{
		HLOG_INFO("Destroyed %s", mDescription.label);
		mDescription = { 0 };
	}
}

namespace box2d
{
	Scene::Scene(const harmony::Scene::Description& description)
		: mDescription(description), mScene{0}
	{
		b2WorldDef createDesc = b2DefaultWorldDef();
		b2Vec2 g{ 0.0f, -9.81f };
		createDesc.gravity = g;

		mScene = b2CreateWorld(&createDesc);
		HLOG_INFO("Created %s", mDescription.label);
	}

	const harmony::Scene::Description& Scene::getDescription() const
	{
		return mDescription;
	}

	harmony::Entity* Scene::createEntity(const harmony::Entity::Description& description)
	{
		b2BodyDef createDesc = b2DefaultBodyDef();
		b2BodyId actor = b2CreateBody(mScene, &createDesc);

		return mRigidBodies.emplace_back(new box2d::RigidBody(description, actor));
	}

	void Scene::initialize()
	{
		for (auto& entity : mRigidBodies)
		{
			entity->initialize();
		}

		HLOG_DEBUG("Initialized %s", mDescription.label);
	}

	void Scene::update()
	{
		for (auto& entity : mRigidBodies)
		{
			entity->update();
		}
	}

	void Scene::deinitialize()
	{
		for (auto& entity : mRigidBodies)
		{
			entity->deinitialize();
		}

		HLOG_DEBUG("Deinitialized %s", mDescription.label);
	}

	Scene::~Scene()
	{
		for (auto& entity : mRigidBodies)
		{
			delete entity;
		}

		mRigidBodies.clear();

		b2DestroyWorld(mScene);
		
		HLOG_INFO("Destroyed %s", mDescription.label);
		mDescription = { 0 };
	}
}