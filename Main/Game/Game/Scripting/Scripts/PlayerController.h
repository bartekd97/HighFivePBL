#pragma once

#include <glm/gtx/vector_angle.hpp>
#include "CreatureController.h"
#include "HFEngine.h"
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
#include "../../GUI/Button.h"
#include "Utility/TimerAnimator.h"
#include "Resourcing/Prefab.h"
#include "Audio/AudioManager.h"
#include "../../Scene/SceneManager.h"

#define GetTransform() HFEngine::ECS.GetComponent<Transform>(GetGameObject())
#define GetAnimator() HFEngine::ECS.GetComponent<SkinAnimator>(visualObject)
#define GetRigidBody() HFEngine::ECS.GetComponent<RigidBody>(GetGameObject())

class PlayerController : public CreatureController
{
private: // parameters
	float healthRecoverySpeed = 5.0f;
	float idleToStartRecoveryTime = 3.0f;
	float healthPercentagePulsing = 0.35f;

	float ghostCooldown = 0.35f;
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

	float moveSpeedSmoothing; // set in Start()
	float rotateSpeedSmoothing = 4.0f * M_PI;
	float pushBackDistance = 5.0f;
	float pushBackForce = 15.0f;
	float healthMaxOpacity = 0.7f;
	float healthPulsingMaxOpacity = 0.85f;
	float healthPulseTime = 0.8f;
	float healthPulsePauseTime = 0.0f;
	bool isHealthPulsing = false;
	bool isGhostCircleFaded = false;
	glm::vec2 healthPanelMinSize = { 1.0f, 1.0f };
	glm::vec2 healthPanelMaxSize = { 1.3f, 1.3f };
	glm::vec2 healthPanelSize = healthPanelMaxSize;
	std::chrono::steady_clock::time_point lastDmgTime;

	float attackAnimationLevel = 0.5f;

	bool hasGhostMovement = false;
	bool ghostOnCooldown = false;
	bool isPushingBack = false;
	bool onPushBackCooldown = false;
	Raycaster raycaster;
	bool isReadyToStartMovement = true;


	std::shared_ptr<GhostController> ghostController;
	std::shared_ptr<Panel> ghostCircleBarPanel;
	std::shared_ptr<Panel> healthPanel;
	std::shared_ptr<Panel> lostGamePanel;
	std::shared_ptr<Button> lostGameButton;
	float ghostBarWidth = 0.6;
	float ghostValueBarOffset = 3.0f;

	TimerAnimator timerAnimator;
	float torchLightDefaultIntensity;
	float torchDefaultEmitRate;
	glm::vec3 torchLightDefaultLightColor;

public:
	PlayerController()
	{
		RegisterFloatParameter("healthRecoverySpeed", &healthRecoverySpeed);
		RegisterFloatParameter("idleToStartRecoveryTime", &idleToStartRecoveryTime);
		RegisterFloatParameter("healthPercentagePulsing", &healthPercentagePulsing);

		RegisterFloatParameter("ghostCooldown", &ghostCooldown);
		RegisterFloatParameter("pushbackCooldown", &pushbackCooldown);

		RegisterFloatParameter("torchCooldownEmitRate", &torchCooldownEmitRate);
		RegisterVec3Parameter("torchLightCooldownLightColor", &torchLightCooldownLightColor);
	}

	~PlayerController()
	{
		GUIManager::RemoveWidget(lostGamePanel);
		GUIManager::RemoveWidget(lostGameButton);
		GUIManager::RemoveWidget(ghostCircleBarPanel);
		GUIManager::RemoveWidget(healthPanel);
	}

