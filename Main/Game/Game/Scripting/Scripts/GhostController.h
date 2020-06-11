#pragma once

#include <glm/gtx/vector_angle.hpp>
#include "../Script.h"
/*#include "HFEngine.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/SkinAnimator.h"
#include "ECS/Components/SkinnedMeshRenderer.h"
#include "ECS/Components/PointLightRenderer.h"
#include "InputManager.h"
#include "Event/Events.h"
#include "Event/EventManager.h"*/
#include "Utility/TimerAnimator.h"
#include "Resourcing/Prefab.h"
#include "../Types/GhostTypes.h"



class GhostController : public Script
{
private: // parameters
	float moveSpeed = 10.0f;
	float damageToEnemies = 5.0f;

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

	int numberOfEnemyToHit = 1;
	int numberOfEnemyHit = 0;

public:
	float maxGhostDistance = 20.0f;
	float ghostDistanceRecoverySpeed = 5.0f;
	float leftGhostDistance;

	float miniGhostSpawnDistance = 1.5f;
	int maxActiveLines = 4;

	std::vector<std::shared_ptr<GhostLine>> activeLines;

	std::vector<glm::vec2> recordedPositions;
	float distanceReached;
	glm::vec3 lastDistanceRecordPos;
	std::vector<GameObject> spawnedMiniGhostsCurrent;


	GhostController()
	{
		RegisterFloatParameter("moveSpeed", &moveSpeed);
		RegisterFloatParameter("damageToEnemies", &damageToEnemies);
		RegisterIntParameter("numberOfEnemyToHit", &numberOfEnemyToHit);
	}

	void Awake();
	void Start();

	void OnTriggerEnter(GameObject that, GameObject other);

	inline float GetLeftGhostLevel() { return leftGhostDistance / maxGhostDistance; }

	void MovementStart(Event& event);
	void MovementStop(Event& event);


	void Update(float dt);

	float GetRotationdifferenceToMousePosition();

	// GHOST MARKING FUNCTIONS
	void StartMarking();
	void UpdateMarking();
	void EndMarking();

	void CheckNewLineCrossings(std::shared_ptr<GhostLine> newLine);

	void CheckClosedLines();

	void FadeOutLine(std::shared_ptr<GhostLine> ghostLine);


	void RemoveGhostLineFromData(std::shared_ptr<GhostLine> ghostLine);

	void AttackWithClosedFigure(
		std::unordered_set<std::shared_ptr<GhostLine>>& lines,
		std::unordered_set<std::shared_ptr<GhostCrossing>>& crossings);

	void CalculateCrossings(
		std::shared_ptr<GhostLine>& l1,
		std::shared_ptr<GhostLine>& l2,
		std::vector<std::shared_ptr<GhostCrossing>>& crossings);
};
