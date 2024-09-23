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

	void Entity::initialize()
	{
		if (mDescription.visual.has_value())
		{
			harmony::Buffer::Description vBufferDescription;
			{
				vBufferDescription.label = mDescription.label;
				vBufferDescription.type = harmony::Buffer::Type::eVERTEX_BUFFER;
				vBufferDescription.data = mDescription.visual->mesh.vertices;
				vBufferDescription.size = mDescription.visual->mesh.verticesSize;
				vBufferDescription.stride = mDescription.visual->mesh.verticesStride;
			}
			auto vertexBuffer = HARMONY->createBuffer(vBufferDescription);
			vertexBuffer->initialize();

			harmony::Buffer* indexBuffer = nullptr;
			if(mDescription.visual->mesh.indicesSize)
			{
				harmony::Buffer::Description iBufferDescription;
				iBufferDescription.label = mDescription.label;
				iBufferDescription.type = harmony::Buffer::Type::eINDEX_BUFFER;
				iBufferDescription.data = mDescription.visual->mesh.indices;
				iBufferDescription.size = mDescription.visual->mesh.indicesSize;
				indexBuffer = HARMONY->createBuffer(iBufferDescription);
				indexBuffer->initialize();
			}

			harmony::Drawable::Description drawableDescription;
			{
				drawableDescription.label = mDescription.label;
				drawableDescription.vertexBuffer = vertexBuffer;
				drawableDescription.indexBuffer = indexBuffer;
				drawableDescription.fs.images[0] = mDescription.visual->texture1.image;
				drawableDescription.fs.imgCount = 1;
				drawableDescription.fs.samplers[0] = mDescription.visual->texture1.sampler;
				drawableDescription.fs.smpCount = 1;
			}


			drawableDescription.drawDescription.baseElement = 0;
			drawableDescription.drawDescription.numElements = mDescription.visual->mesh.numElements;
			drawableDescription.drawDescription.baseInstance = 0;
			drawableDescription.drawDescription.numInstances = 1;

			mDrawable = HARMONY->createDrawable(drawableDescription);
			mDrawable->initialize();
		}

		HLOG_DEBUG("Initialized %s", mDescription.label);
	}

	void Entity::deinitialize() 
	{
		mDrawable->deinitialize();
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

	void Entity::setTransform(const harmony::component::Transform& transform)
	{
		mDescription.transform = transform;
	}

	Entity::~Entity()
	{
		HLOG_INFO("Destroyed %s", mDescription.label);
		mDescription = { 0 };
		delete mDrawable;
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
		for (auto& entity : mEntities)
			delete entity.second;

		mEntities.clear();

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

		if (description.transform.has_value())
		{
			addTransformComponent(entity->getID(), description.transform.value());
		}
		if (description.visual.has_value())
		{
			addVisualComponent(entity->getID(), description.visual.value());
		}
		if (description.physics.has_value())
		{
			addPhysicsComponent(entity->getID(), description.physics.value());
		}

		return mEntities[*(entt::entity*)entity->getID()];
	}

	void Scene::addTransformComponent(const void* entityID, const harmony::component::Transform& component)
	{
		mRegistry.emplace<harmony::component::Transform>(*(entt::entity*)entityID, component);
		HLOG_DEBUG("Added %s", component.label);
	}

	void Scene::addVisualComponent(const void* entityID, const harmony::component::Visual& component)
	{
		mRegistry.emplace<harmony::component::Visual>(*(entt::entity*)entityID, component);
		HLOG_DEBUG("Added %s", component.label);
	}

	void Scene::addPhysicsComponent(const void* entityID, const harmony::component::Physics& component)
	{
		mRegistry.emplace<harmony::component::Physics>(*(entt::entity*)entityID, component);
		HLOG_DEBUG("Added %s", component.label);
	}
}
