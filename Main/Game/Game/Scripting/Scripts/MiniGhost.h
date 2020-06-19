#pragma once

#include <glm/gtx/vector_angle.hpp>
#include "../Script.h"
#include "HFEngine.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/SkinAnimator.h"
#include "ECS/Components/SkinnedMeshRenderer.h"
#include "ECS/Components/PointLightRenderer.h"
#include "Utility/TimerAnimator.h"
#include "Event/Events.h"
#include "Event/EventManager.h"
#include "EnemyController.h"
#include "BossController.h"

#define GetTransform() HFEngine::ECS.GetComponent<Transform>(GetGameObject())
#define GetAnimator() HFEngine::ECS.GetComponent<SkinAnimator>(visualObject)

class MiniGhost : public Script
{
private: // parameters
	float moveSpeed = 7.0f;
	float attackPreparationTime = 0.4f;
	float attackTime = 1.25f;
	float damageToEnemies = 20.0f;
	float damageDealtMultiplier = 0.7f;

	glm::vec3 attackAlbedoColor = { 2.5f, 0.4f, 1.5f };
	glm::vec3 attackEmissiveColor = { 0.5f, 0.1f, 0.3f };
	glm::vec3 attackLightColor = { 1.0f, 0.0f, 1.0f };

private: // variables
	GameObject visualObject;
	GameObject ghostLightObject;

	float currentMoveSpeed = 0.0f;
	float moveSpeedSmoothing = 50.0f; // set in Start()
	float rotateSpeedSmoothing = 4.0f * M_PI;

	TimerAnimator timerAnimator;
	float ghostLightDefaultIntensity;
	float ghostMaterialDefaultOpacity;

	bool fadingOut = false;

	bool rotatingToAttack = false;
	glm::vec3 attackDirection;
	bool attacking = false;

public:

	bool IsAttacking() const
	{
		return attacking;
	}

	MiniGhost()
	{
		RegisterFloatParameter("moveSpeed", &moveSpeed);
		RegisterFloatParameter("attackPreparationTime", &attackPreparationTime);
		RegisterFloatParameter("attackTime", &attackTime);
		RegisterFloatParameter("damageToEnemies", &damageToEnemies);
		RegisterFloatParameter("damageDealtMultiplier", &damageDealtMultiplier);

		RegisterVec3Parameter("attackAlbedoColor", &attackAlbedoColor);
		RegisterVec3Parameter("attackEmissiveColor", &attackEmissiveColor);
		RegisterVec3Parameter("attackLightColor", &attackLightColor);
	}

