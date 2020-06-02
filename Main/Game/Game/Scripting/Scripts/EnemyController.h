#pragma once

#include <deque>
#include <glm/gtx/vector_angle.hpp>
#include "../Script.h"
#include "HFEngine.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/SkinAnimator.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "Event/Events.h"
#include "Event/EventManager.h"
#include "Rendering/PrimitiveRenderer.h"
#include "Utility/Pathfinding.h"

#define GetTransform() HFEngine::ECS.GetComponent<Transform>(GetGameObject())
#define GetAnimator() HFEngine::ECS.GetComponent<SkinAnimator>(visualObject)

class EnemyController : public Script
{
private: // parameters
	float moveSpeed = 5.0f;

private: // variables
	GameObject visualObject;

	float currentMoveSpeed = 0.0f;
	float moveSpeedSmoothing = 50.0f; // set in Start()
	float rotateSpeedSmoothing = 1.0f * M_PI;

	std::deque<glm::vec3> targetPath;
	float nextPointMinDistance2 = 1.5f;

	GameObject playerObject;
	GameObject cellObject;

public:

	EnemyController()
	{
		RegisterFloatParameter("moveSpeed", &moveSpeed);
	}


	void Start()
	{
		visualObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "Visual")[0];
		moveSpeedSmoothing = moveSpeed * 4.0f;
		GetAnimator().SetAnimation("move"); // TODO: idle

		playerObject = HFEngine::ECS.GetGameObjectByName("Player").value();
		cellObject = HFEngine::ECS.GetComponent<CellChild>(GetGameObject()).cell;
	}


	float pathdt = 0.0f;
	void Update(float dt)
	{
		auto& transform = GetTransform();

		// update test path
		{
			pathdt += dt;
			if (pathdt >= 1.0f)
			{
				glm::vec3 playerPos = HFEngine::ECS.GetComponent<Transform>(playerObject).GetWorldPosition();
				if (glm::distance(playerPos, transform.GetWorldPosition()) < 20.0f)
					CalculatePathToPlayer(playerPos);

				pathdt -= 1.0f;
			}
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
			transform.TranslateSelf(moveBy);
	}





	void CalculatePathToPlayer(glm::vec3& playerPos) // in world space
	{
		auto& transform = GetTransform();
		glm::vec3 myPos = transform.GetWorldPosition(); // in world space
		glm::vec3 cellPos = HFEngine::ECS.GetComponent<Transform>(cellObject).GetWorldPosition(); // in world space
		
		MapCell& cellInfo = HFEngine::ECS.GetComponent<MapCell>(cellObject);
		std::shared_ptr<PathfindingGrid> grid = cellInfo.PathFindingGrid;

		LogInfo("CalculatePathToPlayer");
		// uncomment to clear current path
		targetPath.clear();
		// and assign points for new path from myPos to playerPos with targetPath.push_back();

		// CALCULATE PATH HERE
		LogInfo("EnemyPos: {}", myPos - cellPos);
		LogInfo("PlayerPos: {}", playerPos - cellPos);

		targetPath = grid->FindPath(myPos, playerPos, cellPos);
		LogInfo("path size: {}", targetPath.size());


	}









	void LateUpdate(float dt)
	{
#ifdef _DEBUG
		glm::vec3 last = GetTransform().GetWorldPosition();
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
		while (targetPath.size() > 0
			&& glm::distance2(targetPath.front(), transform.GetWorldPosition()) < nextPointMinDistance2)
			targetPath.pop_front();

		if (targetPath.size() == 0)
			return {};
		else
			return targetPath.front();
	}

	float GetRotationdifferenceToPoint(glm::vec3 point)
	{
		auto& transform = GetTransform();

		glm::vec3 direction = glm::normalize(point - transform.GetWorldPosition());
		glm::vec3 front3 = transform.GetWorldFront();
		float diff = glm::orientedAngle(
			glm::normalize(glm::vec2(direction.x, direction.z)),
			glm::normalize(glm::vec2(front3.x, front3.z))
			);

		return diff;
	}
};


#undef GetTransform()
#undef GetAnimator()