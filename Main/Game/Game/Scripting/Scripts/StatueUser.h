#pragma once

#include <glm/glm.hpp>
#include "../Script.h"
#include "Event/Events.h"
#include "ECS/Components/MeshRenderer.h"
#include "ECS/Components/PointLightRenderer.h"
#include "HFEngine.h"
#include "Utility/TimerAnimator.h"

class StatueUser : public Script
{
public:
	enum class State {
		INACTIVE,
		USABLE,
		USED
	};
private: // parameters
	glm::vec3 usableMaxPulseEmissive = { 0.012f, 0.012f, 0.0f };
	glm::vec3 usedEmissive = { 0.007f, 0.007f, 0.007f };
	glm::vec3 usedFlashEmissive = { 0.25f, 1.0f, 1.0f };
	float usedFlashOpacity = 0.8f;
	float usedLightIntensity = 60.0f;

private: // variables
	GameObject flashObject;

	glm::vec3 defaultEmissive;

	float dtsum = 0.0f;
	TimerAnimator timerAnimator;
	ALuint sourceStatue;

	State currentState = State::INACTIVE;

public:
	StatueUser()
	{
		RegisterVec3Parameter("usableMaxPulseEmissive", &usableMaxPulseEmissive);
		RegisterVec3Parameter("usedEmissive", &usedEmissive);
		RegisterVec3Parameter("usedFlashEmissive", &usedFlashEmissive);
		RegisterFloatParameter("usedFlashOpacity", &usedFlashOpacity);
		RegisterFloatParameter("usedLightIntensity", &usedLightIntensity);
	}

	void Awake()
	{
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Statue::MAKE_ME_USABLE, StatueUser::OnMakeMeUsable));
		AudioManager::InitSource(sourceStatue);
		AudioManager::SetSoundInSource(sourceStatue, "bum2", false, 0.5f);
	}

	void Start()
	{
		flashObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "StatueFlashObject")[0];
		HFEngine::ECS.GetComponent<MeshRenderer>(flashObject).material->opacityValue = 0.0f;

		defaultEmissive = HFEngine::ECS.GetComponent<MeshRenderer>(GetGameObject()).material->emissiveColor;

		auto& collider = HFEngine::ECS.GetComponent<Collider>(GetGameObject());
		collider.OnTriggerEnter.push_back(TriggerMethodPointer(StatueUser::OnTriggerEnter));
	}

	void OnMakeMeUsable(Event& ev)
	{
		if (currentState != State::INACTIVE) return;

		currentState = State::USABLE;
		dtsum = (M_PI * -0.5f) / 7.0f; // to start from -1 in sin
		HFEngine::ECS.GetComponent<MeshRenderer>(flashObject).material->opacityValue = 0.0f;
	}

	void OnTriggerEnter(GameObject that, GameObject other)
	{
		if (currentState != State::USABLE) return;

		std::string otherName = HFEngine::ECS.GetNameGameObject(other);

		if (otherName == "PlayerGhost")
		{
			currentState = State::USED;

			auto& thisRenderer = HFEngine::ECS.GetComponent<MeshRenderer>(GetGameObject());
			auto& flashRenderer = HFEngine::ECS.GetComponent<MeshRenderer>(flashObject);
			timerAnimator.AnimateVariable(&thisRenderer.material->emissiveColor, thisRenderer.material->emissiveColor, usedEmissive, 1.0f);
			timerAnimator.AnimateVariable(&flashRenderer.material->emissiveColor, flashRenderer.material->emissiveColor, usedFlashEmissive, 1.0f);
			timerAnimator.AnimateVariable(&flashRenderer.material->opacityValue, flashRenderer.material->opacityValue, usedFlashOpacity, 1.0f);

			for (auto& lightObject : HFEngine::ECS.GetGameObjectsWithComponentInChildren<PointLightRenderer>(GetGameObject()))
			{
				auto& lightRenderer = HFEngine::ECS.GetComponent<PointLightRenderer>(lightObject);
				timerAnimator.AnimateVariable(&lightRenderer.light.intensity, lightRenderer.light.intensity, usedLightIntensity, 1.0f);
			}
			AudioManager::PlaySoundFromSource(sourceStatue);

			EventManager::FireEvent(Events::Gameplay::Ghost::MOVEMENT_CANCEL);
			EventManager::FireEvent(Events::Gameplay::Upgrades::REQUEST_UPGRADE);
		}
	}

	void Update(float dt)
	{
		if (currentState == State::INACTIVE) return;

		timerAnimator.Process(dt);

		if (currentState == State::USABLE)
		{
			dtsum += dt;
			float level = glm::sin(dtsum * 7.0f);

			auto& thisRenderer = HFEngine::ECS.GetComponent<MeshRenderer>(GetGameObject());
			thisRenderer.material->emissiveColor = glm::mix(
				defaultEmissive,
				usableMaxPulseEmissive,
				(level + 1.0f) * 0.5f
				);
			auto& flashRenderer = HFEngine::ECS.GetComponent<MeshRenderer>(flashObject);
			flashRenderer.material->opacityValue = 0.5f + 0.5f * level;
		}

	}
};
