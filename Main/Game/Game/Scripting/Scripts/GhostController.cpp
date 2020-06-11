#include "GhostController.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/ScriptContainer.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/SkinAnimator.h"
#include "ECS/Components/SkinnedMeshRenderer.h"
#include "ECS/Components/PointLightRenderer.h"
#include "InputManager.h"
#include "Event/Events.h"
#include "Event/EventManager.h"
#include "Physics/Physics.h"
#include "EnemyController.h"

#include "HFEngine.h"

#define GetTransform() HFEngine::ECS.GetComponent<Transform>(GetGameObject())
#define GetAnimator() HFEngine::ECS.GetComponent<SkinAnimator>(visualObject)

void GhostController::Awake()
{
	miniGhostPrefab = PrefabManager::GetPrefab("MiniGhost");
	miniGhostPrefab->MakeWarm();
	EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Ghost::MOVEMENT_START, GhostController::MovementStart));
	EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Ghost::MOVEMENT_STOP, GhostController::MovementStop));
}

void GhostController::Start()
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

	auto& collider = HFEngine::ECS.GetComponent<Collider>(GetGameObject());
	collider.OnTriggerEnter.push_back(TriggerMethodPointer(GhostController::OnTriggerEnter));
}

void GhostController::OnTriggerEnter(GameObject that, GameObject other)
{
	if (!strcmp(HFEngine::ECS.GetNameGameObject(other), "enemy") && numberOfEnemyHit < numberOfEnemyToHit)
	{
		auto& scriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(other);
		auto enemyController = scriptContainer.GetScript<EnemyController>();
		enemyController->TakeDamage(damageToEnemies);
		numberOfEnemyHit += 1;
	}
}

void GhostController::MovementStart(Event& event)
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

void GhostController::MovementStop(Event& event)
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

void GhostController::Update(float dt)
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

float GhostController::GetRotationdifferenceToMousePosition()
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
void GhostController::StartMarking()
{
	auto& transform = GetTransform();
	glm::vec3 transformPosition = transform.GetPosition();

	distanceReached = 0.0f;
	lastDistanceRecordPos = transformPosition;

	recordedPositions.clear();
	recordedPositions.emplace_back(glm::vec2{
		transformPosition.x,
		transformPosition.z
		});
	spawnedMiniGhostsCurrent.clear();

	numberOfEnemyHit = 0;

	/*
	IsMarking = true;
	firstEnemyHit = true;
	bossHit = true;
	numberOfEnemyHit = 0;
	*/
}
void GhostController::UpdateMarking()
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

		spawnedMiniGhostsCurrent.emplace_back(mg);
		recordedPositions.emplace_back(glm::vec2{
			transformPosition.x,
			transformPosition.z
			});

		distanceReached = 0.0f;
	}
}
void GhostController::EndMarking()
{
	auto& transform = GetTransform();
	glm::vec3 transformPosition = transform.GetPosition();

	recordedPositions.emplace_back(glm::vec2{
		transformPosition.x,
		transformPosition.z
		});


	if (spawnedMiniGhostsCurrent.size() > 0)
	{
		auto gline = std::make_shared<GhostLine>();
		gline->points = recordedPositions;
		gline->ghosts = spawnedMiniGhostsCurrent;
		activeLines.emplace_back(gline);

		CheckNewLineCrossings(gline);
		CheckClosedLines();

		if (activeLines.size() > maxActiveLines)
		{
			//while (activeLines.Count > 0)
			FadeOutLine(activeLines[0]);
		}
	}

	recordedPositions.clear();
	spawnedMiniGhostsCurrent.clear();
}



void GhostController::CheckNewLineCrossings(std::shared_ptr<GhostLine> newLine)
{
	std::vector<std::shared_ptr<GhostCrossing>> crossings;
	for (auto& line : activeLines)
	{
		if (line == newLine)
			continue;

		crossings.clear();

		CalculateCrossings(line, newLine, crossings);
		for (auto& crossing : crossings)
		{
			line->crosings.insert(crossing);
			newLine->crosings.insert(crossing);
		}
	}
}

void GhostController::CheckClosedLines()
{
	std::unordered_set<std::shared_ptr<GhostLine>> possibleLines;
	for (auto& l : activeLines)
		if (l->crosings.size() >= 2)
			possibleLines.insert(l);

	std::unordered_set<std::shared_ptr<GhostCrossing>> figureCrossings;
	for (auto& l : possibleLines)
		for (auto& c : l->crosings)
		{
			if (possibleLines.contains(c->a) && possibleLines.contains(c->b))
				figureCrossings.insert(c);
		}

	if (figureCrossings.size() >= possibleLines.size())
	{
		AttackWithClosedFigure(possibleLines, figureCrossings);
		return;
	}
}

void GhostController::FadeOutLine(std::shared_ptr<GhostLine> ghostLine)
{
	for (auto g : ghostLine->ghosts)
	{
		EventManager::FireEventTo(g, Events::Gameplay::MiniGhost::FADE_ME_OUT);
	}

	RemoveGhostLineFromData(ghostLine);
}


void GhostController::RemoveGhostLineFromData(std::shared_ptr<GhostLine> ghostLine)
{
	activeLines.erase(std::find(activeLines.begin(), activeLines.end(), ghostLine));
	// clear crossings
	for (auto& line : activeLines)
		for (auto& crossing : ghostLine->crosings)
			line->crosings.erase(crossing);
	ghostLine->crosings.clear();
}

void GhostController::AttackWithClosedFigure(
	std::unordered_set<std::shared_ptr<GhostLine>>& lines,
	std::unordered_set<std::shared_ptr<GhostCrossing>>& crossings)
{
	glm::vec2 center = { 0.0f, 0.0f };
	for (const auto& c : crossings)
		center += c->position;
	center /= (float)crossings.size();

	glm::vec3 center3 = { center.x, 0.0f, center.y };
	for (const auto& line : lines)
	{
		glm::vec3 targetDir = { 0.0f, 0.0f, 0.0f };

		for (auto const& g : line->ghosts)
			targetDir += glm::normalize(center3 - HFEngine::ECS.GetComponent<Transform>(g).GetPosition());

		targetDir /= line->ghosts.size();

		Event ev(Events::Gameplay::MiniGhost::ATTACK);
		ev.SetParam<glm::vec3>(Events::Gameplay::MiniGhost::Direction, targetDir);
		for (auto const& g : line->ghosts)
			EventManager::FireEventTo(g, ev);

		RemoveGhostLineFromData(line);
	}
}

void GhostController::CalculateCrossings(
	std::shared_ptr<GhostLine>& l1,
	std::shared_ptr<GhostLine>& l2,
	std::vector<std::shared_ptr<GhostCrossing>>& crossings)
{
	glm::vec2* pi = l1->points.data();
	glm::vec2* pj;
	for (int i = 0; i < l1->points.size() - 1; i++, pi++)
	{
		pj = l2->points.data();
		for (int j = 0; j < l2->points.size() - 1; j++, pj++)
		{
			glm::vec2 p = Utility::GetSegmentsCommonPoint(
				*pi, *(pi + 1),
				*pj, *(pj + 1)
			);
			if (glm::length2(p) > 0.0001f) {
				std::shared_ptr<GhostCrossing> crossing = std::make_shared<GhostCrossing>();
				crossing->a = l1;
				crossing->b = l2;
				crossing->position = p;
				crossings.push_back(crossing);
			}
		}
	}
}