#pragma once

#include <map>

#include "../Script.h"
#include "../../HFEngine.h"
#include "../../ECS/Components.h"
#include "../../InputManager.h"
#include "../../ECS/Systems/MapCellCollectorSystem.h"
#include "../../MapGenerator/Config.h"
#include "Event/Events.h"

class MapCellOptimizer : public Script
{
public:
	void Awake()
	{
		newCurrentCell = NULL_GAMEOBJECT;
		currentCell = NULL_GAMEOBJECT;
	}

	void Start()
	{
		// TODO: maybe create map generated event?
		auto gates = HFEngine::ECS.GetGameObjectsByName("GateTrigger");
		for (auto& gate : gates)
		{
			auto& collider = HFEngine::ECS.GetComponent<Collider>(gate);
			collider.OnTriggerEnter.push_back(TriggerMethodPointer(MapCellOptimizer::OnTriggerEnter));
		}

		auto cellsContainer = HFEngine::ECS.GetGameObjectByName("Cells");
		cells = gameObjectHierarchy.GetChildren(cellsContainer.value());
	}

	void EnableOptimizer()
	{
		CalculateCurrentCell();
		EnableDisableCells();
	}

	void LateUpdate(float dt)
	{
		if (newCurrentCell != currentCell)
		{
			currentCell = newCurrentCell;
			EnableDisableCells();

			Event ev(Events::Gameplay::Player::CELL_ENTERED);
			ev.SetParam(Events::GameObject::GameObject, currentCell);
			EventManager::FireEvent(ev);
		}
	}

	void OnTriggerEnter(GameObject that, GameObject other)
	{
		if (other == GetGameObject())
		{
			auto gate = gameObjectHierarchy.GetParent(that);
			auto& cellGate = HFEngine::ECS.GetComponent<CellGate>(gate.value());
			newCurrentCell = cellGate.Cell;
		}
	}

	GameObject GetCurrentCell()
	{
		return currentCell;
	}

private:
	void CalculateCurrentCell() // TODO: temporary, only start function
	{
		// TODO: make it somehow so it doesnt have to be calculated
		auto position = HFEngine::ECS.GetComponent<Transform>(GetGameObject()).GetWorldPosition();
		float dist, minDist = std::numeric_limits<float>::max();
		float x, z;
		currentCell = NULL_GAMEOBJECT;
		for (auto cell : cells)
		{
			auto positionCell = HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition();
			x = positionCell.x - position.x;
			z = positionCell.z - position.z;
			dist = sqrt((x * x) + (z * z));
			if (dist < minDist)
			{
				minDist = dist;
				newCurrentCell = cell;
			}
		}
	}

	void EnableDisableCells()
	{
		if (currentCell != NULL_GAMEOBJECT)
		{
			tsl::robin_set<GameObject> enabledCells;
			enabledCells.insert(currentCell);
			auto mapCell = HFEngine::ECS.GetComponent<MapCell>(currentCell);
			for (auto& bridge : mapCell.Bridges)
			{
				enabledCells.insert(bridge.Cell);
			}
			for (auto& cell : cells)
			{
				const auto& cellInfo = HFEngine::ECS.GetComponent<MapCell>(cell);
				if (enabledCells.find(cell) != enabledCells.end())
				{
					HFEngine::ECS.SetEnabledGameObject(cell, true);
					HFEngine::ECS.SetEnabledGameObject(cellInfo.EnemyContainer, true);
				}
				else
				{
					HFEngine::ECS.SetEnabledGameObject(cell, false);
					HFEngine::ECS.SetEnabledGameObject(cellInfo.EnemyContainer, false);
				}
			}
		}
	}

	std::shared_ptr<System> mapCellCollector;
	GameObject newCurrentCell;
	GameObject currentCell;
	std::vector<GameObject> cells;
};
