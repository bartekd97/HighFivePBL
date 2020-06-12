#pragma once

#include <glm/gtx/vector_angle.hpp>
#include "../Script.h"
#include "HFEngine.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/SkinAnimator.h"
#include "ECS/Components/PointLightRenderer.h"
#include "InputManager.h"
#include "Event/Events.h"
#include "../../Physics/Physics.h"
#include "../../Rendering/PrimitiveRenderer.h"
#include "Physics/Raycaster.h"
#include "GhostController.h"
#include "../../GUI/GUIManager.h"
#include "../../GUI/Panel.h"
#include "Utility/TimerAnimator.h"
#include "Resourcing/Prefab.h"

#define GetTransform() HFEngine::ECS.GetComponent<Transform>(GetGameObject())
#define GetAnimator() HFEngine::ECS.GetComponent<SkinAnimator>(visualObject)
#define GetRigidBody() HFEngine::ECS.GetComponent<RigidBody>(GetGameObject())

class PlayerController : public Script
{
private: // parameters
	float moveSpeed = 10.0f;

	float pushbackCooldown = 2.0f;

	float torchCooldownEmitRate = 32.0f;
	glm::vec3 torchLightCooldownLightColor = { 0.7f, 0.25f, 0.1f };

private: // variables
	glm::vec3 startPosition;
	GameObject visualObject;
	GameObject ghostObject;
	GameObject attackSmokeObject;
	GameObject torchFlameLightObject;
	GameObject torchFlameParticleObject;

	float currentMoveSpeed = 0.0f;
	float moveSpeedSmoothing; // set in Start()
	float rotateSpeedSmoothing = 4.0f * M_PI;
	float pushBackDistance = 5.0f;
	float pushBackForce = 15.0f;

	float attackAnimationLevel = 0.5f;

	bool hasGhostMovement = false;
	bool isPushingBack = false;
	bool onPushBackCooldown = false;
	Raycaster raycaster;

	std::shared_ptr<GhostController> ghostController;
	std::shared_ptr<Panel> ghostBarPanel;
	std::shared_ptr<Panel> ghostValueBarPanel;
	float ghostBarWidth = 0.6;
	float ghostValueBarOffset = 3.0f;

	TimerAnimator timerAnimator;
	float torchLightDefaultIntensity;
	float torchDefaultEmitRate;
	glm::vec3 torchLightDefaultLightColor;

public:
	PlayerController()
	{
		RegisterFloatParameter("moveSpeed", &moveSpeed);
		RegisterFloatParameter("pushbackCooldown", &pushbackCooldown);

		RegisterFloatParameter("torchCooldownEmitRate", &torchCooldownEmitRate);
		RegisterVec3Parameter("torchLightCooldownLightColor", &torchLightCooldownLightColor);
	}

	~PlayerController()
	{
		GUIManager::RemoveWidget(ghostBarPanel);
	}

