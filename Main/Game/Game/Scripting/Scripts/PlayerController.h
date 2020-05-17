#pragma once

#include <glm/gtx/vector_angle.hpp>
#include "../Script.h"
#include "HFEngine.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/SkinAnimator.h"
#include "InputManager.h"

#define GetTransform() HFEngine::ECS.GetComponent<Transform>(GetGameObject())
#define GetAnimator() HFEngine::ECS.GetComponent<SkinAnimator>(visualObject)
#define GetRigidBody() HFEngine::ECS.GetComponent<RigidBody>(GetGameObject())

class PlayerController : public Script
{
private: // parameters
	float moveSpeed = 10.0f;

private: // variables
	glm::vec3 startPosition;
	GameObject visualObject;

	float currentMoveSpeed = 0.0f;
	float moveSpeedSmoothing; // set in Start()
	float rotateSpeedSmoothing = 4.0f * M_PI;

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
		visualObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "Visual")[0];
		startPosition = GetTransform().GetWorldPosition();
		moveSpeedSmoothing = moveSpeed * 4.0f;
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

		bool isKeyMoving = glm::length2(direction) > 0.5f;

		if (isKeyMoving)
			animator.TransitToAnimation("running", 0.2f);
		else
			animator.TransitToAnimation("idle", 0.2f);

		// smooth move rotation
		if (isKeyMoving)
		{
			glm::vec3 front3 = transform.GetFront();
			float diff = glm::orientedAngle(
				glm::normalize(glm::vec2(direction.x, direction.z)),
				glm::normalize(glm::vec2(front3.x, front3.z))
			);

			float change = dt * rotateSpeedSmoothing;
			if (glm::abs(change) > glm::abs(diff))
				change = diff;
			else
				change *= glm::sign(diff);
			transform.RotateSelf(glm::degrees(change), transform.GetUp());
		}

		// smoth move speed
		float targetMoveSpeed = isKeyMoving ? moveSpeed : 0.0f;
		{
			float diff = targetMoveSpeed - currentMoveSpeed;
			float change = dt * moveSpeedSmoothing;
			if (glm::abs(change) > glm::abs(diff))
				currentMoveSpeed = targetMoveSpeed;
			else
				currentMoveSpeed += change * glm::sign(diff);
		}


		if (InputManager::GetKeyDown(GLFW_KEY_X))
		{
			rigidBody.isFalling = true;
			transform.TranslateSelf(glm::vec3(0.0f, 15.0f, 0.0f));
		}

		rigidBody.Move(transform.GetPosition() + (transform.GetFront() * currentMoveSpeed * dt));

		if (transform.GetWorldPosition().y < -15.0f)
		{
			transform.SetPosition(startPosition);
		}
	}
};


#undef GetTransform()
#undef GetAnimator()
#undef GetRigidBody()