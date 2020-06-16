#pragma once

#include <deque>
#include <glm/gtx/vector_angle.hpp>
#include "CreatureController.h"
#include "HFEngine.h"
#include "ECS/Components/SkinAnimator.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "ECS/Components/CellPathfinder.h"
#include "Event/Events.h"
#include "Event/EventManager.h"
#include "Rendering/PrimitiveRenderer.h"
#include "GUI/GUIManager.h"
#include "GUI/Panel.h"
#include "Resourcing/Model.h"
#include "Physics/Raycaster.h"
#include "PlayerController.h"
#include "MapCellOptimizer.h"

#define GetTransform() HFEngine::ECS.GetComponent<Transform>(GetGameObject())
#define GetPathfinder() HFEngine::ECS.GetComponent<CellPathfinder>(GetGameObject())
#define GetAnimator() HFEngine::ECS.GetComponent<SkinAnimator>(visualObject)
#define GetRigidBody() HFEngine::ECS.GetComponent<RigidBody>(GetGameObject())

class EnemyController : public CreatureController
{
private: // parameters
	float dmgAnimationDuration = 0.5f;
	float attackDistance = 1.5f;
	float triggerDistance = 10.0f;
	float attackDamage = 5.0f;

private: // variables
	GameObject visualObject;

	float moveSpeedSmoothing = 50.0f; // set in Start()
	float rotateSpeedSmoothing = 2.0f * M_PI;
	bool isAttacking = false;
	bool midAttack;
	bool triggered = false;

	glm::vec3 defaultColor;
	glm::vec3 damagedColor = { 1.0f, 0.0f, 0.0f };
	TimerAnimator timerAnimator;

	std::deque<glm::vec3> targetPath;
	float nextPointMinDistance2 = 2.0f;

	int PathUpdateFrameMod = 4; // every 4rd frame queue pathfinding

	GameObject playerObject;
	GameObject cellObject;
	std::shared_ptr<PlayerController> playerController;
	std::shared_ptr<MapCellOptimizer> playerCellOptimizer;

	Raycaster raycaster;
	ALuint sourceEnemyController;

	std::shared_ptr<Panel> healthBarPanel;
	std::shared_ptr<Panel> healthRedPanel;
	std::shared_ptr<Panel> healthValuePanel;
	glm::vec2 healthBarSize = { 60.0f, 10.0f };
	float healthBorderSize = 2.0f;

public:

	EnemyController()
	{
		RegisterFloatParameter("dmgAnimationDuration", &dmgAnimationDuration);
		RegisterFloatParameter("attackDistance", &attackDistance);
		RegisterFloatParameter("triggerDistance", &triggerDistance);
		RegisterFloatParameter("attackDamage", &attackDamage);
	}

	~EnemyController()
	{
		GUIManager::RemoveWidget(healthBarPanel);
	}

	void Awake()
	{
		HFEngine::ECS.SetNameGameObject(GetGameObject(), "enemy");
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

		auto& scriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(playerObject);
		playerController = scriptContainer.GetScript<PlayerController>();
		playerCellOptimizer = scriptContainer.GetScript<MapCellOptimizer>();

		auto& mesh = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(visualObject);
		defaultColor = mesh.material->emissiveColor;

		health = maxHealth;

		healthBarPanel = std::make_shared<Panel>();
		healthBarPanel->associatedGameObject = GetGameObject();
		healthBarPanel->SetCoordinatesType(Widget::CoordinatesType::WORLD);
		healthBarPanel->SetPivot(Anchor::CENTER);
		healthBarPanel->SetSize(healthBarSize);
		healthBarPanel->textureColor.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		GUIManager::AddWidget(healthBarPanel);

		healthRedPanel = std::make_shared<Panel>();
		healthRedPanel->SetSize(healthBarSize - (healthBorderSize * 2.0f));
		healthRedPanel->SetPosition({ healthBorderSize, healthBorderSize, 0.0f });
		healthRedPanel->textureColor.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		GUIManager::AddWidget(healthRedPanel, healthBarPanel);

		healthValuePanel = std::make_shared<Panel>();
		healthValuePanel->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
		healthValuePanel->SetSize({ 1.0f, 1.0f });
		healthValuePanel->textureColor.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		GUIManager::AddWidget(healthValuePanel, healthRedPanel);

		raycaster.SetIgnoredGameObject(GetGameObject());
	}

	void Attack()
	{
		isAttacking = true;
		midAttack = false;
		auto& animator = GetAnimator();
		animator.TransitToAnimation("attack", 0.0f, AnimationClip::PlaybackMode::SINGLE);
		animator.SetAnimatorSpeed(animator.GetCurrentClipDuration() / 1000.0f / dmgAnimationDuration);
	}

