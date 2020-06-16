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

private:
	GameObject leftWingObject;
	GameObject rightWingObject;

	State currentState = State::OPENED;
	ALuint sourceGate;

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
	}

	void Awake()
	{
		leftWingObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), leftWingName)[0];
		rightWingObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), rightWingName)[0];
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Gate::OPEN_ME, GateOpener::OnOpenMe));
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Gate::CLOSE_ME, GateOpener::OnCloseMe));
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
		AudioManager::CreateDefaultSourceAndPlay(sourceGate, "squeaky_metal_gate", false, 0.1f);

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
		AudioManager::CreateDefaultSourceAndPlay(sourceGate, "squeaky_metal_gate", false, 0.1f);

		currentState = State::CLOSED;
	}
};
