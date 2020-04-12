#include <iostream>

#include "PhysicsSystem.h"
#include "HFEngine.h"

#include "../Components.h"

void PhysicsSystem::Init()
{
	step = 0.05f;
}

// TODO: move it
bool DetectCollision(float x, float y, float radius, float x2, float y2, float radius2)
{
    float r = radius + radius2;
    r *= r;
    return r >= ((x - x2) * (x - x2)) + ((y - y2) * (y - y2));
}

void PhysicsSystem::Update(float dt)
{
	for (auto const& gameObject : gameObjects)
	{
		auto& rigidBody = HFEngine::ECS.GetComponent<RigidBody>(gameObject);
		auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
        auto const& collider = HFEngine::ECS.GetComponent<CircleCollider>(gameObject);

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

        for (int s = 0; s < steps; s++)
        {
            tempPosition += moveStep;
            bool collided = false;

            for (auto const& otherObject : gameObjects)
            {
                if (gameObject != otherObject)
                {
                    auto& otherRb = HFEngine::ECS.GetComponent<RigidBody>(otherObject);
                    auto& otherTransform = HFEngine::ECS.GetComponent<Transform>(otherObject);
                    auto const& otherCollider = HFEngine::ECS.GetComponent<CircleCollider>(otherObject);
                    auto const& otherPosition = otherTransform.GetPosition();

                    collided = DetectCollision(tempPosition.x, tempPosition.z, collider.radius, otherPosition.x, otherPosition.z, otherCollider.radius);
                    if (collided) break;
                }
            }
            if (collided)
            {
                rigidBody.velocity = glm::vec3(0.0f);
                break;
            }
            else
            {
                transform.SetPosition(tempPosition);
            }
        }
        rigidBody.moved = false;
	}
}
