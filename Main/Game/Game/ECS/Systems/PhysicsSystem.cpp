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
    int steps;
    float length;
    glm::vec3 moveStep, tempPosition, sepVector, oldVelocity;
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

        length = VECLEN(displacement);

        steps = std::min(std::max(1, (int)std::round(length / Physics::step)), Physics::maxSteps);
		moveStep = displacement / (float)steps;
        tempPosition = transform.GetWorldPosition();
        sepVector = glm::vec3(0.0f);
        oldVelocity = rigidBody.velocity;

        auto& cacheNode = Physics::cacheNodes[gameObject];

        for (int s = 0; s < steps; s++)
        {
            tempPosition += moveStep;
            
            collided = false;

            // get gameObjects from colliderCollectingSystem
            for (auto& otherCacheNode : Physics::cacheNodes)
            {
                if (gameObject != otherCacheNode.first &&  otherCacheNode.second.active)
                {
                    auto dist = tempPosition - otherCacheNode.second.position;
                    float distLen = VECLEN(dist);

                    localCollided = false;

                    if (cacheNode.collider.shape == Collider::ColliderShapes::CIRCLE && otherCacheNode.second.collider.shape == Collider::ColliderShapes::CIRCLE)
                    {
                        if (distLen <= (cacheNode.circleCollider.radius + otherCacheNode.second.circleCollider.radius))
                        {
                            if (Physics::DetectCollision(tempPosition, cacheNode.circleCollider, otherCacheNode.second.position, otherCacheNode.second.circleCollider, sepVector))
                            {
                                collided = true;
                                localCollided = true;
                            }
                        }
                    } else if (cacheNode.collider.shape == Collider::ColliderShapes::CIRCLE && otherCacheNode.second.collider.shape == Collider::ColliderShapes::BOX)
                    {
                        if (distLen <= (cacheNode.circleCollider.radius + std::max(otherCacheNode.second.boxCollider.width, otherCacheNode.second.boxCollider.height)))
                        {
                            if (Physics::DetectCollision(tempPosition, cacheNode.circleCollider,  otherCacheNode.first, sepVector))
                            {
                                collided = true;
                                localCollided = true;
                            }
                        }
                    }

                    if (otherCacheNode.second.collider.type == Collider::ColliderTypes::TRIGGER)
                    {
                        auto it = cacheNode.triggers.find( otherCacheNode.first);
                        if (localCollided)
                        {
                            if (it == cacheNode.triggers.end())
                            {
                                cacheNode.triggers.insert( otherCacheNode.first);
                                for (const auto& onTriggerEnter : HFEngine::ECS.GetComponent<Collider>( otherCacheNode.first).OnTriggerEnter)
                                {
                                    onTriggerEnter( otherCacheNode.first, gameObject);
                                }
                            }
                        }
                        else
                        {
                            if (it != cacheNode.triggers.end())
                            {
                                cacheNode.triggers.erase(it);
                                for (const auto& onTriggerExit : HFEngine::ECS.GetComponent<Collider>( otherCacheNode.first).OnTriggerExit)
                                {
                                    onTriggerExit( otherCacheNode.first, gameObject);
                                }
                            }
                        }
                    }
                    else
                    {
                        if (localCollided)
                        {
                            if (otherCacheNode.second.collider.type == Collider::ColliderTypes::DYNAMIC && HFEngine::ECS.SearchComponent<RigidBody>( otherCacheNode.first))
                            {
                                auto& otherRb = HFEngine::ECS.GetComponent<RigidBody>( otherCacheNode.first);
                                auto& otherTransform = HFEngine::ECS.GetComponent<Transform>( otherCacheNode.first);
                                float massFactor = rigidBody.mass / (rigidBody.mass + otherRb.mass);
                                otherTransform.TranslateSelf(-(sepVector * massFactor));
                                otherCacheNode.second.position = otherTransform.GetWorldPosition();
                                sepVector *= 1.0f - massFactor;
                            }

                            tempPosition += sepVector;
                        }
                    }
                }
            }
        }
        transform.TranslateSelf(tempPosition - transform.GetWorldPosition());
        cacheNode.position = transform.GetWorldPosition();
        rigidBody.moved = false;
	}
}

void PhysicsSystem::SetCollector(std::shared_ptr<ColliderCollectorSystem> colliderCollectorSystem)
{
    this->colliderCollectorSystem = colliderCollectorSystem;
}
