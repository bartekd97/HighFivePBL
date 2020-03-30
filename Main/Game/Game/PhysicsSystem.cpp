#include <iostream>

#include "PhysicsSystem.h"
#include "HFEngine.h"

#include "RigidBody.h"
#include "Transform.h"
#include "Gravity.h"

void PhysicsSystem::Update(float dt)
{
	for (auto const& gameObject : gameObjects)
	{
		auto& rigidBody = HFEngine::ECS.GetComponent<RigidBody>(gameObject);
		auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);

		auto const& gravity = HFEngine::ECS.GetComponent<Gravity>(gameObject);

		transform.position += rigidBody.velocity * dt;
		rigidBody.velocity += gravity.force * dt;
	}
}
