#include <iostream>
#include <glm/gtx/rotate_vector.hpp>

#include "PhysicsSystem.h"
#include "HFEngine.h"

#include "../../Physics/Physics.h"
#include "../Components.h"

void PhysicsSystem::Init()
{
    minimalMovement = 0.005f;
}

void PhysicsSystem::Update(float dt)
{
    Physics::ProcessGameObjects(colliderCollectorSystem->gameObjects, true);
    bool localCollided;
    int steps;
    float length, distLen;
    glm::vec3 moveStep, tempPosition, sepVector, oldVelocity, dist;
	for (auto const& gameObject : gameObjects)
	{
        auto& cacheNode = Physics::cacheNodes[gameObject];
		auto& rigidBody = HFEngine::ECS.GetComponent<RigidBody>(gameObject);

		glm::vec3 displacement;
		if (rigidBody.moved && !rigidBody.isFalling)
		{
			displacement = rigidBody.movePosition - cacheNode.position;
		}
		else
		{
			displacement = rigidBody.velocity * dt;
		}
        displacement.y = 0.0f;

        length = VECLEN(displacement);

        steps = std::min(std::max(1, (int)std::round(length / Physics::step)), Physics::maxSteps);
		moveStep = displacement / (float)steps;
        tempPosition = cacheNode.position;
        sepVector = glm::vec3(0.0f);
        oldVelocity = rigidBody.velocity;

        for (int s = 0; s < steps; s++)
        {
            tempPosition += moveStep;

            // get gameObjects from colliderCollectingSystem
            //for (int i = 0; i <= Physics::maxGameObject; i++)
            CacheNode* otherCacheNode = Physics::cacheNodes.data();
            for (int i = 0; i <= Physics::maxGameObject; i++, otherCacheNode++)
            {
                //auto& otherCacheNode = Physics::cacheNodes[i];
                if (gameObject != i && otherCacheNode->state == CacheNode::STATE::ACTIVE)
                {
                    if (cacheNode.gridMaxX < otherCacheNode->gridMinX
                        || cacheNode.gridMinX > otherCacheNode->gridMaxX
                        || cacheNode.gridMaxY < otherCacheNode->gridMinY
                        || cacheNode.gridMinY > otherCacheNode->gridMaxY) continue;
                    dist = tempPosition - otherCacheNode->position;
                    distLen = VECLEN(dist);

                    localCollided = false;

                    if (cacheNode.collider.shape == Collider::ColliderShapes::CIRCLE && otherCacheNode->collider.shape == Collider::ColliderShapes::CIRCLE)
                    {
                        if (distLen <= (cacheNode.circleCollider.radius + otherCacheNode->circleCollider.radius))
                        {
                            if (Physics::DetectCollision(tempPosition, cacheNode.circleCollider, otherCacheNode->position, otherCacheNode->circleCollider, sepVector))
                            {
                                localCollided = true;
                            }
                        }
                    } else if (cacheNode.collider.shape == Collider::ColliderShapes::CIRCLE && otherCacheNode->collider.shape == Collider::ColliderShapes::BOX)
                    {
                        if (distLen <= (cacheNode.circleCollider.radius + otherCacheNode->maxSide))
                        {
                            if (Physics::DetectCollision(tempPosition, cacheNode.circleCollider, i, sepVector))
                            {
                                localCollided = true;
                            }
                        }
                    }

                    if (otherCacheNode->collider.type == Collider::ColliderTypes::TRIGGER)
                    {
                        auto it = cacheNode.triggers.find(i);
                        if (localCollided)
                        {
                            if (it == cacheNode.triggers.end())
                            {
                                cacheNode.triggers.insert(i);
                                for (const auto& onTriggerEnter : HFEngine::ECS.GetComponent<Collider>(i).OnTriggerEnter)
                                {
                                    onTriggerEnter(i, gameObject);
                                }
                                for (const auto& onTriggerEnter : HFEngine::ECS.GetComponent<Collider>(gameObject).OnTriggerEnter)
                                {
                                    onTriggerEnter(gameObject, i);
                                }
                            }
                        }
                        else
                        {
                            if (it != cacheNode.triggers.end())
                            {
                                cacheNode.triggers.erase(it);
                                for (const auto& onTriggerExit : HFEngine::ECS.GetComponent<Collider>(i).OnTriggerExit)
                                {
                                    onTriggerExit(i, gameObject);
                                }
                                for (const auto& onTriggerExit : HFEngine::ECS.GetComponent<Collider>(gameObject).OnTriggerExit)
                                {
                                    onTriggerExit(gameObject, i);
                                }
                            }
                        }
                    }
                    else
                    {
                        if (localCollided)
                        {
                            if (otherCacheNode->collider.type == Collider::ColliderTypes::DYNAMIC && HFEngine::ECS.SearchComponent<RigidBody>(i))
                            {
                                auto& otherRb = HFEngine::ECS.GetComponent<RigidBody>(i);
                                auto& otherTransform = HFEngine::ECS.GetComponent<Transform>(i);
                                float massFactor = rigidBody.mass / (rigidBody.mass + otherRb.mass);
                                otherTransform.TranslateSelf(-(sepVector * massFactor));
                                otherCacheNode->position = otherTransform.GetWorldPosition();
                                sepVector *= 1.0f - massFactor;
                            }

                            tempPosition += sepVector;
                        }
                    }
                }
            }
        }
        tempPosition -= cacheNode.position;
        if (VECLEN(tempPosition) >= minimalMovement)
        {
            auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
            transform.TranslateSelf(tempPosition);
            cacheNode.position = transform.GetWorldPosition();
        }
        rigidBody.moved = false;
	}
}

void PhysicsSystem::SetCollector(std::shared_ptr<ColliderCollectorSystem> colliderCollectorSystem)
{
    this->colliderCollectorSystem = colliderCollectorSystem;
}
