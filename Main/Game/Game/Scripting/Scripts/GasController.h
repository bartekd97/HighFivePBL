#pragma once

#include "ObstacleController.h"

class GasController : public ObstacleController
{
	struct GasEffect
	{
		std::chrono::steady_clock::time_point spawnTime;
		std::chrono::steady_clock::time_point lastDmgTime;
		std::shared_ptr<CreatureController> controller;
		GameObject object;
	};
private: // PARAMETERS
	float dmgTickTime = 1.0f;
	float dmgTick = 3.0f;
	float duration = 6.0f;
	float slowForce = 2.5f;
private: // VARIABLES
	std::unordered_map<GameObject, GasEffect> refreshList;
	std::shared_ptr<Prefab> gasEffectPrefab;
	ALuint source;
public:
	GasController()
	{
		RegisterFloatParameter("dmgTickTime", &dmgTickTime);
		RegisterFloatParameter("dmgTick", &dmgTick);
		RegisterFloatParameter("duration", &duration);
		RegisterFloatParameter("slowForce", &slowForce);
	}

	void Start() override
	{
		ObstacleController::Start();
		gasEffectPrefab = PrefabManager::GetPrefab("GasCreatureEffect");
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
			}
			else
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
		auto& controller = controllers[object];
		if (refreshList.find(object) == refreshList.end())
		{
			GasEffect effect;
			effect.controller = controller;
			effect.object = gasEffectPrefab->Instantiate(object);
			effect.spawnTime = std::chrono::steady_clock::now();
			refreshList[object] = effect;
			
			auto& scriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(object);
			if (controllers[object] == scriptContainer.GetScript<PlayerController>())
			{
				float playerHealth = scriptContainer.GetScript<PlayerController>()->GetHealth();
				float playerMaxHealth = scriptContainer.GetScript<PlayerController>()->GetMaxHealth();
				if (playerHealth < playerMaxHealth * 0.5f)
				{
					AudioManager::CreateDefaultSourceAndPlay(source, "choking", false);
				}
			}
			
		}
		if (!controller->GetIsFlying()) controller->SetSlow(controller->GetSlow() + slowForce);
	}

	void OnObstacleExit(GameObject object) override
	{

		auto& controller = controllers[object];
		if (!controller->GetIsFlying()) controller->SetSlow(controller->GetSlow() - slowForce);

		AudioManager::StopSource(source);
	}
};
