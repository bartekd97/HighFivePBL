#include <iostream>
#include <glm/gtx/rotate_vector.hpp>

#include "PhysicsSystem.h"
#include "HFEngine.h"

#include "../../Physics/Physics.h"
#include "../Components.h"

void PhysicsSystem::Init()
{

}

/*
 * -what about transform scale?
 * -replace velocity with toCheck flag?
 */
void PhysicsSystem::Update(float dt)
{
    Physics::ProcessGameObjects(colliderCollectorSystem->gameObjects, true);
    bool collided, localCollided;
	for (auto const& gameObject : gameObjects)
	{
		auto& rigidBody = HFEngine::ECS.GetComponent<RigidBody>(gameObject);
		auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);

		glm::vec3 displacement;
		if (rigidBody.moved && !rigidBody.isFalling)
		{
			displacement = rigidBody.movePosition - transform.GetWorldPosition();
		}
		else
		{
			displacement = rigidBody.velocity * dt;
		}
        displacement.y = 0.0f;

        if (!HFEngine::ECS.SearchComponent<Collider>(gameObject))
        {
            transform.SetPosition(transform.GetWorldPosition() + displacement);
            continue;
        }

        float length = VECLEN(displacement);

        int steps = std::min(std::max(1, (int)std::round(length / Physics::step)), Physics::maxSteps);
		glm::vec3 moveStep = displacement / (float)steps;
        glm::vec3 tempPosition = transform.GetWorldPosition();
        glm::vec3 sepVector(0.0f);
        glm::vec3 oldVelocity = rigidBody.velocity;

        auto& cacheNode = Physics::cacheNodes[gameObject];

        for (int s = 0; s < steps; s++)
        {
            tempPosition += moveStep;
            
            collided = false;

            // get gameObjects from colliderCollectingSystem
            for (auto const& otherObject : colliderCollectorSystem->gameObjects)
            {
                if (gameObject != otherObject)
                {
                    auto& otherCacheNode = Physics::cacheNodes[otherObject];

                    auto dist = tempPosition - otherCacheNode.position;
                    float distLen = VECLEN(dist);

                    localCollided = false;

                    if (cacheNode.collider.shape == Collider::ColliderShapes::CIRCLE && otherCacheNode.collider.shape == Collider::ColliderShapes::CIRCLE)
                    {
                        if (distLen <= (cacheNode.circleCollider.radius + otherCacheNode.circleCollider.radius))
                        {
                            if (Physics::DetectCollision(tempPosition, cacheNode.circleCollider, otherCacheNode.position, otherCacheNode.circleCollider, sepVector))
                            {
                                collided = true;
                                localCollided = true;
                            }
                        }
                    } else if (cacheNode.collider.shape == Collider::ColliderShapes::CIRCLE && otherCacheNode.collider.shape == Collider::ColliderShapes::BOX)
                    {
                        if (distLen <= (cacheNode.circleCollider.radius + std::max(otherCacheNode.boxCollider.width, otherCacheNode.boxCollider.height)))
                        {
                            if (Physics::DetectCollision(tempPosition, cacheNode.circleCollider, otherCacheNode.position, otherCacheNode.rotation, otherCacheNode.boxCollider, sepVector))
                            {
                                collided = true;
                                localCollided = true;
                            }
                        }
                    }

                    if (otherCacheNode.collider.type == Collider::ColliderTypes::TRIGGER)
                    {
                        auto it = cacheNode.triggers.find(otherObject);
                        if (localCollided)
                        {
                            if (it == cacheNode.triggers.end())
                            {
                                cacheNode.triggers.insert(otherObject);
                                for (const auto& onTriggerEnter : otherCacheNode.collider.OnTriggerEnter)
                                {
                                    onTriggerEnter(otherObject, gameObject);
                                }
                            }
                        }
                        else
                        {
                            if (it != cacheNode.triggers.end())
                            {
                                cacheNode.triggers.erase(it);
                                for (const auto& onTriggerExit : otherCacheNode.collider.OnTriggerExit)
                                {
                                    onTriggerExit(otherObject, gameObject);
                                }
                            }
                        }
                    }
                    else
                    {
                        if (localCollided)
                        {
                            if (otherCacheNode.collider.type == Collider::ColliderTypes::DYNAMIC && HFEngine::ECS.SearchComponent<RigidBody>(otherObject))
                            {
                                auto& otherRb = HFEngine::ECS.GetComponent<RigidBody>(otherObject);
                                auto& otherTransform = HFEngine::ECS.GetComponent<Transform>(otherObject);
                                float massFactor = rigidBody.mass / (rigidBody.mass + otherRb.mass);
                                otherTransform.TranslateSelf(-(sepVector * massFactor));
                                otherCacheNode.position = otherTransform.GetWorldPosition();
                                otherRb.velocity.x -= sepVector.x / dt * massFactor;
                                otherRb.velocity.z -= sepVector.z / dt * massFactor;
                                sepVector *= 1.0f - massFactor;
                            }

                            tempPosition += sepVector;
                            rigidBody.velocity.x += sepVector.x / dt;
                            rigidBody.velocity.z += sepVector.z / dt;
                        }
                    }
                }
            }
            //if (collided) break;
        }
        transform.TranslateSelf(tempPosition - transform.GetWorldPosition());
        cacheNode.position = transform.GetWorldPosition();
        if (rigidBody.velocity.x * oldVelocity.x < 0) rigidBody.velocity.x = 0.0f;
        if (rigidBody.velocity.z * oldVelocity.z < 0) rigidBody.velocity.z = 0.0f;
        rigidBody.velocity.x *= 0.75f;
        rigidBody.velocity.z *= 0.75f;
        rigidBody.moved = false;
	}
}

void PhysicsSystem::SetCollector(std::shared_ptr<ColliderCollectorSystem> colliderCollectorSystem)
{
    this->colliderCollectorSystem = colliderCollectorSystem;
}
