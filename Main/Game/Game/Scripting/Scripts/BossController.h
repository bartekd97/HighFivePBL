#pragma once

#include <glm/gtx/vector_angle.hpp>
#include "../Script.h"
#include "HFEngine.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/SkinAnimator.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "ECS/Components/CellPathfinder.h"
#include "Event/Events.h"
#include "GUI/GUIManager.h"
#include "GUI/Panel.h"
#include "Physics/Raycaster.h"

#define GetTransform() HFEngine::ECS.GetComponent<Transform>(GetGameObject())
#define GetPathfinder() HFEngine::ECS.GetComponent<CellPathfinder>(GetGameObject())
#define GetAnimator() HFEngine::ECS.GetComponent<SkinAnimator>(visualObject)
#define GetRigidBody() HFEngine::ECS.GetComponent<RigidBody>(GetGameObject())

class BossController : public Script
{
private: // parameters
	float moveSpeed = 10.0f;
	float maxHealth = 100.0f;
	std::string awaitingAnimation = "idle";

private: // variables
	GameObject visualObject;

	float currentMoveSpeed = 0.0f;
	float moveSpeedSmoothing = 50.0f; // set in Start()
	float rotateSpeedSmoothing = 4.0f * M_PI;

	float health;
	bool triggered = false;
	bool dead = false;

	GameObject playerObject;
	GameObject cellObject;

	Raycaster raycaster;

	std::shared_ptr<Panel> healthBarPanel;
	std::shared_ptr<Panel> healthRedPanel;
	std::shared_ptr<Panel> healthValuePanel;
	glm::vec2 healthBarSize = { 90.0f, 20.0f };
	float healthBorderSize = 2.0f;


	struct AnimationAction
	{
		float actionTime;
		bool wasActionCalled = false;
		std::function<void()> onAction;
		float endTime;
	};
	std::optional<AnimationAction> currentAnimAction;

public:
	
	std::function<void(float)> OnRequestToTakeDamage;
	glm::vec3 TargetMovePosition;
	glm::vec3 PositionToLookAt;


	BossController()
	{
		RegisterFloatParameter("moveSpeed", &moveSpeed);
		RegisterFloatParameter("maxHealth", &maxHealth);
		RegisterStringParameter("awaitingAnimation", &awaitingAnimation);
	}

	~BossController()
	{
		GUIManager::RemoveWidget(healthBarPanel);
	}

	void Awake()
	{
		HFEngine::ECS.SetNameGameObject(GetGameObject(), "boss");
	}

	inline float GetHealth()
	{
		return health;
	}

	inline float GetMaxHealth()
	{
		return maxHealth;
	}

	GameObject GetPlayerObject() { return playerObject; }
	Raycaster& GetRaycaster() { return raycaster; }
	bool IsTriggered() { return triggered; }
	bool IsDead() { return dead; }

	void Start()
	{
		if (!HFEngine::ECS.SearchComponent<CellPathfinder>(GetGameObject()))
			HFEngine::ECS.AddComponent<CellPathfinder>(GetGameObject(), {});
		// TODO: pathfinder not used yet for boss
		//GetPathfinder().onPathReady = PathReadyMethodPointer(EnemyController::OnNewPathToPlayer);

		visualObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "Visual")[0];
		moveSpeedSmoothing = moveSpeed * 4.0f;
		GetAnimator().SetAnimation(awaitingAnimation);

		playerObject = HFEngine::ECS.GetGameObjectByName("Player").value();
		cellObject = HFEngine::ECS.GetComponent<CellChild>(GetGameObject()).cell;

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

