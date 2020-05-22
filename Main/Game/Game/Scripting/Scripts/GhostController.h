#pragma once

#include <glm/gtx/vector_angle.hpp>
#include "../Script.h"
#include "HFEngine.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/SkinAnimator.h"
#include "InputManager.h"
#include "Event/Events.h"
#include "Event/EventManager.h"

#define GetTransform() HFEngine::ECS.GetComponent<Transform>(GetGameObject())
#define GetAnimator() HFEngine::ECS.GetComponent<SkinAnimator>(visualObject)

class GhostController : public Script
{
private: // parameters
	float moveSpeed = 10.0f;

private: // variables
	GameObject visualObject;

	float currentMoveSpeed = 0.0f;
	float moveSpeedSmoothing = 50.0f; // set in Start()
	float rotateSpeedSmoothing = 4.0f * M_PI;

public:
	GhostController()
	{
		RegisterFloatParameter("moveSpeed", &moveSpeed);
	}

	void Awake()
	{
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Ghost::MOVEMENT_START, GhostController::MovementStart));
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Ghost::MOVEMENT_STOP, GhostController::MovementStop));
	}
	void Start()
	{
		HFEngine::ECS.SetEnabledGameObject(GetGameObject(), false);
		visualObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "Visual")[0];
		moveSpeedSmoothing = moveSpeed * 4.0f;
		GetAnimator().SetAnimation("running");
	}

	void MovementStart(Event& event)
	{
		HFEngine::ECS.SetEnabledGameObject(GetGameObject(), true);
		GetTransform().SetPosition({ 0.0f,0.0f,0.0f });
		currentMoveSpeed = 0.0f;
	}
	void MovementStop(Event& event)
	{
		HFEngine::ECS.SetEnabledGameObject(GetGameObject(), false);
	}


	void Update(float dt)
	{
		auto& transform = GetTransform();

		glm::vec2 cursorPos = InputManager::GetMousePosition();
		glm::vec3 world = HFEngine::MainCamera.ScreenToWorldPosition(cursorPos, 0.0f);
		glm::vec3 worldDirection = HFEngine::MainCamera.ScreenToWorldPosition(cursorPos, 1.0f);
		worldDirection = world - worldDirection;
		worldDirection /= worldDirection.y;
		glm::vec3 worldZero = world - (worldDirection * world.y);

		glm::vec3 direction = glm::normalize(worldZero - transform.GetWorldPosition());
		// smooth rotate
		{
			glm::vec3 front3 = transform.GetWorldFront();
			float diff = glm::orientedAngle(
				glm::normalize(glm::vec2(direction.x, direction.z)),
				glm::normalize(glm::vec2(front3.x, front3.z))
				);

			float change = dt * rotateSpeedSmoothing;
			if (glm::abs(change) > glm::abs(diff))
				change = diff;
			else
				change *= glm::sign(diff);

			if (glm::abs(change) > 0.01f)
				transform.RotateSelf(glm::degrees(change), transform.GetUp());
		}

		// smoth move speed
		float targetMoveSpeed = moveSpeed;
		{
			float diff = targetMoveSpeed - currentMoveSpeed;
			float change = dt * moveSpeedSmoothing;
			if (glm::abs(change) > glm::abs(diff))
				currentMoveSpeed = targetMoveSpeed;
			else
				currentMoveSpeed += change * glm::sign(diff);
		}

		if (currentMoveSpeed > 0.01f)
			transform.TranslateSelf(currentMoveSpeed * dt, transform.GetFront());
	}
};


#undef GetTransform()
#undef GetAnimator()