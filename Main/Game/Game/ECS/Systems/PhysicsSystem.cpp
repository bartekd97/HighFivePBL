#include <iostream>

#include "PhysicsSystem.h"
#include "HFEngine.h"

#include "../Components.h"

#define veclen(v) std::sqrtf(v.x*v.x + v.y*v.y);

void PhysicsSystem::Init()
{
	step = 0.05f;
}

void PhysicsSystem::Update(float dt)
{
    bool collided, localCollided;
	for (auto const& gameObject : gameObjects)
	{
		auto& rigidBody = HFEngine::ECS.GetComponent<RigidBody>(gameObject);
		auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
        auto& collider = HFEngine::ECS.GetComponent<Collider>(gameObject);

		glm::vec3 displacement;
		if (rigidBody.moved)
		{
			displacement = rigidBody.movePosition - transform.GetPosition();
		}
		else
		{
			displacement = rigidBody.velocity * dt;
		}

		float length = sqrt((displacement.x * displacement.x) + (displacement.z * displacement.z));

        if (islessequal(length, step)) continue;
        int steps = std::max(1, (int)std::round(length / step));
		glm::vec3 moveStep = displacement / (float)steps;
        glm::vec3 tempPosition = transform.GetPosition();
        glm::vec3 sepVector(0.0f);
        glm::vec3 oldVelocity = rigidBody.velocity;

        for (int s = 0; s < steps; s++)
        {
            tempPosition += moveStep;
            transform.SetPosition(tempPosition);
            
            collided = false;
            for (auto const& otherObject : gameObjects)
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
                    }

                    if (localCollided)
                    {
                        if (otherCollider.type == Collider::ColliderTypes::DYNAMIC)
                        {
                            sepVector *= 0.5f;
                            //auto& otherRb = HFEngine::ECS.GetComponent<RigidBody>(otherObject);
                            otherTransform.SetPosition(otherPosition - sepVector);
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
