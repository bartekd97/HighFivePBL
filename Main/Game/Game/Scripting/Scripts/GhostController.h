#pragma once

#include <glm/gtx/vector_angle.hpp>
#include "../Script.h"
#include "HFEngine.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/SkinAnimator.h"
#include "ECS/Components/SkinnedMeshRenderer.h"
#include "ECS/Components/PointLightRenderer.h"
#include "InputManager.h"
#include "Event/Events.h"
#include "Event/EventManager.h"
#include "Utility/TimerAnimator.h"
#include "Resourcing/Prefab.h"

#define GetTransform() HFEngine::ECS.GetComponent<Transform>(GetGameObject())
#define GetAnimator() HFEngine::ECS.GetComponent<SkinAnimator>(visualObject)

class GhostController : public Script
{
private: // parameters
	float moveSpeed = 10.0f;

	std::shared_ptr<Prefab> miniGhostPrefab;

private: // variables
	bool hasGhostMovement = false;

	GameObject playerObject;
	GameObject visualObject;
	GameObject ghostLightObject;

	float currentMoveSpeed = 0.0f;
	float moveSpeedSmoothing = 50.0f; // set in Start()
	float rotateSpeedSmoothing = 4.0f * M_PI;

	TimerAnimator timerAnimator;
	float ghostLightDefaultIntensity;
	float ghostMaterialDefaultOpacity;

public:
	float maxGhostDistance = 10.0f;
	float ghostDistanceRecoverySpeed = 3.0f;
	float leftGhostDistance;

	float miniGhostSpawnDistance = 1.5f;
	int maxActiveLines = 4;

	std::vector<glm::vec2> recordedPositions;
	float distanceReached;
	glm::vec3 lastDistanceRecordPos;


	GhostController()
	{
		RegisterFloatParameter("moveSpeed", &moveSpeed);
	}

