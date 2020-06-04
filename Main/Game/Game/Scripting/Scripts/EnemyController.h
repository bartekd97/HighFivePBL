#pragma once

#include <deque>
#include <glm/gtx/vector_angle.hpp>
#include "../Script.h"
#include "HFEngine.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/SkinAnimator.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "ECS/Components/CellPathfinder.h"
#include "Event/Events.h"
#include "Event/EventManager.h"
#include "Rendering/PrimitiveRenderer.h"

#define GetTransform() HFEngine::ECS.GetComponent<Transform>(GetGameObject())
#define GetPathfinder() HFEngine::ECS.GetComponent<CellPathfinder>(GetGameObject())
#define GetAnimator() HFEngine::ECS.GetComponent<SkinAnimator>(visualObject)
#define GetRigidBody() HFEngine::ECS.GetComponent<RigidBody>(GetGameObject())

class EnemyController : public Script
{
private: // parameters
	float moveSpeed = 5.0f;

private: // variables
	GameObject visualObject;

	float currentMoveSpeed = 0.0f;
	float moveSpeedSmoothing = 50.0f; // set in Start()
	float rotateSpeedSmoothing = 2.0f * M_PI;

	std::deque<glm::vec3> targetPath;
	float nextPointMinDistance2 = 2.0f;

	int PathUpdateFrameMod = 4; // every 4rd frame queue pathfinding

	GameObject playerObject;
	GameObject cellObject;

public:

	EnemyController()
	{
		RegisterFloatParameter("moveSpeed", &moveSpeed);
	}


	void Start()
	{
		if (!HFEngine::ECS.SearchComponent<CellPathfinder>(GetGameObject()))
			HFEngine::ECS.AddComponent<CellPathfinder>(GetGameObject(), {});
		GetPathfinder().onPathReady = PathReadyMethodPointer(EnemyController::OnNewPathToPlayer);

		visualObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "Visual")[0];
		moveSpeedSmoothing = moveSpeed * 4.0f;
		GetAnimator().SetAnimation("move"); // TODO: idle

		playerObject = HFEngine::ECS.GetGameObjectByName("Player").value();
		cellObject = HFEngine::ECS.GetComponent<CellChild>(GetGameObject()).cell;
	}


	bool CanQueuePathThisFrame()
	{
		unsigned long long num = GetGameObject() + HFEngine::CURRENT_FRAME_NUMBER;
		return num % PathUpdateFrameMod == 0;
	}

	//float pathdt = 0.0f;
	void Update(float dt)
	{
		auto& transform = GetTransform();
		auto& rigidBody = GetRigidBody();
		auto& pathfinder = GetPathfinder();

		// update test path
		if (CanQueuePathThisFrame())
		{
			glm::vec3 playerPos = HFEngine::ECS.GetComponent<Transform>(playerObject).GetPosition();
			if (glm::distance2(playerPos, transform.GetPosition()) < 900.0f
				&& glm::distance2(playerPos, pathfinder.GetCurrentTargetPosition()) > 1.0f)
				pathfinder.QueuePath(playerPos);
		}


		auto targetPoint = GetTargetPoint();
		if (targetPoint.has_value())
		{
			// smooth rotate
			float diff = GetRotationdifferenceToPoint(targetPoint.value());
			float change = dt * rotateSpeedSmoothing;
			if (glm::abs(change) > glm::abs(diff))
				change = diff;
			else
				change *= glm::sign(diff);

			if (glm::abs(change) > 0.01f)
				transform.RotateSelf(glm::degrees(change), transform.GetUp());
		}

		// smoth move speed
		float targetMoveSpeed = targetPoint.has_value() ? moveSpeed : 0.0f;
		{
			float diff = targetMoveSpeed - currentMoveSpeed;
			float change = dt * moveSpeedSmoothing;
			if (glm::abs(change) > glm::abs(diff))
				currentMoveSpeed = targetMoveSpeed;
			else
				currentMoveSpeed += change * glm::sign(diff);
		}

		auto moveBy = (currentMoveSpeed * dt) * transform.GetFront();
		if (currentMoveSpeed > 0.01f)
			rigidBody.Move(transform.GetPosition() + moveBy);
	}



	void OnNewPathToPlayer(const std::deque<glm::vec3>& path)
	{
		targetPath.clear();
		for (auto& point : path)
			targetPath.push_back(point);

		if (targetPath.size() > 0)
			targetPath.pop_front(); // get rid of first point cuz its may be too near

		//LogInfo("EnemyController::OnNewPathToPlayer(): Path size: {}", targetPath.size());
	}



	void LateUpdate(float dt)
	{
#ifdef HF_DEBUG_RENDER
		glm::vec3 last = GetTransform().GetPosition();
		for (auto p : targetPath)
		{
			PrimitiveRenderer::DrawLine(last, p);
			last = p;
		}
#endif
	}


	std::optional<glm::vec3> GetTargetPoint()
	{
		auto& transform = GetTransform();
		while (targetPath.size() > 1
			&& glm::distance2(targetPath[0], transform.GetPosition()) >
			glm::distance2(targetPath[1], transform.GetPosition()))
			targetPath.pop_front();
		while (targetPath.size() > 0
			&& glm::distance2(targetPath.front(), transform.GetPosition()) < nextPointMinDistance2)
			targetPath.pop_front();

		if (targetPath.size() == 0)
			return {};
		else
			return targetPath.front();
	}

	float GetRotationdifferenceToPoint(glm::vec3 point)
	{
		auto& transform = GetTransform();

		glm::vec3 direction = glm::normalize(point - transform.GetPosition());
		glm::vec3 front3 = transform.GetFront();
		float diff = glm::orientedAngle(
			glm::normalize(glm::vec2(direction.x, direction.z)),
			glm::normalize(glm::vec2(front3.x, front3.z))
			);

		return diff;
	}
};


#undef GetTransform()
#undef GetPathfinder()
#undef GetAnimator()
#undef GetRigidBody()