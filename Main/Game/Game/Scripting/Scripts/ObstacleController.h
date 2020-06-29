#pragma once

#include "../Script.h"
#include "../../HFEngine.h"
#include "../../ECS/Components/Collider.h"
#include "../../ECS/Components/ScriptContainer.h"
#include "PlayerController.h"
#include "EnemyController.h"

const char* playerName = "Player";
const char* enemyName = "enemy";

class ObstacleController : public Script
{
private:
	bool IsCreature(GameObject gameObject)
	{
		if (!HFEngine::ECS.IsValidGameObject(gameObject)) return false;
		auto name = HFEngine::ECS.GetNameGameObject(gameObject);
		if (strcmp(name, playerName) == 0) return true;
		if (strcmp(name, enemyName) == 0) return true;
		return false;
	}

protected:
	std::unordered_map<GameObject, int> activeGameObjects;
	std::unordered_map<GameObject, std::shared_ptr<CreatureController>> controllers;

public:
	void Start()
	{
		auto collidersObjects = HFEngine::ECS.GetGameObjectsWithComponentInChildren<Collider>(GetGameObject());
		for (auto object : collidersObjects)
		{
			auto& collider = HFEngine::ECS.GetComponent<Collider>(object);
			collider.OnTriggerEnter.push_back(TriggerMethodPointer(ObstacleController::OnTriggerEnter));
			collider.OnTriggerExit.push_back(TriggerMethodPointer(ObstacleController::OnTriggerExit));
		}
	}

	void Update(float dt)
	{
		for (auto it = activeGameObjects.begin(); it != activeGameObjects.end(); )
		{
			if (!HFEngine::ECS.IsValidGameObject(it->first))
			{
				controllers.erase(it->first);
				it = activeGameObjects.erase(it);
			}
			else
			{
				it++;
			}
		}
	}

	void OnTriggerEnter(GameObject that, GameObject other)
	{
		if (IsCreature(other))
		{
			auto it = activeGameObjects.find(other);
			if (it == activeGameObjects.end())
			{
				auto name = HFEngine::ECS.GetNameGameObject(other);
				auto& scriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(other);
				if (strcmp(name, enemyName) == 0)
				{
					auto& rigidBody = HFEngine::ECS.GetComponent<RigidBody>(other);
					if (scriptContainer.GetScript<EnemyController>()->IsAvoiding(GetGameObject()) && !rigidBody.isFalling)
					{
						return;
					}
				}
				activeGameObjects[other] = 1;
				if (strcmp(name, playerName) == 0) controllers[other] = scriptContainer.GetScript<PlayerController>();
				if (strcmp(name, enemyName) == 0) controllers[other] = scriptContainer.GetScript<EnemyController>();
				OnObstacleEnter(other);
			}
			else
			{
				it->second += 1;
			}
		}
	}

	void OnTriggerExit(GameObject that, GameObject other)
	{
		if (IsCreature(other))
		{
			auto it = activeGameObjects.find(other);
			if (it != activeGameObjects.end())
			{
				it->second -= 1;
				if (it->second == 0)
				{
					activeGameObjects.erase(it);
					OnObstacleExit(other);
					controllers.erase(other);
				}
			}
		}
	}

	virtual void OnObstacleEnter(GameObject object) {}
	virtual void OnObstacleExit(GameObject object) {}
};