	void Awake()
	{
		miniGhostPrefab = PrefabManager::GetPrefab("MiniGhost");
		miniGhostPrefab->MakeWarm();
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Ghost::MOVEMENT_START, GhostController::MovementStart));
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Ghost::MOVEMENT_STOP, GhostController::MovementStop));
	}
	void Start()
	{
		playerObject = HFEngine::ECS.GetGameObjectByName("Player").value();

		HFEngine::ECS.SetEnabledGameObject(GetGameObject(), false);
		visualObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "Visual")[0];
		ghostLightObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "GhostLight")[0];

		ghostLightDefaultIntensity = HFEngine::ECS.GetComponent<PointLightRenderer>(ghostLightObject).light.intensity;
		ghostMaterialDefaultOpacity = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(visualObject).material->opacityValue;

		moveSpeedSmoothing = moveSpeed * 4.0f;
		leftGhostDistance = maxGhostDistance;
		GetAnimator().SetAnimation("ghostrunning");
	}

	float GetLeftGhostLevel() { return leftGhostDistance / maxGhostDistance; }

	void MovementStart(Event& event)
	{
		HFEngine::ECS.SetEnabledGameObject(GetGameObject(), true);
		hasGhostMovement = true;

		auto& transform = GetTransform();
		transform.SetPosition(HFEngine::ECS.GetComponent<Transform>(playerObject).GetPosition());
		transform.RotateSelf(
			glm::degrees(GetRotationdifferenceToMousePosition()),
			transform.GetUp()
			);
		currentMoveSpeed = 0.0f;

		auto& ghostLight = HFEngine::ECS.GetComponent<PointLightRenderer>(ghostLightObject);
		auto& ghostMesh = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(visualObject);
		timerAnimator.AnimateVariable(&ghostLight.light.intensity, 0.0f, ghostLightDefaultIntensity, 0.3f);
		timerAnimator.AnimateVariable(&ghostMesh.material->opacityValue, 0.0f, ghostMaterialDefaultOpacity, 0.3f);
	
		StartMarking();
	}
	void MovementStop(Event& event)
	{
		hasGhostMovement = false;

		auto& ghostLight = HFEngine::ECS.GetComponent<PointLightRenderer>(ghostLightObject);
		auto& ghostMesh = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(visualObject);
		timerAnimator.AnimateVariable(&ghostLight.light.intensity, ghostLight.light.intensity, 0.0f, 0.3f);
		timerAnimator.AnimateVariable(&ghostMesh.material->opacityValue, ghostMesh.material->opacityValue, 0.0f, 0.3f);

		timerAnimator.DelayAction(0.5f, [&]() {
			if (ghostLight.light.intensity < 0.0001f)
				HFEngine::ECS.SetEnabledGameObject(GetGameObject(), false);
			});

		EndMarking();
	}


	void Update(float dt)
	{
		timerAnimator.Process(dt);

		if (!hasGhostMovement) return;

		auto& transform = GetTransform();

		UpdateMarking();

		// smooth rotate
		{
			float diff = GetRotationdifferenceToMousePosition();
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

		auto moveBy = (currentMoveSpeed * dt) * transform.GetFront();
		if (currentMoveSpeed > 0.01f)
			transform.TranslateSelf(moveBy);

		leftGhostDistance -= VECLEN(moveBy);
		if (leftGhostDistance <= 0.0f)
		{
			leftGhostDistance = 0.0f;
			EventManager::FireEvent(Events::Gameplay::Ghost::MOVEMENT_STOP);
		}
	}

	float GetRotationdifferenceToMousePosition()
	{
		auto& transform = GetTransform();

		glm::vec2 cursorPos = InputManager::GetMousePosition();
		glm::vec3 world = HFEngine::MainCamera.ScreenToWorldPosition(cursorPos, 0.0f);
		glm::vec3 worldDirection = HFEngine::MainCamera.ScreenToWorldPosition(cursorPos, 1.0f);
		worldDirection = world - worldDirection;
		worldDirection /= worldDirection.y;
		glm::vec3 worldZero = world - (worldDirection * world.y);

		glm::vec3 direction = glm::normalize(worldZero - transform.GetPosition());
		glm::vec3 front3 = transform.GetFront();
		float diff = glm::orientedAngle(
			glm::normalize(glm::vec2(direction.x, direction.z)),
			glm::normalize(glm::vec2(front3.x, front3.z))
			);

		return diff;
	}







	// GHOST MARKING FUNCTIONS
	void StartMarking()
	{
		auto& transform = GetTransform();
		glm::vec3 transformPosition = transform.GetPosition();

		distanceReached = 0.0f;
		lastDistanceRecordPos = transformPosition;

		recordedPositions.clear();
		//recordedLinePositions = new List<Vector3>();

		recordedPositions.emplace_back(glm::vec2{
			transformPosition.x,
			transformPosition.z
		});
		/*
		recordedLinePositions.Add(new Vector3(
			transform.position.x,
			transform.position.y,
			transform.position.z
			));
		recordedLine = SpawnLineGenerator(recordedLinePositions);
		//lastMiniGhostSpawnPosition = transform.position;
		distanceReached = 0.0f;
		lastDistanceRecordPos = transform.position;
		spawnedMiniGhostsCurrent = new List<MiniGhost>();
		IsMarking = true;
		firstEnemyHit = true;
		bossHit = true;
		numberOfEnemyHit = 0;
		*/
	}
	void EndMarking()
	{
		auto& transform = GetTransform();
		glm::vec3 transformPosition = transform.GetPosition();

		recordedPositions.emplace_back(glm::vec2{
			transformPosition.x,
			transformPosition.z
			});

		/*
		recordedLinePositions.Add(new Vector3(
			transform.position.x,
			transform.position.y,
			transform.position.z
			));
		UpdateLineRenderer(recordedLinePositions);

		if (spawnedMiniGhostsCurrent.Count > 0)
		{
			activeLines.Add(new GhostLine()
				{
					//from = startPosition,
					//to = endPosition,
					points = recordedPositions,
					ghosts = spawnedMiniGhostsCurrent,
					lineRend = recordedLine

				});
		}

		//SpawnLineGenerator(recordedLinePositions);
		recordedLine = null;
		*/

		recordedPositions.clear();

		/*
		recordedLinePositions = null;
		spawnedMiniGhostsCurrent = null;

		IsMarking = false;

		UpdateLineCrossings();
		CheckClosedLines();

		if (activeLines.Count > maxActiveLines)
		{
			//while (activeLines.Count > 0)
			FadeOutLine(activeLines[0]);
		}
		*/
	}
	void UpdateMarking()
	{
		auto& transform = GetTransform();
		glm::vec3 transformPosition = transform.GetPosition();

		distanceReached += glm::distance(lastDistanceRecordPos, transformPosition);
		lastDistanceRecordPos = transformPosition;

		if (distanceReached >= miniGhostSpawnDistance)
		{
			GameObject mg = miniGhostPrefab->Instantiate(
				transformPosition,
				transform.GetRotationEuler()
				);

			SkinAnimator* mgAnimator = HFEngine::ECS.GetComponentInChildren<SkinAnimator>(mg).value();
			mgAnimator->SetAnimation("ghostrunning");
			mgAnimator->SetCurrentClipLevel(GetAnimator().GetCurrentClipLevel());
			mgAnimator->SetAnimatorSpeed(0.0f);

			//spawnedMiniGhostsCurrent.Add(mg);
			recordedPositions.emplace_back(glm::vec2{
				transformPosition.x,
				transformPosition.z
			});

			/**
			recordedLinePositions.Add(new Vector3(
				transform.position.x,
				transform.position.y,
				transform.position.z
				));
			UpdateLineRenderer(recordedLinePositions);
			*/
			distanceReached = 0.0f;
		}
	}
};


#undef GetTransform()
#undef GetAnimator()