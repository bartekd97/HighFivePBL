#pragma once

#include <glm/gtx/vector_angle.hpp>
#include "../Script.h"
#include "HFEngine.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/SkinAnimator.h"
#include "Event/Events.h"
#include "Event/EventManager.h"
#include "Rendering/PrimitiveRenderer.h"
#include "GUI/GUIManager.h"
#include "GUI/Panel.h"
#include "Utility/TimerAnimator.h"

#define GetTransform() HFEngine::ECS.GetComponent<Transform>(GetGameObject())
#define GetAnimator() HFEngine::ECS.GetComponent<SkinAnimator>(visualObject)
#define GetRigidBody() HFEngine::ECS.GetComponent<RigidBody>(GetGameObject())

class TutorialEnemy : public Script
{
private: // parameters
	float moveSpeed = 5.0f;
	float maxHealth = 10.0f;
	float dmgAnimationDuration = 0.5f;
	float mainGhostDamage = 1.0f;
	float miniGhostDamage = 2.75f;

private: // variables
	GameObject visualObject;
	GameObject cellObject;

	float currentMoveSpeed = 0.0f;
	float moveSpeedSmoothing = 50.0f; // set in Start()
	float rotateSpeedSmoothing = 4.0f * M_PI;

	float health;

	glm::vec3 defaultColor;
	glm::vec3 damagedColor = { 1.0f, 0.0f, 0.0f };
	TimerAnimator timerAnimator;

	glm::vec3 originalPos;
	glm::vec3 originalLookPos;

	std::shared_ptr<Panel> healthBarPanel;
	std::shared_ptr<Panel> healthRedPanel;
	std::shared_ptr<Panel> healthValuePanel;
	glm::vec2 healthBarSize = { 60.0f, 10.0f };
	float healthBorderSize = 2.0f;

public:
	TutorialEnemy()
	{
		RegisterFloatParameter("moveSpeed", &moveSpeed);
		RegisterFloatParameter("maxHealth", &maxHealth);
		RegisterFloatParameter("dmgAnimationDuration", &dmgAnimationDuration);
		RegisterFloatParameter("mainGhostDamage", &mainGhostDamage);
		RegisterFloatParameter("miniGhostDamage", &miniGhostDamage);
	}

	~TutorialEnemy()
	{
		GUIManager::RemoveWidget(healthBarPanel);
	}

	void Awake()
	{
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Player::PUSHBACK_ENEMIES, TutorialEnemy::OnPushBackEnemies));
	}

	void Start()
	{
		visualObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "Visual")[0];
		moveSpeedSmoothing = moveSpeed * 4.0f;
		GetAnimator().SetAnimation("move"); // TODO: idle

		cellObject = HFEngine::ECS.GetComponent<CellChild>(GetGameObject()).cell;

		auto& mesh = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(visualObject);
		defaultColor = mesh.material->emissiveColor;

		health = maxHealth;

		originalPos = GetTransform().GetPosition();
		originalLookPos = GetTransform().GetPosition() + GetTransform().GetFront();

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

		auto& collider = HFEngine::ECS.GetComponent<Collider>(GetGameObject());
		collider.OnTriggerEnter.push_back(TriggerMethodPointer(TutorialEnemy::OnTriggerEnter));
	}

	void Update(float dt)
	{
		auto& transform = HFEngine::ECS.GetComponent<Transform>(GetGameObject());

		glm::vec3 currentPos = transform.GetPosition();
		float dist = glm::distance(currentPos, originalPos);
		if (dist > 0.25f)
		{
			{
				// smooth rotate
				float diff = GetRotationdifferenceToPoint(originalPos);
				float change = dt * rotateSpeedSmoothing;
				if (glm::abs(change) > glm::abs(diff))
					change = diff;
				else
					change *= glm::sign(diff);

				if (glm::abs(change) > 0.01f)
					transform.RotateSelf(glm::degrees(change), transform.GetUp());
			}

			// smoth move speed
			float speedFactor = dist;
			if (speedFactor > 1.0f) speedFactor = glm::pow(speedFactor, 0.25f);
			float targetMoveSpeed = speedFactor * moveSpeed;
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
				GetRigidBody().Move(transform.GetPosition() + moveBy);
		}
		else
		{
			{
				// smooth rotate
				float diff = GetRotationdifferenceToPoint(originalLookPos);
				float change = dt * rotateSpeedSmoothing;
				if (glm::abs(change) > glm::abs(diff))
					change = diff;
				else
					change *= glm::sign(diff);

				if (glm::abs(change) > 0.01f)
					transform.RotateSelf(glm::degrees(change), transform.GetUp());
			}
		}

		timerAnimator.Process(dt);
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

	void LateUpdate(float dt)
	{
		auto& transform = GetTransform();
		healthBarPanel->SetPosition(transform.GetWorldPosition() + glm::vec3(0.0f, 3.0f, 0.0f));
		healthValuePanel->SetSize({ health / maxHealth, 1.0f });
	}

	void OnPushBackEnemies(Event& ev)
	{
		glm::vec3 pos = ev.GetParam<glm::vec3>(Events::Gameplay::Player::Position);
		float pushBackDistance = ev.GetParam<float>(Events::Gameplay::Player::PushBackDistance);
		float pushBackForce = ev.GetParam<float>(Events::Gameplay::Player::PushBackForce);

		auto objPos = GetTransform().GetWorldPosition();
		glm::vec3 dir = objPos - pos;
		if (VECLEN(dir) < pushBackDistance)
		{
			auto& objectRb = GetRigidBody();
			objectRb.AddForce(glm::normalize(glm::normalize(dir / 2.0f) + glm::vec3(0.0f, 0.75f, 0.0f)) * pushBackForce);
			objectRb.isFalling = true;
		}
	}

	void RestoreDefaultEmissive()
	{
		auto& mesh = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(visualObject);
		timerAnimator.AnimateVariable(&mesh.material->emissiveColor, mesh.material->emissiveColor, defaultColor, dmgAnimationDuration / 2.0f);
	}

	void OnTriggerEnter(GameObject that, GameObject other)
	{
		std::string otherName = HFEngine::ECS.GetNameGameObject(other);

		float damageValue = 0.0f;

		if (otherName == "PlayerGhost")
			damageValue = mainGhostDamage;
		else if (otherName == "MiniGhost")
			damageValue = miniGhostDamage;

		if (damageValue > 0.0f)
		{
			health -= damageValue;
			auto& mesh = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(visualObject);
			timerAnimator.AnimateVariable(&mesh.material->emissiveColor, mesh.material->emissiveColor, damagedColor, dmgAnimationDuration / 2.0f);
			timerAnimator.DelayAction(dmgAnimationDuration / 2.0f, std::bind(&TutorialEnemy::RestoreDefaultEmissive, this));
		
			if (health <= 0)
			{
				EventManager::FireEvent(Events::Gameplay::Tutorial::ENEMYTOY_KILLED);
				DestroyGameObjectSafely();
			}
		}
	}
};


#undef GetTransform()
#undef GetAnimator()
#undef GetRigidBody()