	void Awake()
	{
		ghostObject = PrefabManager::GetPrefab("PlayerGhost")->Instantiate();
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Ghost::MOVEMENT_START, PlayerController::GhostMovementStart));
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Ghost::MOVEMENT_STOP, PlayerController::GhostMovementStop));
		raycaster.SetIgnoredGameObject(GetGameObject());
		AudioManager::SetMovementSound("footsteps_in_grass", 0.1f);

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

		health = maxHealth;

		auto& ghostScriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(ghostObject);
		ghostController = ghostScriptContainer.GetScript<GhostController>();

		ghostCircleBarPanel = std::make_shared<Panel>();
		ghostCircleBarPanel->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
		ghostCircleBarPanel->SetPivot(Anchor::CENTER);
		ghostCircleBarPanel->SetPositionAnchor({ 0.0f, 0.0f, 0.0f }, Anchor::CENTER);
		ghostCircleBarPanel->SetSize(glm::vec2{ 0.5625f, 1.0f } * 0.3f); // 16:9
		ghostCircleBarPanel->textureColor.texture = TextureManager::GetTexture("GUI/Player", "playerGhostBar");
		ghostCircleBarPanel->textureColor.color = glm::vec4(0.0f, 0.7f, 0.7f, 0.0f);
		GUIManager::AddWidget(ghostCircleBarPanel);

		healthPanel = std::make_shared<Panel>();
		healthPanel->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
		healthPanel->SetPivot(Anchor::CENTER);
		healthPanel->SetPositionAnchor({ 0.0f, 0.0f, 0.0f }, Anchor::CENTER);
		healthPanel->SetSize(healthPanelSize);
		healthPanel->textureColor.texture = TextureManager::GetTexture("GUI/Player", "playerHealth");
		healthPanel->textureColor.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
		GUIManager::AddWidget(healthPanel);

		lostGamePanel = std::make_shared<Panel>();
		lostGamePanel->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
		lostGamePanel->SetSize({ 0.333f, 0.444f });
		lostGamePanel->SetPivot(Anchor::CENTER);
		lostGamePanel->SetPositionAnchor(glm::vec3(0.0f, -0.25f, 0.0f), Anchor::CENTER);
		lostGamePanel->textureColor.texture = TextureManager::GetTexture("GUI", "lostGame");
		lostGamePanel->textureColor.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
		GUIManager::AddWidget(lostGamePanel, nullptr, 3);
		lostGamePanel->SetEnabled(false);

		lostGameButton = std::make_shared<Button>();
		lostGameButton->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
		lostGameButton->SetPositionAnchor(glm::vec3(0.0f, 0.25f, 0.0f), Anchor::CENTER);
		lostGameButton->SetSize({ 0.1953f, 0.125f });//250 120
		lostGameButton->SetPivot(Anchor::CENTER);
		lostGameButton->OnClickListener = GUI_METHOD_POINTER(PlayerController::BackToMainMenu);

		for (int i = (int)Button::STATE::NORMAL; i <= (int)Button::STATE::PRESSED; i++)
		{
			lostGameButton->textureColors[(Button::STATE)i].texture = TextureManager::GetTexture("GUI", "lostGameButton");
			lostGameButton->textureColors[(Button::STATE)i].color = glm::vec4(glm::vec3(1.0f), 0.6f + (i * 0.2f));
		}

		GUIManager::AddWidget(lostGameButton, nullptr, 3);
		lostGameButton->SetEnabled(false);

		// TODO: tmp
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Boss::DEAD, PlayerController::OnBossDead));
	}

	void OnBossDead(Event& ev)
	{
		health = 0;
		GetAnimator().TransitToAnimation("idle", 0.2f);
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

		ghostOnCooldown = true;
		timerAnimator.DelayAction(ghostCooldown, [&]() {ghostOnCooldown = false;});
	}

	void BackToMainMenu()
	{
		SceneManager::RequestLoadScene("MainMenu");
	}

	void TakeDamage(float dmg)
	{
		health -= dmg;
		lastDmgTime = std::chrono::high_resolution_clock::now();
		if (health > 0.0f)
		{
			AudioManager::PlayFromDefaultSource("damage4", false, 0.5f);
		}
		if (health <= 0.0f)
		{
			health = 0.0f;

			AudioManager::PlayFromDefaultSource("death", false, 0.5f);

			GetAnimator().TransitToAnimation("dying", 0.1f, AnimationClip::PlaybackMode::SINGLE);
			timerAnimator.AnimateVariable(&healthPanel->textureColor.color,
				healthPanel->textureColor.color,
				glm::vec4(0.0f, 0.0f, 0.0f, 0.8f),
				2.0f
			);
			timerAnimator.DelayAction(2.0f, [&]() {
				HFEngine::ECS.GetComponent<ParticleEmitter>(torchFlameParticleObject).emitting = false;
			});
			lostGamePanel->SetEnabled(true);
			timerAnimator.AnimateVariable(&lostGamePanel->textureColor.color,
				lostGamePanel->textureColor.color,
				glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
				3.0f
			);
			for (int i = (int)Button::STATE::NORMAL; i <= (int)Button::STATE::PRESSED; i++)
			{
				timerAnimator.AnimateVariable(&lostGameButton->textureColors[(Button::STATE)i].color,
					glm::vec4(glm::vec3(1.0f), 0.0f),
					lostGameButton->textureColors[(Button::STATE)i].color,
					3.0f
				);
			}
			lostGameButton->SetEnabled(true);
			EventManager::FireEvent(Events::Gameplay::Player::DEATH);
			AudioManager::PlayFromDefaultSource("ghostly_game_over", false, 2.0f);

		}
	}

	void Update(float dt)
	{
		timerAnimator.Process(dt);

		auto& transform = GetTransform();
		auto& animator = GetAnimator();
		auto& rigidBody = GetRigidBody();

		if (IsDead()) return;

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
			if (!hasGhostMovement && !ghostOnCooldown && InputManager::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
				EventManager::FireEvent(Events::Gameplay::Ghost::MOVEMENT_START);
			else if (hasGhostMovement && InputManager::GetMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT))
				EventManager::FireEvent(Events::Gameplay::Ghost::MOVEMENT_STOP);

			else if (!onPushBackCooldown && InputManager::GetKeyDown(GLFW_KEY_SPACE) &&  !hasGhostMovement)
			{
				StartPushBack();
				isPushingBack = true;
				onPushBackCooldown = true;
			}

			auto stopTime = std::chrono::high_resolution_clock::now();
			auto diff = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - lastDmgTime).count();
			if (diff >= idleToStartRecoveryTime && health < maxHealth)
			{
				health = std::min(health + healthRecoverySpeed * dt, maxHealth);
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
				ghostController->GetUpgradedMaxGhostDistance(),
				ghostController->leftGhostDistance + dt * ghostController->GetUpgradedDistanseRecoverySpeed()
			);
		}

		if (InputManager::GetKeyDown(GLFW_KEY_X))
		{
			rigidBody.isFalling = true;
			transform.TranslateSelf(glm::vec3(0.0f, 15.0f, 0.0f));
		}

		if (InputManager::GetKeyDown(GLFW_KEY_U))
		{
			TakeDamage(maxHealth / 10.0f);
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
			TakeDamage(maxHealth);
		}

		float ghostLevel = ghostController->GetLeftGhostLevel();
		ghostCircleBarPanel->SetCircleFilling(ghostLevel);
		if (ghostLevel > 0.99f)
		{
			if (!isGhostCircleFaded)
			{
				isGhostCircleFaded = true;
				timerAnimator.AnimateVariable(&ghostCircleBarPanel->textureColor.color.a, 0.25f, 0.0f, 0.2f);
			}
		}
		else
		{
			isGhostCircleFaded = false;
			ghostCircleBarPanel->textureColor.color.a = 0.75f;
		}
		if (!isHealthPulsing)
			healthPanel->textureColor.color = glm::vec4(1.0f, 1.0f, 1.0f, (1.0f - health / maxHealth) * healthMaxOpacity);

		if (GetHealth() / GetMaxHealth() <= healthPercentagePulsing)
		{
			if (!isHealthPulsing)
			{
				PulseHealth();
			}
			
		}
		healthPanel->SetSize(healthPanelSize);
	}

	bool IsDead()
	{
		return health <= 0;
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

		if((InputManager::GetKeyStatus(GLFW_KEY_W) || InputManager::GetKeyStatus(GLFW_KEY_S) ||
			InputManager::GetKeyStatus(GLFW_KEY_A) || InputManager::GetKeyStatus(GLFW_KEY_D)) && isReadyToStartMovement == true)
		{
			AudioManager::PlayMovement();
			isReadyToStartMovement = false;
		}
		else if ((!InputManager::GetKeyStatus(GLFW_KEY_W) && !InputManager::GetKeyStatus(GLFW_KEY_S) &&
			!InputManager::GetKeyStatus(GLFW_KEY_A) && !InputManager::GetKeyStatus(GLFW_KEY_D)) && isReadyToStartMovement == false)
		{
			AudioManager::StopMovement();
			isReadyToStartMovement = true;
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
			Move(transform.GetFront(), dt);

		return isMoving;
	}

	void PulseHealth()
	{
		if (!isHealthPulsing)
		{
			isHealthPulsing = true;
			timerAnimator.AnimateVariable(&healthPanelSize, healthPanel->GetSize(), { 1.0f, 1.0f }, healthPulseTime / 2.0f);
			timerAnimator.AnimateVariable(&healthPanel->textureColor.color, healthPanel->textureColor.color, glm::vec4(glm::vec3(1.0f), healthPulsingMaxOpacity), healthPulseTime / 2.0f);
			timerAnimator.DelayAction(healthPulseTime / 2.0f, [&]() {

				timerAnimator.AnimateVariable(&healthPanelSize, healthPanel->GetSize(), { 1.2f, 1.2f }, healthPulseTime / 2.0f);
				timerAnimator.AnimateVariable(&healthPanel->textureColor.color, healthPanel->textureColor.color, glm::vec4(glm::vec3(1.0f), (1.0f - healthPercentagePulsing) * healthMaxOpacity), healthPulseTime / 2.0f);
				});
			timerAnimator.DelayAction(healthPulseTime + healthPulsePauseTime, [&]() {
				isHealthPulsing = false;
				});
		}
	}

	void StartPushBack()
	{
		auto& emitterSmoke = HFEngine::ECS.GetComponent<ParticleEmitter>(attackSmokeObject);
		auto& emitterTorch = HFEngine::ECS.GetComponent<ParticleEmitter>(torchFlameParticleObject);
		auto& lightTorch = HFEngine::ECS.GetComponent<PointLightRenderer>(torchFlameLightObject);
 
		AudioManager::PlayFromDefaultSource("pushback", false);

		// anim & pushback stuff
		timerAnimator.DelayAction(0.2f, [&]() {
			emitterSmoke.emitting = true;
			});
		timerAnimator.DelayAction(0.35f, [&]() {
			Pushback();
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

	void Pushback()
	{
		auto objects = HFEngine::ECS.GetGameObjectsByName("enemy");
		glm::vec3 dir;
		auto pos = GetTransform().GetWorldPosition();

		Event evDistance(Events::StatModification::PUSHBACK_DISTANCE);
		Event evForce(Events::StatModification::PUSHBACK_FORCE);
		evDistance.SetParam(Events::StatModification::FloatValue, pushBackDistance);
		evForce.SetParam(Events::StatModification::FloatValue, pushBackForce);
		EventManager::FireEvent(evDistance);
		EventManager::FireEvent(evForce);
		float upgradedPushBackDistance = evDistance.GetParam<float>(Events::StatModification::FloatValue);
		float upgradedPushBackForce = evForce.GetParam<float>(Events::StatModification::FloatValue);

		Event ev(Events::Gameplay::Player::PUSHBACK_ENEMIES);
		ev.SetParam(Events::Gameplay::Player::Position, pos);
		ev.SetParam(Events::Gameplay::Player::PushBackDistance, upgradedPushBackDistance);
		ev.SetParam(Events::Gameplay::Player::PushBackForce, upgradedPushBackForce);
		EventManager::FireEvent(ev);

		for (auto& object : objects)
		{
			auto objPos = HFEngine::ECS.GetComponent<Transform>(object).GetWorldPosition();
			dir = objPos - pos;
			if (VECLEN(dir) < upgradedPushBackDistance)
			{
				auto& objectRb = HFEngine::ECS.GetComponent<RigidBody>(object);
				objectRb.AddForce(glm::normalize(glm::normalize(dir / 2.0f) + glm::vec3(0.0f, 0.75f, 0.0f)) * upgradedPushBackForce);
				objectRb.isFalling = true;
			}
		}
	}

	void SetIsReadyToStartMovementTrue()
	{
		isReadyToStartMovement = true;
	}

};


#undef GetTransform()
#undef GetAnimator()
#undef GetRigidBody()