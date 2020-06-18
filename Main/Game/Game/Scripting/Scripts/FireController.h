#pragma once

#include "ObstacleController.h"

class FireController : public ObstacleController
{
	struct FireEffect
	{
		std::chrono::steady_clock::time_point spawnTime;
		std::chrono::steady_clock::time_point lastDmgTime;
		std::shared_ptr<CreatureController> controller;
		GameObject object;
	};
private: // PARAMETERS
	float dmgTickTime = 1.0f;
	float dmgTick = 5.0f;
	float duration = 3.0f;
private: // VARIABLES
	std::unordered_map<GameObject, FireEffect> refreshList;
	std::shared_ptr<Prefab> firedEffectPrefab;
	ALuint sourceFire;
	ALuint sourceFireScreaming;

public:
	FireController()
	{
		RegisterFloatParameter("dmgTickTime", &dmgTickTime);
		RegisterFloatParameter("dmgTick", &dmgTick);
		RegisterFloatParameter("duration", &duration);
	}

	void Start() override
	{
		ObstacleController::Start();
		firedEffectPrefab = PrefabManager::GetPrefab("FiredCreatureEffect");

		AudioManager::InitSource(sourceFire);
		AudioManager::SetSoundInSource(sourceFire, "fire", false, 0.2f);

		AudioManager::InitSource(sourceFireScreaming);
		AudioManager::SetSoundInSource(sourceFireScreaming, "fire_screaming", false);
	}

	void Update(float dt)
	{
		ObstacleController::Update(dt);
		auto timePoint = std::chrono::steady_clock::now();
		for (auto it = refreshList.begin(); it != refreshList.end();)
		{
			if (!HFEngine::ECS.IsValidGameObject(it->first))
			{
				it = refreshList.erase(it);
				continue;
			}

			if (activeGameObjects.find(it->first) != activeGameObjects.end())
			{
				it->second.spawnTime = timePoint;
			}
			if (std::chrono::duration<float, std::chrono::seconds::period>(timePoint - it->second.spawnTime).count() >= duration)
			{
				HFEngine::ECS.DestroyGameObject(it->second.object);
				it = refreshList.erase(it);
			} else
			{
				if (std::chrono::duration<float, std::chrono::seconds::period>(timePoint - it->second.lastDmgTime).count() >= dmgTickTime)
				{
					it->second.controller->TakeDamage(dmgTick);
					it->second.lastDmgTime = timePoint;
				}
				it++;
			}
		}
	}

	void OnObstacleEnter(GameObject object) override
	{
		if (refreshList.find(object) == refreshList.end())
		{
			auto& controller = controllers[object];
			FireEffect effect;
			effect.controller = controller;
			effect.object = firedEffectPrefab->Instantiate(object);
			effect.spawnTime = std::chrono::steady_clock::now();
			refreshList[object] = effect;

			auto& scriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(object);
			if (controllers[object] == scriptContainer.GetScript<PlayerController>())
			{
				AudioManager::PlaySoundFromSource(sourceFire);

				if (scriptContainer.GetScript<PlayerController>()->GetHealth() < scriptContainer.GetScript<PlayerController>()->GetMaxHealth() *0.2f)
				{
					AudioManager::PlaySoundFromSource(sourceFireScreaming);
				}
			}
		}
	}

	void OnObstacleExit(GameObject object) override
	{
		AudioManager::StopSource(sourceFire);
		AudioManager::StopSource(sourceFireScreaming);
	}

};
