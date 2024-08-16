#include "PhysX_Scene.h"

#include <cmath>

namespace helper
{
	static physx::PxVec3 toEuler(const physx::PxQuat& q)
	{
		physx::PxVec3 euler;

		float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
		float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
		euler.x = std::atan2(sinr_cosp, cosr_cosp);
		
		float sinp = 2 * (q.w * q.y - q.z * q.x);
		if (std::abs(sinp) >= 1)
			euler.y = std::copysign(physx::PxPiDivTwo, sinp);
		else
			euler.y = std::asin(sinp);

		float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
		float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
		euler.z = std::atan2(siny_cosp, cosy_cosp);

		return euler;
	}

	static harmony::component::Transform toTransform(const physx::PxTransform& pxTransform)
	{
		harmony::component::Transform transform;
		physx::PxMat44 modelMatrix(pxTransform);

		transform.position[0] = modelMatrix.getPosition().x;
		transform.position[1] = modelMatrix.getPosition().y;
		transform.position[2] = modelMatrix.getPosition().z;

		physx::PxVec3 col0 = physx::PxVec3(modelMatrix.column0.x, modelMatrix.column0.y, modelMatrix.column0.z);
		physx::PxVec3 col1 = physx::PxVec3(modelMatrix.column1.x, modelMatrix.column1.y, modelMatrix.column1.z);
		physx::PxVec3 col2 = physx::PxVec3(modelMatrix.column2.x, modelMatrix.column2.y, modelMatrix.column2.z);

		transform.scale[0] = col0.magnitude();
		transform.scale[1] = col1.magnitude();
		transform.scale[2] = col2.magnitude();

		col0 /= transform.scale[0];
		col1 /= transform.scale[1];
		col2 /= transform.scale[2];

		physx::PxMat33 rotationMatrix(col0, col1, col2);
		physx::PxQuat rotationQuat = physx::PxQuat(rotationMatrix);

		physx::PxVec3 euler = toEuler(rotationQuat);

		transform.rotation[0] = euler.x * (180.0f / physx::PxPi);
		transform.rotation[1] = euler.y * (180.0f / physx::PxPi);
		transform.rotation[2] = euler.z * (180.0f / physx::PxPi);
	}

	static physx::PxTransform toPxTransform(const harmony::component::Transform& transform)
	{
		physx::PxMat44 translationMatrix(physx::PxTransform(
			physx::PxVec3(transform.position[0], transform.position[1], transform.position[2])
		));

		physx::PxQuat rotationQuat = 
			physx::PxQuat(physx::PxDegToRad(transform.rotation[0]), physx::PxVec3(1, 0, 0)) *
			physx::PxQuat(physx::PxDegToRad(transform.rotation[1]), physx::PxVec3(0, 1, 0)) *
			physx::PxQuat(physx::PxDegToRad(transform.rotation[2]), physx::PxVec3(0, 0, 1));
		physx::PxMat44 rotationMatrix(rotationQuat);

		physx::PxMat44 scaleMatrix(physx::PxIdentity);
		scaleMatrix.column0 = physx::PxVec4(transform.scale[0], 0, 0, 0);
		scaleMatrix.column1 = physx::PxVec4(0, transform.scale[1], 0, 0);
		scaleMatrix.column2 = physx::PxVec4(0, 0, transform.scale[2], 0);

		physx::PxMat44 modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
		
		return physx::PxTransform(modelMatrix);
	}
}

namespace physx
{
	static physx::PxFoundation* gPhysXFoundation = nullptr;
	static physx::PxPhysics* gPhysX = nullptr;

#ifdef _DEBUG
	static physx::PxPvd* gPVD = nullptr;
#endif // _DEBUG

}

namespace physx
{
	RigidBody::RigidBody(const harmony::Entity::Description& description)
		: mDescription(description), mMaterial(nullptr)
	{}

	RigidBody::~RigidBody()
	{
		mDescription = { 0 };
	}
	const void* RigidBody::getID() const
	{
		return mActor;
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
		const auto& phxComponent = mDescription.physics.value();

		auto transform = helper::toPxTransform(mDescription.transform.value());
		bool isKinematic = false;
		switch (phxComponent.type)
		{
		default:
		case harmony::component::Physics::Type::eSTATIC:
			mActor = gPhysX->createRigidStatic(transform);
			break;
		case harmony::component::Physics::Type::eKINEMATIC:
			isKinematic = true;
		case harmony::component::Physics::Type::eDYNAMIC:
			physx::PxRigidDynamic* actor = gPhysX->createRigidDynamic(transform);
			actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, isKinematic);
			actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !phxComponent.gravity);
			physx::PxRigidBodyExt::setMassAndUpdateInertia(*actor, phxComponent.mass);
			mActor = actor;
			break;
		}

		mActor->setGlobalPose(transform);
		mMaterial = gPhysX->createMaterial(phxComponent.staticFriction, phxComponent.dynamicFriction, phxComponent.restitution);

		switch (phxComponent.shape)
		{
			default:
			case harmony::component::Physics::Shape::eSPHERE:
			{
				// TODO: Implement sphere
			}break;

			case harmony::component::Physics::Shape::eBOX:
			{
				//physx::PxBounds3 bounds = mActor->getWorldBounds();
				physx::PxBoxGeometry geometry(physx::PxVec3(phxComponent.box.halfExtents[0], phxComponent.box.halfExtents[1], phxComponent.box.halfExtents[2]));
				physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape((*mActor), geometry, (*mMaterial));
				shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
				shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
				shape->setLocalPose(transform);
				mActor->attachShape(*shape);
				shape->release();
			}break;

			case harmony::component::Physics::Shape::eCAPSULE:
			{
				// TODO: Implement capsule
			}break;
		}
	}

	void RigidBody::deinitialize()
	{
		mActor->release();
	}
}

namespace physx
{
	Scene::Scene(const harmony::Scene::Description& description)
		: mDescription(description), mScene(nullptr)
	{
#ifdef _DEBUG
		gPVD = PxCreatePvd(*gPhysXFoundation);

		PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 100);
		gPVD->connect(*transport, PxPvdInstrumentationFlag::eALL);

#endif // _DEBUG
		gPhysX = PxCreatePhysics(PX_PHYSICS_VERSION, *gPhysXFoundation, PxTolerancesScale(), true, gPVD);

	}
	const harmony::Scene::Description& Scene::getDescription() const
	{
		return mDescription;
	}

	harmony::Entity* Scene::createEntity(const harmony::Entity::Description& description)
	{
		return new physx::RigidBody(description);
	}

	void Scene::initialize()
	{
		PxSceneDesc sceneDesc(gPhysX->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		mScene = gPhysX->createScene(sceneDesc);
	}

	void Scene::update()
	{
		mScene->simulate(1.0f / 60.0f);
		mScene->fetchResults(true);
	}

	void Scene::deinitialize()
	{
		mScene->release();
	}

	Scene::~Scene()
	{

#ifdef _DEBUG
		gPVD->disconnect();
		gPVD->release();
#endif // _DEBUG
		gPhysX->release();
		gPhysXFoundation->release();
	}
}