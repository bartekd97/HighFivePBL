#include <iostream>

#include "PhysicsSystem.h"
#include "HFEngine.h"

#include "../Components.h"

#define clamp(x,min,max) x > min ? (x < max ? x : max) : min
#define veclen(v) std::sqrtf(v.x*v.x + v.y*v.y);

void PhysicsSystem::Init()
{
	step = 0.05f;
}

/*
 * TODO LIST:
 * -refactor
 * -what about transform scale?
 * -what about moving dynamic colliders into another colliders (no collision detection when moved inside loops)?
 * -what about rigidBody requirement?
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

        if (islessequal(length, step)) continue;
        int steps = std::max(1, (int)std::round(length / step));
		glm::vec3 moveStep = displacement / (float)steps;
        glm::vec3 tempPosition = transform.GetPosition();
        glm::vec3 sepVector(0.0f);
        glm::vec3 oldVelocity = rigidBody.velocity;

        auto& collider = HFEngine::ECS.GetComponent<Collider>(gameObject);

        for (int s = 0; s < steps; s++)
        {
            tempPosition += moveStep;
            transform.SetPosition(tempPosition);
            
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
                        if (DetectCollision(tempPosition, HFEngine::ECS.GetComponent<CircleCollider>(gameObject), otherPosition, otherTransform.GetRotationEuler().y, HFEngine::ECS.GetComponent<BoxCollider>(otherObject), sepVector))
                        {
                            collided = true;
                            localCollided = true;
                        }
                    }

                    if (localCollided)
                    {
                        if (otherCollider.type == Collider::ColliderTypes::DYNAMIC)
                        {
                            sepVector *= 0.5f; // mass?
                            //auto& otherRb = HFEngine::ECS.GetComponent<RigidBody>(otherObject);
                            otherTransform.SetPosition(otherPosition - sepVector); // rb move?
                            //otherRb.velocity.x -= sepVector.x / dt;
                            //otherRb.velocity.z -= sepVector.y / dt;
                        }

                        tempPosition += sepVector;
                        rigidBody.velocity.x += sepVector.x / dt;
                        rigidBody.velocity.z += sepVector.y / dt;
                    }
                }
            }
            if (collided) break;
        }
        transform.SetPosition(tempPosition);
        //if (rigidBody.velocity.x * oldVelocity.x < 0) rigidBody.velocity.x = 0.0f;
        //if (rigidBody.velocity.z * oldVelocity.z < 0) rigidBody.velocity.z = 0.0f;
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
    float tbx1, tby1, tbx2, tby2;
    float bx1, by1, bx2, by2;
    tbx1 = -c2.width / 2.0f;
    tby1 = -c2.height / 2.0f;
    tbx2 = c2.width / 2.0f;
    tby2 = c2.height / 2.0f;

    float rrotation2 = glm::radians(rotation2);

    bx1 = (tbx1 * cos(rotation2)) - (tby1 * sin(rotation2)) + pos2.x;
    by1 = (tbx1 * sin(rotation2)) + (tby1 * cos(rotation2)) + pos2.z;
    bx2 = (tbx2 * cos(rotation2)) - (tby2 * sin(rotation2)) + pos2.x;
    by2 = (tbx2 * sin(rotation2)) + (tby2 * cos(rotation2)) + pos2.z;

    //float closestX = clamp(pos1.x, bx1, bx2);
    //float closestY = clamp(pos1.z, by1, by2);

    // Calculate the distance between the circle's center and this closest point
    //float distanceX = pos1.x - closestX;
    //float distanceY = pos1.z - closestY;

    glm::vec2 f = { clamp(pos1.x, bx1, bx2), clamp(pos1.z, by1, by2) };
    glm::vec2 cf = { pos1.x - f.x, pos1.z - f.y };
    float dist = veclen(cf);
    if (dist >= c1.radius)  return false;

    if (dist < 0.001f)
    {
        float left = pos1.x - bx1 + c1.radius, right = bx2 - pos1.x + c1.radius;
        float top = pos1.y - by1 + c1.radius, bottom = by2 - pos1.y + c1.radius;

        left < right ? sepVector.x = -left : sepVector.x = right;
        top < bottom ? sepVector.z = -top : sepVector.y = bottom;
        if (abs(sepVector.x) < abs(sepVector.z))
            sepVector.z = 0;
        else if (abs(sepVector.x) > abs(sepVector.z))
            sepVector.x = 0;
    }
    else
    {
        sepVector.x = (cf.x / dist) * (c1.radius - dist);
        sepVector.z = (cf.y / dist) * (c1.radius - dist);
    }

    return true;

    // If the distance is less than the circle's radius, an intersection occurs
    /*float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
    if (distanceSquared < (c1.radius * c1.radius))
    {
        sepVector.x = distanceX;
        sepVector.z = distanceY;

        return true;
    }
    return false;*/
}

void PhysicsSystem::SetCollector(std::shared_ptr<ColliderCollectorSystem> colliderCollectorSystem)
{
    this->colliderCollectorSystem = colliderCollectorSystem;
}