	void Awake()
	{
		ghostObject = PrefabManager::GetPrefab("PlayerGhost")->Instantiate();
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Ghost::MOVEMENT_START, PlayerController::GhostMovementStart));
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Ghost::MOVEMENT_STOP, PlayerController::GhostMovementStop));
		raycaster.SetIgnoredGameObject(GetGameObject());
	}

	void Start()
	{
		visualObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "Visual")[0];
		attackSmokeObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "AttackSmoke")[0];
		torchFlameLightObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "FlameLight")[0];
		torchFlameParticleObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "TorchFlame")[0];

		torchLightDefaultIntensity = HFEngine::ECS.GetComponent<PointLightRenderer>(torchFlameLightObject).light.intensity;
		torchDefaultEmitRate = HFEngine::ECS.GetComponent<ParticleEmitter>(torchFlameParticleObject).rate;
		torchLightDefaultLightColor = HFEngine::ECS.GetComponent<PointLightRenderer>(torchFlameLightObject).light.color;


		startPosition = GetTransform().GetWorldPosition();
		moveSpeedSmoothing = moveSpeed * 4.0f;
		GetAnimator().SetAnimation("idle");


		auto& ghostScriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(ghostObject);
		ghostController = ghostScriptContainer.GetScript<GhostController>();

		ghostBarPanel = std::make_shared<Panel>();
		ghostBarPanel->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
		//ghostBarPanel->SetPositionAnchor(glm::vec3(((1.0f - ghostBarWidth) / 2.0f) * WindowManager::SCREEN_WIDTH, -100.0f, 0.0f), Anchor::BOTTOMLEFT);
		//ghostBarPanel->SetSize(glm::vec2(ghostBarWidth * WindowManager::SCREEN_WIDTH, 50.0f));
		ghostBarPanel->SetPositionAnchor({ (1.0f - ghostBarWidth) / 2.0f, -0.1388f, 0.0f }, Anchor::BOTTOMLEFT);
		ghostBarPanel->SetSize({ ghostBarWidth, 0.07f });
		ghostBarPanel->SetClipping(true);
		ghostBarPanel->textureColor.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		GUIManager::AddWidget(ghostBarPanel);

		ghostValueBarPanel = std::make_shared<Panel>();
		ghostValueBarPanel->SetPositionAnchor(glm::vec3(ghostValueBarOffset, ghostValueBarOffset, 0.0f), Anchor::TOPLEFT);
		ghostValueBarPanel->SetSize(glm::vec2(0.0f, 50.0f - 2 * ghostValueBarOffset));
		ghostValueBarPanel->SetClipping(true);
		ghostValueBarPanel->textureColor.color = glm::vec4(0.0f, 0.78f, 0.76f, 0.75f);

		GUIManager::AddWidget(ghostValueBarPanel, ghostBarPanel);
	}

	void GhostMovementStart(Event& event) {
		hasGhostMovement = true;
		auto& torch = HFEngine::ECS.GetComponent<PointLightRenderer>(torchFlameLightObject);
		timerAnimator.AnimateVariable(&torch.light.intensity, torch.light.intensity, 0.0f, 0.3f);
		HFEngine::ECS.GetComponent<ParticleEmitter>(torchFlameParticleObject).emitting = false;
	}
	void GhostMovementStop(Event& event) {
		hasGhostMovement = false;
		auto& torch = HFEngine::ECS.GetComponent<PointLightRenderer>(torchFlameLightObject);
		timerAnimator.AnimateVariable(&torch.light.intensity, torch.light.intensity, torchLightDefaultIntensity, 0.3f);
		HFEngine::ECS.GetComponent<ParticleEmitter>(torchFlameParticleObject).emitting = true;
	}

	void Update(float dt)
	{
		timerAnimator.Process(dt);

		auto& transform = GetTransform();
		auto& animator = GetAnimator();
		auto& rigidBody = GetRigidBody();

		bool isMoving = UpdateMovement(dt);

		if (isMoving)
			animator.TransitToAnimation("running", 0.2f);
		else if (isPushingBack)
			animator.TransitToAnimation("attack", 0.1f, AnimationClip::PlaybackMode::SINGLE);
		else if (hasGhostMovement)
			animator.TransitToAnimation("standToCrouch", 0.15f, AnimationClip::PlaybackMode::SINGLE);
		else
			animator.TransitToAnimation("idle", 0.2f);

		if (!isPushingBack)
		{
			if (!hasGhostMovement && InputManager::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
				EventManager::FireEvent(Events::Gameplay::Ghost::MOVEMENT_START);
			else if (hasGhostMovement && InputManager::GetMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT))
				EventManager::FireEvent(Events::Gameplay::Ghost::MOVEMENT_STOP);

			else if (!onPushBackCooldown && InputManager::GetKeyDown(GLFW_KEY_SPACE))
			{
				StartPushBack();
				isPushingBack = true;
				onPushBackCooldown = true;
			}
		}
		else
		{
			if (animator.GetCurrentClipLevel() >= attackAnimationLevel)
			{
				isPushingBack = false;
			}
		}

		if (!hasGhostMovement)
		{
			ghostController->leftGhostDistance = std::min(
				ghostController->maxGhostDistance,
				ghostController->leftGhostDistance + dt * ghostController->ghostDistanceRecoverySpeed
			);
		}

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

		ghostValueBarPanel->SetSize(glm::vec2(ghostController->GetLeftGhostLevel() * ghostBarPanel->GetLocalSize().x - 2 * ghostValueBarOffset, ghostValueBarPanel->GetLocalSize().y));
	}

	bool UpdateMovement(float dt)
	{
		auto& transform = GetTransform();
		auto& rigidBody = GetRigidBody();

		glm::vec3 direction(0.0f);
		if (!hasGhostMovement && !isPushingBack)
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


	void StartPushBack()
	{
		auto& emitterSmoke = HFEngine::ECS.GetComponent<ParticleEmitter>(attackSmokeObject);
		auto& emitterTorch = HFEngine::ECS.GetComponent<ParticleEmitter>(torchFlameParticleObject);
		auto& lightTorch = HFEngine::ECS.GetComponent<PointLightRenderer>(torchFlameLightObject);

		// anim & pushback stuff
		timerAnimator.DelayAction(0.2f, [&]() {
			emitterSmoke.emitting = true;
			});
		timerAnimator.DelayAction(0.35f, [&]() {
			PushbackTest();
			});
		timerAnimator.DelayAction(0.5f, [&]() {
			emitterSmoke.emitting = false;
			});

		// cooldown stuff
		timerAnimator.AnimateVariable(&emitterTorch.rate, torchDefaultEmitRate, torchCooldownEmitRate, 0.7f);
		timerAnimator.AnimateVariable(&lightTorch.light.color, torchLightDefaultLightColor, torchLightCooldownLightColor, 0.7f);
		timerAnimator.DelayAction(0.7f + pushbackCooldown, [&]() {
			timerAnimator.AnimateVariable(&emitterTorch.rate, torchCooldownEmitRate, torchDefaultEmitRate, 0.3f);
			timerAnimator.AnimateVariable(&lightTorch.light.color, torchLightCooldownLightColor, torchLightDefaultLightColor, 0.3f);
			});
		timerAnimator.DelayAction(1.0f + pushbackCooldown, [&]() {
			onPushBackCooldown = false;
			});
	}

	void PushbackTest()
	{
		auto objects = HFEngine::ECS.GetGameObjectsByName("enemy");
		glm::vec3 dir;
		auto pos = GetTransform().GetWorldPosition();
		for (auto& object : objects)
		{
			auto objPos = HFEngine::ECS.GetComponent<Transform>(object).GetWorldPosition();
			dir = objPos - pos;
			if (VECLEN(dir) < pushBackDistance)
			{
				auto& objectRb = HFEngine::ECS.GetComponent<RigidBody>(object);
				objectRb.AddForce(glm::normalize(glm::normalize(dir / 2.0f) + glm::vec3(0.0f, 0.75f, 0.0f)) * pushBackForce);
				objectRb.isFalling = true;
			}
		}
	}
};


#undef GetTransform()
#undef GetAnimator()
#undef GetRigidBody()