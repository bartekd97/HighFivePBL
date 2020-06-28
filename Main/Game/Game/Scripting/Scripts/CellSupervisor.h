#pragma once

#include "../Script.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "Event/Events.h"
#include "HFEngine.h"
#include "Utility/TimerAnimator.h"
#include "InputManager.h"

class CellSupervisor : public Script
{
private:
	bool isThisCurrentCell = false;
	bool scanForNoEnemies = false;

	GameObject bossObjectHere = NULL_GAMEOBJECT;

	TimerAnimator timerAnimator;

public:
	void Awake()
	{
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Player::CELL_ENTERED, CellSupervisor::OnCellEntered));
	}

	void Start()
	{
		MapCell& mc = HFEngine::ECS.GetComponent<MapCell>(GetGameObject());
		if (mc.CellType == MapCell::Type::BOSS)
		{
			bossObjectHere = gameObjectHierarchy.GetChildren(mc.EnemyContainer)[0];
			EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Boss::TRIGGERED, CellSupervisor::OnBossTriggered));
			EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Boss::DEAD, CellSupervisor::OnBossDead));
		}
	}

	void OnCellEntered(Event& ev)
	{
		isThisCurrentCell = GetGameObject() == ev.GetParam<GameObject>(Events::GameObject::GameObject);
		if (isThisCurrentCell)
		{
			MapCell& mc = HFEngine::ECS.GetComponent<MapCell>(GetGameObject());
			if (mc.CellType == MapCell::Type::NORMAL)
			{
				auto& enemies = gameObjectHierarchy.GetChildren(mc.EnemyContainer);
				if (enemies.size() > 0)
				{
					CloseGatesHere();
					scanForNoEnemies = true;
				}
				else
				{
					scanForNoEnemies = false;
				}
			}
		}
	}

	void OnBossTriggered(Event& ev)
	{
		if (bossObjectHere == ev.GetParam<GameObject>(Events::GameObject::GameObject))
		{
			CloseGatesHere();
		}
	}
	void OnBossDead(Event& ev)
	{
		if (bossObjectHere == ev.GetParam<GameObject>(Events::GameObject::GameObject))
		{
			OpenGatesHere();
		}
	}

	void CloseGatesHere()
	{
		MapCell& mc = HFEngine::ECS.GetComponent<MapCell>(GetGameObject());
		for (auto bridge : mc.Bridges)
			EventManager::FireEventTo(bridge.Gate, Events::Gameplay::Gate::CLOSE_ME);
	}

	void OpenGatesHere()
	{
		MapCell& mc = HFEngine::ECS.GetComponent<MapCell>(GetGameObject());
		for (auto bridge : mc.Bridges)
			EventManager::FireEventTo(bridge.Gate, Events::Gameplay::Gate::OPEN_ME);
	}

	void LightOnGatesHere()
	{
		MapCell& mc = HFEngine::ECS.GetComponent<MapCell>(GetGameObject());
		for (auto bridge : mc.Bridges)
			EventManager::FireEventTo(bridge.Gate, Events::Gameplay::Gate::LIGHTON_ME);
	}

	void MakeStatueUsable()
	{
		MapCell& mc = HFEngine::ECS.GetComponent<MapCell>(GetGameObject());
		if (mc.Statue != NULL_GAMEOBJECT)
			EventManager::FireEventTo(mc.Statue, Events::Gameplay::Statue::MAKE_ME_USABLE);
	}

	void LateUpdate(float dt)
	{
		if (!isThisCurrentCell) return;

		timerAnimator.Process(dt);

		if (scanForNoEnemies)
		{
			MapCell& mc = HFEngine::ECS.GetComponent<MapCell>(GetGameObject());
			auto& enemies = gameObjectHierarchy.GetChildren(mc.EnemyContainer);
			if (enemies.size() == 0 || InputManager::GetKeyDown(GLFW_KEY_F8)) // TODO: remvoe this debug
			{
				scanForNoEnemies = false;
				MakeStatueUsable();
				LightOnGatesHere();
				timerAnimator.DelayAction(0.5f, std::bind(&CellSupervisor::OpenGatesHere, this));
			}
		}
	}
};
