#include "PhysicsComponent.h"
#include "Sandbox/Systems/PhysicsSystem.h"

PhysicsComponent::PhysicsComponent(const Hollow::Vector3& position, float mass) :
	originPosition(position), mass(mass)
{
	isDynamic = (mass != 0.f);
}

PhysicsComponent::~PhysicsComponent()
{
	PhysicsSystem::instance()->dynamicsWorld->removeRigidBody(body.get());
}

void PhysicsComponent::addBoxShape(const Hollow::Vector3& size)
{
	type = PhysicsShapeType::PST_BOX;
	
	boxSize = size;
	shape = std::make_shared<btBoxShape>(btVector3(size.x, size.y, size.z));
}

void PhysicsComponent::addSphereShape(float radius)
{
	type = PhysicsShapeType::PST_SPHERE;

	sphereRadius = radius;
	shape = std::make_shared<btSphereShape>(radius);
}

void PhysicsComponent::addPlaneShape(const Hollow::Vector3& normal, float size)
{
	type = PhysicsShapeType::PST_PLANE;

	planeNormal = normal;
	planeSize = size;
	shape = std::make_shared<btStaticPlaneShape>(btVector3(planeNormal.x, planeNormal.y, planeNormal.z), size);
}

void PhysicsComponent::addCapsuleShape(float height, float radius)
{
	type = PhysicsShapeType::PST_CAPSULE;

	capsuleHeight = height;
	capsuleRadius = radius;
	shape = std::make_shared<btCapsuleShape>(radius, height);
}

void PhysicsComponent::addAABBShape(const Hollow::s_ptr<Hollow::Import::Model>& model)
{
	type = PhysicsShapeType::PST_AABB;
	shape = std::make_shared<btConvexHullShape>();
	btConvexHullShape* convex = std::static_pointer_cast<btConvexHullShape>(shape).get();
	
	convex->addPoint(btVector3(model->A.x, model->A.y, model->A.z));
	convex->addPoint(btVector3(model->A.x, model->A.y, model->B.z));
	convex->addPoint(btVector3(model->A.x, model->B.y, model->A.z));
	convex->addPoint(btVector3(model->A.x, model->B.y, model->B.z));

	convex->addPoint(btVector3(model->B.x, model->A.y, model->A.z));
	convex->addPoint(btVector3(model->B.x, model->A.y, model->B.z));
	convex->addPoint(btVector3(model->B.x, model->B.y, model->A.z));
	convex->addPoint(btVector3(model->B.x, model->B.y, model->B.z));
}

void PhysicsComponent::setPosition(const Hollow::Vector3& position)
{
	originPosition = position;

	if (body == nullptr) {
		return;
	}

	btTransform transform;
	transform.setIdentity();

	transform.setOrigin(btVector3(position.x, position.y, position.z));
	transform.setRotation(body->getWorldTransform().getRotation());
	body->setWorldTransform(transform);
	body->activate(true);
}

void PhysicsComponent::setRotation(const Hollow::Vector3 rotation)
{
	if (body == nullptr) {
		return;
	}

	btTransform transform;
	btQuaternion rotationQ;
	rotationQ.setEulerZYX(rotation.z, rotation.y, rotation.x);

	if (isDynamic) {
		body->getMotionState()->getWorldTransform(transform);
	} else {
		transform = body->getWorldTransform();
	}

	transform.setRotation(rotationQ);
	
	body->setCenterOfMassTransform(transform);
	body->activate(true);
}

void PhysicsComponent::setLocalScale(const Hollow::Vector3& scale)
{
	localScale = scale;
	if (shape != nullptr) {
		shape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
	}
}

void PhysicsComponent::setAngularFactor(const Hollow::Vector3& factor)
{
	angularFactor = factor;
	body->setAngularFactor(btVector3(factor.x, factor.y, factor.z));
}

void PhysicsComponent::init()
{
	btTransform startTransform;
	startTransform.setIdentity();

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(originPosition.x, originPosition.y, originPosition.z));
	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	myMotionState = std::make_shared<btDefaultMotionState>(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState.get(), shape.get(), localInertia);

	if (body != nullptr) {
		PhysicsSystem::instance()->dynamicsWorld->removeRigidBody(body.get());
	}

	body = std::make_shared<btRigidBody>(rbInfo);
}