		EventManager::FireEventTo(GetGameObject(), Events::Gameplay::Boss::INITLIAZE_SCRIPT);
	}

	void Update(float dt)
	{
		if (!triggered) return;
		if (dead) return;

		auto& transform = GetTransform();
		auto& rigidbody = GetRigidBody();
		auto& animator = GetAnimator();

		// process animation action
		if (currentAnimAction.has_value())
		{
			float prog = animator.GetCurrentClipLevel();
			if (prog >= currentAnimAction->actionTime && !currentAnimAction->wasActionCalled)
			{
				currentAnimAction->onAction();
				currentAnimAction->wasActionCalled = true;
			}
			else if (prog >= currentAnimAction->endTime)
			{
				currentAnimAction = std::nullopt;
			}
		}

		glm::vec3 currentPos = transform.GetPosition();
		glm::vec3 moveDirection = glm::normalize(TargetMovePosition - currentPos);
		float targetMoveSpeed = GetTargetMoveSpeed();

		{
			// smoth move speed
			{
				float diff = targetMoveSpeed - currentMoveSpeed;
				float change = dt * moveSpeedSmoothing;
				if (glm::abs(change) > glm::abs(diff))
					currentMoveSpeed = targetMoveSpeed;
				else
					currentMoveSpeed += change * glm::sign(diff);
			}
			auto moveBy = (currentMoveSpeed * dt) * moveDirection;
			if (currentMoveSpeed > 0.01f)
				GetRigidBody().Move(transform.GetPosition() + moveBy);
		}

		// update animator
		if (targetMoveSpeed > 0.01f)
		{
			glm::vec3 front3 = transform.GetFront();
			float diff = glm::orientedAngle(
				glm::normalize(glm::vec2(moveDirection.x, moveDirection.z)),
				glm::normalize(glm::vec2(front3.x, front3.z))
				);

			// TODO: right and left anim movement
			if (glm::abs(diff) < 90.0f)
				animator.TransitToAnimation("running", 0.15f);
			else
				animator.TransitToAnimation("running_back", 0.15f);
		}
		else if (!currentAnimAction.has_value())
		{
			animator.TransitToAnimation("idle", 0.15f);
		}

		{
			// smooth rotate
			float diff = GetRotationdifferenceToPoint(PositionToLookAt);
			float change = dt * rotateSpeedSmoothing;
			if (glm::abs(change) > glm::abs(diff))
				change = diff;
			else
				change *= glm::sign(diff);

			if (glm::abs(change) > 0.01f)
				transform.RotateSelf(glm::degrees(change), transform.GetUp());
		}
	}

	GameObject GetVisualObject()
	{
		return visualObject;
	}

	float GetTargetMoveSpeed()
	{
		if (currentAnimAction.has_value()) return 0.0f;

		auto& transform = GetTransform();
		glm::vec3 currentPos = transform.GetPosition();
		glm::vec3 moveDirection = glm::normalize(TargetMovePosition - currentPos);

		float dist = glm::distance(currentPos, TargetMovePosition);
		if (dist < 0.25f)
			return 0.0f;
		else
		{
			bool clearWay = true;
			if (raycaster.Raycast(currentPos, moveDirection))
			{
				if (raycaster.GetOut().distance < 1.0f)
					clearWay = false;
			}
			return clearWay ? moveSpeed : 0.0f;
		}
	}

	bool RequestAnimationAction(std::string animName, float transitionTime, float actionTime, float endTime, std::function<void()> onAction)
	{
		if (currentAnimAction.has_value()) return false;
		GetAnimator().TransitToAnimation(animName, transitionTime, AnimationClip::PlaybackMode::SINGLE);
		currentAnimAction = AnimationAction();
		currentAnimAction->actionTime = actionTime;
		currentAnimAction->endTime = endTime;
		currentAnimAction->onAction = onAction;
		return true;
	}

	void LateUpdate(float dt)
	{
		if (dead) return;

		auto& transform = GetTransform();
		healthBarPanel->SetPosition(transform.GetWorldPosition() + glm::vec3(0.0f, 7.0f, 0.0f));
		healthValuePanel->SetSize({ health / maxHealth, 1.0f });
	}

	void RequestToTakeDamage(float value)
	{
		if (dead) return;

		if (!triggered)
		{
			triggered = true;

			TargetMovePosition = GetTransform().GetPosition();
			PositionToLookAt = HFEngine::ECS.GetComponent<Transform>(playerObject).GetPosition();

			GetAnimator().TransitToAnimation("idle");

			Event ev(Events::Gameplay::Boss::TRIGGERED);
			ev.SetParam(Events::GameObject::GameObject, GetGameObject());
			EventManager::FireEvent(ev);

			return;
		}

		if (OnRequestToTakeDamage)
			OnRequestToTakeDamage(value);
	}

	void TakeDamage(float value)
	{
		health -= value;
		//auto& mesh = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(visualObject);
		//timerAnimator.AnimateVariable(&mesh.material->emissiveColor, mesh.material->emissiveColor, damagedColor, dmgAnimationDuration / 2.0f);
		//timerAnimator.DelayAction(dmgAnimationDuration / 2.0f, std::bind(&BossController::RestoreDefaultEmissive, this));

		if (health <= 0)
		{
			healthBarPanel->SetEnabled(false);
			healthBarPanel->associatedGameObject = NULL_GAMEOBJECT;

			GetAnimator().TransitToAnimation("dying", 0.25f, AnimationClip::PlaybackMode::SINGLE);
			dead = true;

			Event ev(Events::Gameplay::Boss::DEAD);
			ev.SetParam(Events::GameObject::GameObject, GetGameObject());
			EventManager::FireEvent(ev);
		}
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