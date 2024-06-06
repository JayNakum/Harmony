#include "ENTT_Scene.h"


namespace ENTT
{
	Entity::Entity(const harmony::Entity::Description& description, entt::registry& registry)
		: mDescription(description), mRegistry(registry) 
	{
		mEntity = mRegistry.create();
	}

	Entity::~Entity()
	{
		mRegistry.destroy(mEntity);
		mDescription = { 0 };
	}

	void Entity::initialize()
	{
		if (mDescription.visual.has_value())
		{
			mRegistry.emplace<harmony::component::Visual>(mEntity, mDescription.visual.value());
		}
	}

	void ENTT::Entity::deinitialize() {}

	const void* Entity::getID() const
	{
		return &mEntity;
	}

	const harmony::Entity::Description& ENTT::Entity::getDescription() const
	{
		return mDescription;
	}
}

namespace ENTT
{
	Scene::Scene(const harmony::Scene::Description& description)
		: mDescription(description) {}

	Scene::~Scene()
	{
		mRegistry.clear();
	}

	void Scene::initialize()
	{
	}

	void Scene::deinitialize()
	{
	}

	const harmony::Scene::Description& Scene::getDescription() const
	{
		return mDescription;
	}

	harmony::Entity* Scene::createEntity(const harmony::Entity::Description& description)
	{
		return new Entity(description, mRegistry);
	}
}