	void Awake()
	{
		visualObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "Visual")[0];
		ghostLightObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "MiniGhostLight")[0];

		ghostLightDefaultIntensity = HFEngine::ECS.GetComponent<PointLightRenderer>(ghostLightObject).light.intensity;
		ghostMaterialDefaultOpacity = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(visualObject).material->opacityValue;

		auto& ghostLight = HFEngine::ECS.GetComponent<PointLightRenderer>(ghostLightObject);
		auto& ghostMesh = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(visualObject);
		timerAnimator.AnimateVariable(&ghostLight.light.intensity, 0.0f, ghostLightDefaultIntensity, 0.25f);
		timerAnimator.AnimateVariable(&ghostMesh.material->opacityValue, 0.0f, ghostMaterialDefaultOpacity, 0.25f);
	
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::MiniGhost::FADE_ME_OUT, MiniGhost::OnFadeMeOut));
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::MiniGhost::ATTACK, MiniGhost::OnAttack));

	}

	void Start()
	{
		moveSpeedSmoothing = moveSpeed * 4.0f;
		auto& collider = HFEngine::ECS.GetComponent<Collider>(GetGameObject());
		collider.OnTriggerEnter.push_back(TriggerMethodPointer(MiniGhost::OnTriggerEnter));
	}

	void Update(float dt)
	{
		timerAnimator.Process(dt);
	}

	// in late update to allow other thread being executed
	void LateUpdate(float dt)
	{
		auto& transform = GetTransform();

		glm::vec3 translateVec = { 0.0f, 0.0f, 0.0f };

		if (fadingOut)
		{
			translateVec += transform.GetUp() * 2.0f;
		}

		if (rotatingToAttack)
		{
			glm::vec3 front3 = transform.GetFront();
			float diff = glm::orientedAngle(
				glm::normalize(glm::vec2(attackDirection.x, attackDirection.z)),
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

		float targetMoveSpeed = attacking ? moveSpeed : 0.0f;
		{
			float diff = targetMoveSpeed - currentMoveSpeed;
			float change = dt * moveSpeedSmoothing;
			if (glm::abs(change) > glm::abs(diff))
				currentMoveSpeed = targetMoveSpeed;
			else
				currentMoveSpeed += change * glm::sign(diff);
		}
		if (currentMoveSpeed > 0.01f)
			translateVec += transform.GetFront() * currentMoveSpeed;

		if (glm::length2(translateVec) > 0.0001f)
			transform.TranslateSelf(translateVec * dt);
	}

	void OnFadeMeOut(Event& ev)
	{
		FadeMeOut(0.5f);
	}
	void FadeMeOut(float time)
	{
		auto& ghostLight = HFEngine::ECS.GetComponent<PointLightRenderer>(ghostLightObject);
		auto& ghostMesh = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(visualObject);
		timerAnimator.AnimateVariable(&ghostLight.light.intensity, ghostLight.light.intensity, 0.0f, time);
		timerAnimator.AnimateVariable(&ghostMesh.material->opacityValue, ghostMesh.material->opacityValue, 0.0f, time);
		timerAnimator.DelayAction(time + 0.1f, std::bind(&MiniGhost::DestroyGameObjectSafely, this));
		fadingOut = true;
		attacking = false;
	}

	void OnTriggerEnter(GameObject that, GameObject other)
	{
		if (!attacking) return;
		auto otherName = HFEngine::ECS.GetNameGameObject(other);
		if (!strcmp(otherName, "enemy"))
		{
			auto& scriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(other);
			auto enemyController = scriptContainer.GetScript<EnemyController>();
			AudioManager::PlayFromDefaultSource("ghostattack", false, 0.1f);
			enemyController->TakeDamage(damageToEnemies);
			damageToEnemies *= damageDealtMultiplier;
		}
		else if (!strcmp(otherName, "boss"))
		{
			auto& scriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(other);
			auto bossController = scriptContainer.GetScript<BossController>();
			AudioManager::PlayFromDefaultSource("ghostattack", false, 0.1f);
			bossController->RequestToTakeDamage(damageToEnemies);
			damageToEnemies *= damageDealtMultiplier;
		}
	}


	void OnAttack(Event& ev)
	{
		auto& animator = GetAnimator();

		rotatingToAttack = true;
		attackDirection = ev.GetParam<glm::vec3>(Events::Gameplay::MiniGhost::Direction);
		
		auto& ghostLight = HFEngine::ECS.GetComponent<PointLightRenderer>(ghostLightObject);
		auto& ghostMesh = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(visualObject);
		timerAnimator.AnimateVariable(&ghostLight.light.color, ghostLight.light.color, attackLightColor, attackPreparationTime);
		timerAnimator.AnimateVariable(&ghostMesh.material->albedoColor, ghostMesh.material->albedoColor, attackAlbedoColor, attackPreparationTime);
		timerAnimator.AnimateVariable(&ghostMesh.material->emissiveColor, ghostMesh.material->emissiveColor, attackEmissiveColor, attackPreparationTime);

		timerAnimator.DelayAction(attackPreparationTime, [&]() {
			animator.TransitToAnimation("ghostrunning");
			animator.SetAnimatorSpeed(1.0f);
			rotatingToAttack = false;
			attacking = true;
			Physics::ReLaunchTriggers(GetGameObject());

			timerAnimator.DelayAction(attackTime, std::bind(&MiniGhost::FadeMeOut, this, 0.3f));
		});
	}
};


#undef GetTransform()
#undef GetAnimator()