#pragma once

#include "../Script.h"
#include "HFEngine.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/SkinAnimator.h"
#include "InputManager.h"

#define GetTransform() HFEngine::ECS.GetComponent<Transform>(GetGameObject())
#define GetAnimator() HFEngine::ECS.GetComponent<SkinAnimator>(GetGameObject())
#define GetRigidBody() HFEngine::ECS.GetComponent<RigidBody>(GetGameObject())

class PlayerController : public Script
{
public:
	PlayerController()
	{
		RegisterFloatParameter("moveSpeed", &moveSpeed);
	}

	void Awake()
	{
	}

	void Start()
	{
		startPosition = GetTransform().GetWorldPosition();
		GetAnimator().SetAnimation("idle");
	}

	void Update(float dt)
	{
		auto& transform = GetTransform();
		auto& animator = GetAnimator();
		auto& rigidBody = GetRigidBody();

		glm::vec3 direction(0.0f);
		if (InputManager::GetKeyStatus(GLFW_KEY_A)) direction.x = -1.0f;
		else if (InputManager::GetKeyStatus(GLFW_KEY_D)) direction.x = 1.0f;

		if (InputManager::GetKeyStatus(GLFW_KEY_W)) direction.z = -1.0f;
		else if (InputManager::GetKeyStatus(GLFW_KEY_S)) direction.z = 1.0f;

		if (glm::length2(direction) > 0.5f)
		{
			direction = glm::normalize(direction);
			animator.SetAnimation("running");
		}
		else
		{
			animator.SetAnimation("idle");
		}

		if (InputManager::GetKeyDown(GLFW_KEY_X))
		{
			rigidBody.isFalling = true;
			transform.TranslateSelf(glm::vec3(0.0f, 15.0f, 0.0f));
		}

		if (glm::length2(direction) > 0.5f)
		{
			transform.SetRotation(glm::vec3(0.0f, std::atan2(direction.x, direction.z) * 180.0 / M_PI, 0.0f));
		}
		rigidBody.Move(transform.GetPosition() + (direction * moveSpeed * dt));
		if (transform.GetWorldPosition().y < -15.0f)
		{
			transform.SetPosition(startPosition);
		}
	}


private:
	float moveSpeed = 10.0f;

	glm::vec3 startPosition;
};


#undef GetTransform()
#undef GetAnimator()
#undef GetRigidBody()