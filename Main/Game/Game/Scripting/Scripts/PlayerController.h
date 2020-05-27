#pragma once

#include <glm/gtx/vector_angle.hpp>
#include "../Script.h"
#include "HFEngine.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/SkinAnimator.h"
#include "InputManager.h"
#include "Event/Events.h"
#include "../../Physics/Physics.h"
#include "../../Rendering/PrimitiveRenderer.h"
#include "Physics/Raycaster.h"

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

	bool hasGhostMovement = false;
	Raycaster raycaster;

public:
	PlayerController()
	{
		RegisterFloatParameter("moveSpeed", &moveSpeed);
	}

	void Awake()
	{
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Ghost::MOVEMENT_START, PlayerController::GhostMovementStart));
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Ghost::MOVEMENT_STOP, PlayerController::GhostMovementStop));
		raycaster.SetIgnoredGameObject(GetGameObject());
	}

	void Start()
	{
		visualObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "Visual")[0];
		startPosition = GetTransform().GetWorldPosition();
		moveSpeedSmoothing = moveSpeed * 4.0f;
		GetAnimator().SetAnimation("idle");
	}


	void GhostMovementStart(Event& event) { hasGhostMovement = true; }
	void GhostMovementStop(Event& event) { hasGhostMovement = false; }


	void Update(float dt)
	{
		auto& transform = GetTransform();
		auto& animator = GetAnimator();
		auto& rigidBody = GetRigidBody();

		bool isMoving = UpdateMovement(dt);

		if (isMoving)
			animator.TransitToAnimation("running", 0.2f);
		else
			animator.TransitToAnimation("idle", 0.2f);

		if (InputManager::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
			EventManager::FireEvent(Events::Gameplay::Ghost::MOVEMENT_START);
		else if (InputManager::GetMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT))
			EventManager::FireEvent(Events::Gameplay::Ghost::MOVEMENT_STOP);

		if (InputManager::GetKeyDown(GLFW_KEY_X))
		{
			rigidBody.isFalling = true;
			transform.TranslateSelf(glm::vec3(0.0f, 15.0f, 0.0f));
		}

		if (InputManager::GetKeyStatus(GLFW_KEY_R))
		{
			auto pos = transform.GetWorldPosition();
			auto dir = glm::normalize(transform.GetWorldFront());
			if (raycaster.Raycast(pos, dir))
			{
				PrimitiveRenderer::DrawLine(pos, raycaster.GetOut().hitPosition);
			}
		}

		if (InputManager::GetKeyDown(GLFW_KEY_B))
		{
			auto pos = transform.GetWorldPosition();
			BoxCollider box;
			box.SetWidthHeight(1.5f, 1.5f);
			auto rot = transform.GetWorldRotation();
			RaycastHit out;
			if (Physics::Raycast(pos, rot, box, out, GetGameObject()))
			{
				LogInfo("PlayerController: raycast box ({}, {}): hitted object {}", box.width, box.height, out.hittedObject);
			}
		}

		if (InputManager::GetKeyDown(GLFW_KEY_C))
		{
			auto pos = transform.GetWorldPosition();
			CircleCollider circle;
			circle.radius = 1.0f;
			RaycastHit out;
			if (Physics::Raycast(pos, circle, out, GetGameObject()))
			{
				LogInfo("PlayerController: raycast circle ({}): hitted object {}", circle.radius, out.hittedObject);
			}
		}

		if (transform.GetWorldPosition().y < -15.0f)
		{
			transform.SetPosition(startPosition);
		}

	}

	bool UpdateMovement(float dt)
	{
		auto& transform = GetTransform();
		auto& rigidBody = GetRigidBody();

		glm::vec3 direction(0.0f);
		if (!hasGhostMovement)
		{
			if (InputManager::GetKeyStatus(GLFW_KEY_A)) direction.x = -1.0f;
			else if (InputManager::GetKeyStatus(GLFW_KEY_D)) direction.x = 1.0f;

			if (InputManager::GetKeyStatus(GLFW_KEY_W)) direction.z = -1.0f;
			else if (InputManager::GetKeyStatus(GLFW_KEY_S)) direction.z = 1.0f;
		}

		bool isMoving = glm::length2(direction) > 0.5f;

		// smooth move rotation
		if (isMoving)
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

			if (glm::abs(change) > 0.01f)
				transform.RotateSelf(glm::degrees(change), transform.GetUp());
		}

		// smoth move speed
		float targetMoveSpeed = isMoving ? moveSpeed : 0.0f;
		{
			float diff = targetMoveSpeed - currentMoveSpeed;
			float change = dt * moveSpeedSmoothing;
			if (glm::abs(change) > glm::abs(diff))
				currentMoveSpeed = targetMoveSpeed;
			else
				currentMoveSpeed += change * glm::sign(diff);
		}

		if (currentMoveSpeed > 0.01f)
			rigidBody.Move(transform.GetPosition() + (transform.GetFront() * currentMoveSpeed * dt));

		return isMoving;
	}
};


#undef GetTransform()
#undef GetAnimator()
#undef GetRigidBody()