	void MidAttack()
	{
		midAttack = true;
		glm::vec3 playerPos = HFEngine::ECS.GetComponent<Transform>(playerObject).GetPosition();
		glm::vec3 pos = GetTransform().GetPosition();
		glm::vec3 playerDir = glm::normalize(playerPos - pos);
		raycaster.Raycast(pos, playerDir);

		if (raycaster.GetOut().hittedObject == playerObject)
		{
			if (raycaster.GetOut().distance <= attackDistance)
			{
				AudioManager::CreateDefaultSourceAndPlay(sourceEnemyController, "hit3", false, 0.2f);
				playerController->TakeDamage(attackDamage);
			}
		}
	}

	void EndAttack(bool random = false)
	{
		isAttacking = false;
		auto& animator = GetAnimator();
		if (random)
		{
			auto pos = GetTransform().GetWorldPosition();
			float delay = pos.x * (GetGameObject() / 100.0f);
			delay = (delay - floorf(delay));
			timerAnimator.DelayAction(delay, [&]() {
				animator.TransitToAnimation("move", 0.0f);
				animator.SetAnimatorSpeed(1.0f);
			});
		}
		else
		{
			animator.TransitToAnimation("move", 0.0f);
			animator.SetAnimatorSpeed(1.0f);
		}
	}

	bool CanQueuePathThisFrame()
	{
		unsigned long long num = GetGameObject() + HFEngine::CURRENT_FRAME_NUMBER;
		return num % PathUpdateFrameMod == 0;
	}

	//float pathdt = 0.0f;
	void Update(float dt)
	{
		timerAnimator.Process(dt);

		auto& transform = GetTransform();
		auto& rigidBody = GetRigidBody();
		auto& pathfinder = GetPathfinder();

		if (rigidBody.isFalling)
		{
			if (transform.GetWorldPosition().y < -15.0f)
			{
				DestroyGameObjectSafely();
			}
			return;
		}

		if (playerController->IsDead())
		{
			if (isAttacking) EndAttack(true);

			return;
		}

		glm::vec3 playerPos = HFEngine::ECS.GetComponent<Transform>(playerObject).GetPosition();

		if (!triggered)
		{
			if (playerCellOptimizer->GetCurrentCell() == cellObject)
			{
				if (glm::distance2(playerPos, transform.GetPosition()) <= triggerDistance)
				{
					triggered = true;
				}
			}

			return;
		}

		if (isAttacking)
		{
			auto& animator = GetAnimator();
			if (animator.GetCurrentClipLevel() >= 0.5f && !midAttack)
			{
				MidAttack();
			}
			if (animator.GetCurrentClipLevel() >= 1.0f)
			{
				EndAttack();
			}

			float diff = GetRotationdifferenceToPoint(playerPos);
			float change = dt * rotateSpeedSmoothing;
			if (glm::abs(change) > glm::abs(diff))
				change = diff;
			else
				change *= glm::sign(diff);

			if (glm::abs(change) > 0.01f)
				transform.RotateSelf(glm::degrees(change), transform.GetUp());
		}
		else
		{
			std::optional<glm::vec3> targetPoint;
			glm::vec3 pos = transform.GetPosition();
			glm::vec3 playerDir = glm::normalize(playerPos - pos);
			raycaster.Raycast(pos, playerDir);

			if (raycaster.GetOut().hittedObject == playerObject)
			{
				if (raycaster.GetOut().distance >= attackDistance)
				{
					targetPoint = playerPos - (playerDir * attackDistance);
				}
				else
				{
					Attack();
				}
			}
			else
			{
				// update test path
				if (CanQueuePathThisFrame())
				{
					if (glm::distance2(playerPos, transform.GetPosition()) > attackDistance)
						pathfinder.QueuePath(playerPos);
				}

				targetPoint = GetTargetPoint();
			}

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

			if (currentMoveSpeed > 0.01f)
				Move(transform.GetFront(), dt);
		}
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

	void RestoreDefaultEmissive()
	{
		auto& mesh = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(visualObject);
		timerAnimator.AnimateVariable(&mesh.material->emissiveColor, mesh.material->emissiveColor, defaultColor, dmgAnimationDuration / 2.0f);
	}

	void TakeDamage(float value)
	{
		health -= value;
		auto& mesh = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(visualObject);
		timerAnimator.AnimateVariable(&mesh.material->emissiveColor, mesh.material->emissiveColor, damagedColor, dmgAnimationDuration / 2.0f);
		timerAnimator.DelayAction(dmgAnimationDuration / 2.0f, std::bind(&EnemyController::RestoreDefaultEmissive, this));
		AudioManager::CreateDefaultSourceAndPlay(sourceEnemyController, "hit4", false, 1.0f);

		if (health <= 0)
		{
			AudioManager::CreateDefaultSourceAndPlay(sourceEnemyController, "monsterdeath", false, 0.5f);
			DestroyGameObjectSafely();
		}
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
		auto& transform = GetTransform();
		healthBarPanel->SetPosition(transform.GetWorldPosition() + glm::vec3(0.0f, 3.0f, 0.0f));
		healthValuePanel->SetSize({ health / maxHealth, 1.0f });
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