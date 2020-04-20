#include <iostream>
#include <glm/gtx/rotate_vector.hpp>

#include "PhysicsSystem.h"
#include "HFEngine.h"

#include "../Components.h"

#define clamp(x,min,max) x > min ? (x < max ? x : max) : min
#define veclen(v) std::sqrtf(v.x*v.x + v.y*v.y);

void PhysicsSystem::Init()
{
	step = 0.25f;
}

/*
 * TODO LIST:
 * -refactor
 * -what about transform scale?
 * -caching?
 * -check only colliders on same cell
 */
void PhysicsSystem::Update(float dt)
{
    bool collided, localCollided;
	for (auto const& gameObject : gameObjects)
	{
		auto& rigidBody = HFEngine::ECS.GetComponent<RigidBody>(gameObject);
		auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);

		glm::vec3 displacement;
		if (rigidBody.moved)
		{
			displacement = rigidBody.movePosition - transform.GetPosition();
		}
		else
		{
			displacement = rigidBody.velocity * dt;
		}

        if (!HFEngine::ECS.SearchComponent<Collider>(gameObject))
        {
            transform.SetPosition(transform.GetPosition() + displacement);
            continue;
        }

		float length = sqrt((displacement.x * displacement.x) + (displacement.z * displacement.z));

        //if (islessequal(length, step)) continue;
        int steps = std::max(1, (int)std::round(length / step));
		glm::vec3 moveStep = displacement / (float)steps;
        glm::vec3 tempPosition = transform.GetPosition();
        glm::vec3 sepVector(0.0f);
        glm::vec3 oldVelocity = rigidBody.velocity;

        auto& collider = HFEngine::ECS.GetComponent<Collider>(gameObject);

        for (int s = 0; s < steps; s++)
        {
            tempPosition += moveStep;
            //transform.SetPosition(tempPosition);
            
            collided = false;

            // get gameObjects from colliderCollectingSystem
            for (auto const& otherObject : colliderCollectorSystem->gameObjects)
            {
                if (gameObject != otherObject)
                {
                    auto& otherTransform = HFEngine::ECS.GetComponent<Transform>(otherObject);
                    auto const& otherCollider = HFEngine::ECS.GetComponent<Collider>(otherObject);
                    auto const& otherPosition = otherTransform.GetPosition();

                    localCollided = false;

                    if (collider.shape == Collider::ColliderShapes::CIRCLE && otherCollider.shape == Collider::ColliderShapes::CIRCLE)
                    {
                        if (DetectCollision(tempPosition, HFEngine::ECS.GetComponent<CircleCollider>(gameObject), otherPosition, HFEngine::ECS.GetComponent<CircleCollider>(otherObject), sepVector))
                        {
                            collided = true;
                            localCollided = true;
                        }
                    } else if (collider.shape == Collider::ColliderShapes::CIRCLE && otherCollider.shape == Collider::ColliderShapes::BOX)
                    {
                        if (DetectCollision(tempPosition, HFEngine::ECS.GetComponent<CircleCollider>(gameObject), otherPosition, -glm::angle(otherTransform.GetRotation()), HFEngine::ECS.GetComponent<BoxCollider>(otherObject), sepVector))
                        {
                            collided = true;
                            localCollided = true;
                        }
                    }

                    if (localCollided)
                    {
                        if (otherCollider.type == Collider::ColliderTypes::DYNAMIC && HFEngine::ECS.SearchComponent<RigidBody>(otherObject))
                        {
                            auto& otherRb = HFEngine::ECS.GetComponent<RigidBody>(otherObject);
                            float massFactor = rigidBody.mass / (rigidBody.mass + otherRb.mass);
                            otherTransform.SetPosition(otherPosition - (sepVector * massFactor));
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
            //if (collided) break;
        }
        transform.SetPosition(tempPosition);
        if (rigidBody.velocity.x * oldVelocity.x < 0) rigidBody.velocity.x = 0.0f;
        if (rigidBody.velocity.z * oldVelocity.z < 0) rigidBody.velocity.z = 0.0f;
        rigidBody.velocity *= 0.75f;
        rigidBody.moved = false;
	}
}

bool PhysicsSystem::DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, const CircleCollider& c2, glm::vec3& sepVector)
{
    float r = c1.radius + c2.radius;
    glm::vec2 c1c2(pos1.x - pos2.x, pos1.z - pos2.z);

    float dist = veclen(c1c2);
    if (dist >= r) return false;

    sepVector.x = (c1c2.x / dist) * (r - dist);
    sepVector.z = (c1c2.y / dist) * (r - dist);

    return true;
}

bool PhysicsSystem::DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, const float& rotation2, const BoxCollider& c2, glm::vec3& sepVector)
{
    // TODO: refaktor. maybe cache?
    float tbx1, tby1, tbx2, tby2;
    tbx1 = -c2.width / 2.0f;
    tby1 = -c2.height / 2.0f;
    tbx2 = c2.width / 2.0f;
    tby2 = c2.height / 2.0f;

    glm::vec2 boxPoints[4] = {
        { tbx1, tby1 },
        { tbx2, tby1 },
        { tbx2, tby2 },
        { tbx1, tby2 }
    };
    glm::vec2 boxPos(pos2.x, pos2.z);
    for (int i = 0; i < 4; i++)
    {
        boxPoints[i] = glm::rotate(boxPoints[i], rotation2) + boxPos;
    }

    glm::vec2 circleCenter(pos1.x, pos1.z);
    glm::vec2 axes[5];
    int nearestPointIndex = 0;
    float minDist = std::numeric_limits<float>::max();
    float tmpDist;
    glm::vec2 temp;

    //Get box axes & nearest point
    for (int i = 0; i < 4; i++)
    {
        auto secondIndex = (i == 3) ? 0 : i + 1;
        axes[i] = glm::normalize(boxPoints[secondIndex] - boxPoints[i]);

        temp = boxPoints[i] - circleCenter;
        tmpDist = veclen(temp);
        if (tmpDist < minDist)
        {
            minDist = tmpDist;
            nearestPointIndex = i;
        }
    }

    temp = boxPoints[nearestPointIndex] - circleCenter;
    axes[4] = glm::normalize(temp);

    float overlap;
    float minCircle, maxCircle, minBox, maxBox;
    float tempProjection;
    int mtvAxisIndex;
    float minOverlap = std::numeric_limits<float>::max();

    for (int i = 0; i < 5; i++)
    {
        tempProjection = glm::dot(circleCenter, axes[i]);
        minCircle = tempProjection - c1.radius;
        maxCircle = tempProjection + c1.radius;

        minBox = glm::dot(boxPoints[0], axes[i]);
        maxBox = minBox;

        for (int j = 1; j < 4; j++)
        {
            tempProjection = glm::dot(boxPoints[j], axes[i]);
            if (tempProjection < minBox) minBox = tempProjection;
            if (tempProjection > maxBox) maxBox = tempProjection;
        }

        auto min1 = std::min(maxCircle, maxBox);
        auto max1 = std::max(minCircle, minBox);
        overlap = min1 - max1;
        if (overlap <= 0.0f) return false;

        if (overlap < minOverlap)
        {
            minOverlap = overlap;
            mtvAxisIndex = i;
        }

    }

    auto p2top1 = glm::vec2(pos2.x - pos1.x, pos2.z - pos1.z);
    if (glm::dot(axes[mtvAxisIndex], p2top1) >= 0)
    {
        axes[mtvAxisIndex] *= -1.0f;
    }
    sepVector.x = axes[mtvAxisIndex].x * minOverlap;
    sepVector.z = axes[mtvAxisIndex].y * minOverlap;

    return true;
}

void PhysicsSystem::SetCollector(std::shared_ptr<ColliderCollectorSystem> colliderCollectorSystem)
{
    this->colliderCollectorSystem = colliderCollectorSystem;
}
