#include "ENTT_Scene.h"

#include "Harmony.h"

#include "Helpers/HLog.h"

#include <string>

namespace ENTT
{
	Entity::Entity(const harmony::Entity::Description& description, const entt::entity& entity)
		: mDescription(description), mEntity(entity), mDrawable(nullptr) 
	{
		HLOG_INFO("Created %s", mDescription.label);
	}

	Entity::~Entity()
	{
		HLOG_INFO("Destroyed %s", mDescription.label);
		mDescription = { 0 };
	}

	void Entity::initialize()
	{
		/*if (mDescription.visual.has_value())
		{
			harmony::Buffer::Description vBufferDescription;
			{
				vBufferDescription.label = mDescription.label;
				vBufferDescription.type = harmony::Buffer::Type::eVERTEX_BUFFER;
				vBufferDescription.data = mDescription.visual->mesh.vertices;
				vBufferDescription.size = mDescription.visual->mesh.verticesSize;
			}
			auto vertexBuffer = harmony::createBuffer(vBufferDescription);

			harmony::Buffer::Description iBufferDescription;
			{
				iBufferDescription.label = mDescription.label;
				iBufferDescription.type = harmony::Buffer::Type::eINDEX_BUFFER;
				iBufferDescription.data = mDescription.visual->mesh.indices;
				iBufferDescription.size = mDescription.visual->mesh.indicesSize;
			}
			auto indexBuffer = harmony::createBuffer(iBufferDescription);

			harmony::Drawable::Description drawableDescription;
			{
				drawableDescription.label = mDescription.label;
				drawableDescription.vertexBuffer = vertexBuffer;
				drawableDescription.indexBuffer = indexBuffer;
			}
			mDrawable = harmony::createDrawable(drawableDescription);
			mDrawable->initialize();
		}*/

		HLOG_DEBUG("Initialized %s", mDescription.label);
	}

	void Entity::deinitialize() 
	{
		HLOG_DEBUG("Deinitialized %s", mDescription.label);
	}

	const void* Entity::getID() const
	{
		return &mEntity;
	}

	const harmony::Entity::Description& Entity::getDescription() const
	{
		return mDescription;
	}
	const harmony::Drawable* Entity::getDrawable() const
	{
		return mDrawable;
	}
}

namespace ENTT
{
	Scene::Scene(const harmony::Scene::Description& description)
		: mDescription(description) 
	{
		HLOG_INFO("Created %s", mDescription.label);
	}

	Scene::~Scene()
	{
		mRegistry.clear();
		HLOG_INFO("Destroyed %s", mDescription.label);
		mDescription = { 0 };
	}

	void Scene::initialize()
	{
		for (auto& entity : mEntities)
			entity.second->initialize();

		HLOG_DEBUG("Initialized %s", mDescription.label);
	}

	void Scene::deinitialize()
	{
		for (auto& entity : mEntities)
			entity.second->deinitialize();

		HLOG_DEBUG("Deinitialized %s", mDescription.label);
	}

	std::list<harmony::Entity*> Scene::getVisualEntities() const
	{
		std::list<harmony::Entity*> visualEntities;

		auto entities = mRegistry.view<harmony::component::Visual>();
		
		for (auto entity : entities)
			visualEntities.push_back(mEntities.at(entity));

		return visualEntities;
	}

	const harmony::Scene::Description& Scene::getDescription() const
	{
		return mDescription;
	}

	harmony::Entity* Scene::createEntity(const harmony::Entity::Description& description)
	{
		Entity* entity = new Entity(description, mRegistry.create());
		mEntities[*(entt::entity*)entity->getID()] = entity;

		if (description.visual.has_value())
		{
			addVisualComponent(entity->getID(), *description.visual);
		}

		return mEntities[*(entt::entity*)entity->getID()];
	}

	void Scene::addVisualComponent(const void* entityID, const harmony::component::Visual& component)
	{
		mRegistry.emplace<harmony::component::Visual>(*(entt::entity*)entityID, component);
		HLOG_DEBUG("Added %s", component.label);
	}
}
