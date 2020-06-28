#pragma once

#include "../Script.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "Event/Events.h"
#include "Utility/TimerAnimator.h"
#include "HFEngine.h"

class GateOpener : public Script
{
public:
	enum class State {
		OPENED,
		CLOSED
	};

private: // parameters
	std::string leftWingName = "LeftWingAxle";
	std::string rightWingName = "RightWingAxle";
	glm::vec3 leftRotationOpened = { 0.0f, 90.0f, 0.0f };
	glm::vec3 leftRotationClosed = { 0.0f, 0.0f, 0.0f };
	glm::vec3 rightRotationOpened = { 0.0f, -90.0f, 0.0f };
	glm::vec3 rightRotationClosed = { 0.0f, 0.0f, 0.0f };
	float animTime = 1.0f;

	float lighOnIntenisity = 1.0f;
	glm::vec3 lighOnEmissive = { 1.0f, 1.0f, 1.0f };
	float lightOnAnimTime = 1.5f;

private:
	GameObject leftWingObject;
	GameObject rightWingObject;

	State currentState = State::OPENED;

	std::vector<GameObject> lampRenderers;
	std::vector<GameObject> lampLights;

	TimerAnimator timerAnimator;

public:

	GateOpener()
	{
		RegisterStringParameter("leftWingName", &leftWingName);
		RegisterStringParameter("rightWingName", &rightWingName);

		RegisterVec3Parameter("leftRotationOpened", &leftRotationOpened);
		RegisterVec3Parameter("leftRotationClosed", &leftRotationClosed);
		RegisterVec3Parameter("rightRotationOpened", &rightRotationOpened);
		RegisterVec3Parameter("rightRotationClosed", &rightRotationClosed);

		RegisterFloatParameter("animTime", &animTime);

		RegisterFloatParameter("lighOnIntenisity", &lighOnIntenisity);
		RegisterVec3Parameter("lighOnEmissive", &lighOnEmissive);
		RegisterFloatParameter("lightOnAnimTime", &lightOnAnimTime);
	}

	void Awake()
	{
		leftWingObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), leftWingName)[0];
		rightWingObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), rightWingName)[0];
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Gate::OPEN_ME, GateOpener::OnOpenMe));
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Gate::CLOSE_ME, GateOpener::OnCloseMe));
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Gate::LIGHTON_ME, GateOpener::OnLightOnMe));
	}

	void Start()
	{
		lampRenderers = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "GateLamp");
		lampLights = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "GateLampLight");
	}

	void Update(float dt)
	{
		timerAnimator.Process(dt);
	}

	void OnOpenMe(Event& ev)
	{
		if (currentState == State::OPENED) return;

		Transform& leftTransform = HFEngine::ECS.GetComponent<Transform>(leftWingObject);
		Transform& rightTransform = HFEngine::ECS.GetComponent<Transform>(rightWingObject);

		timerAnimator.UpdateInTime(animTime, [&](float prog) {
			leftTransform.SetRotation(glm::mix(leftRotationClosed, leftRotationOpened, prog));
			rightTransform.SetRotation(glm::mix(rightRotationClosed, rightRotationOpened, prog));
		});

		currentState = State::OPENED;
		AudioManager::PlayFromDefaultSource("squeaky_metal_gate", false, 0.05f);

	}

	void OnCloseMe(Event& ev)
	{
		if (currentState == State::CLOSED) return;

		Transform& leftTransform = HFEngine::ECS.GetComponent<Transform>(leftWingObject);
		Transform& rightTransform = HFEngine::ECS.GetComponent<Transform>(rightWingObject);

		timerAnimator.UpdateInTime(animTime, [&](float prog) {
			leftTransform.SetRotation(glm::mix(leftRotationOpened, leftRotationClosed, prog));
			rightTransform.SetRotation(glm::mix(rightRotationOpened, rightRotationClosed, prog));
			});
		AudioManager::PlayFromDefaultSource("squeaky_metal_gate", false, 0.05f);

		currentState = State::CLOSED;
	}

	void OnLightOnMe(Event& ev)
	{
		for (auto& go : lampRenderers)
		{
			MeshRenderer& renderer = HFEngine::ECS.GetComponent<MeshRenderer>(go);
			timerAnimator.AnimateVariable(&renderer.material->emissiveColor, renderer.material->emissiveColor, lighOnEmissive, lightOnAnimTime);
		}

		for (auto& go : lampLights)
		{
			PointLightRenderer& renderer = HFEngine::ECS.GetComponent<PointLightRenderer>(go);
			timerAnimator.AnimateVariable(&renderer.light.intensity, renderer.light.intensity, lighOnIntenisity, lightOnAnimTime);
		}
	}
};
