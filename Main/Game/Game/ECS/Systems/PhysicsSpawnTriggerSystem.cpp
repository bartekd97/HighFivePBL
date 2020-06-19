#include <iostream>
#include <glm/gtx/rotate_vector.hpp>

#include "PhysicsSpawnTriggerSystem.h"
#include "HFEngine.h"

#include "../../Physics/Physics.h"
#include "../Components.h"

void PhysicsSpawnTriggerSystem::Update(float dt)
{
    if (dt > Physics::maxDelta) return;

    bool localCollided;
    glm::vec3 sepVector, dist;
    float length, distLen;
    Physics::ProcessGameObjects(triggerColliderCollectorSystem->gameObjects, false);

    for (auto it = triggerColliderCollectorSystem->gameObjects.begin(); it != triggerColliderCollectorSystem->gameObjects.end(); it = triggerColliderCollectorSystem->gameObjects.erase(it))
    {
        auto& cacheNode = Physics::cacheNodes[*it];
        if (cacheNode.collider.type == Collider::ColliderTypes::TRIGGER)
        {
            for (auto const& gameObject : gameObjects)
            {
                auto& otherCacheNode = Physics::cacheNodes[gameObject];
                auto& rigidBody = HFEngine::ECS.GetComponent<RigidBody>(gameObject);

                if (cacheNode.gridMaxX < otherCacheNode.gridMinX
                    || cacheNode.gridMinX > otherCacheNode.gridMaxX
                    || cacheNode.gridMaxY < otherCacheNode.gridMinY
                    || cacheNode.gridMinY > otherCacheNode.gridMaxY) continue;

                localCollided = false;
                dist = cacheNode.position - otherCacheNode.position;
                distLen = VECLEN(dist);

                if (cacheNode.collider.shape == Collider::ColliderShapes::CIRCLE && otherCacheNode.collider.shape == Collider::ColliderShapes::CIRCLE)
                {
                    if (distLen <= (cacheNode.circleCollider.radius + otherCacheNode.circleCollider.radius))
                    {
                        if (Physics::DetectCollision(cacheNode.position, cacheNode.circleCollider, otherCacheNode.position, otherCacheNode.circleCollider, sepVector))
                        {
                            localCollided = true;
                        }
                    }
                }
                else if (cacheNode.collider.shape == Collider::ColliderShapes::CIRCLE && otherCacheNode.collider.shape == Collider::ColliderShapes::BOX)
                {
                    if (distLen <= (cacheNode.circleCollider.radius + otherCacheNode.maxSide))
                    {
                        if (Physics::DetectCollision(cacheNode.position, cacheNode.circleCollider, gameObject, sepVector))
                        {
                            localCollided = true;
                        }
                    }
                }

                if (localCollided)
                {
                    auto triggerIt = cacheNode.triggers.find(gameObject);
                    if (triggerIt == cacheNode.triggers.end())
                    {
                        cacheNode.triggers.insert(gameObject);
                        for (const auto& onTriggerEnter : HFEngine::ECS.GetComponent<Collider>(*it).OnTriggerEnter)
                        {
                            onTriggerEnter(*it, gameObject);
                        }
                        for (const auto& onTriggerEnter : HFEngine::ECS.GetComponent<Collider>(gameObject).OnTriggerEnter)
                        {
                            onTriggerEnter(gameObject, *it);
                        }
                    }
                }
            }
        }
    }
}

void PhysicsSpawnTriggerSystem::SetCollector(std::shared_ptr<TriggerColliderCollectorSystem> triggerColliderCollectorSystem)
{
    this->triggerColliderCollectorSystem = triggerColliderCollectorSystem;
}
