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

	TimerAnimator timerAnimator;

public:
	void Awake()
	{
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Player::CELL_ENTERED, CellSupervisor::OnCellEntered));
	}

	void Start()
	{
	}

	void OnCellEntered(Event& ev)
	{
		isThisCurrentCell = GetGameObject() == ev.GetParam<GameObject>(Events::GameObject::GameObject);
		if (isThisCurrentCell)
		{
			MapCell& mc = HFEngine::ECS.GetComponent<MapCell>(GetGameObject());
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
				timerAnimator.DelayAction(0.5f, std::bind(&CellSupervisor::OpenGatesHere, this));
			}
		}
	}
};
