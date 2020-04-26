#pragma once

#include <map>

#include "../Script.h"
#include "../../HFEngine.h"
#include "../../ECS/Components.h"
#include "../../InputManager.h"
#include "../../ECS/Systems/MapCellCollectorSystem.h"
#include "../../MapGenerator/Config.h"

class MapCellOptimizer : public Script
{
public:
	void Awake()
	{
	}

	void Start()
	{
		mapCellCollector = HFEngine::ECS.GetSystemByTypeName(typeid(MapCellCollectorSystem).name());
		cells = mapCellCollector->gameObjects;
		CalculateCurrentCell();
		CalculateEnabledCells();
		EnableDisableCells();
	}

	void LateUpdate(float dt)
	{
		auto position = HFEngine::ECS.GetComponent<Transform>(GetGameObject()).GetWorldPosition();
		auto positionCell = HFEngine::ECS.GetComponent<Transform>(currentCell).GetWorldPosition();
		/*float x, z;
		x = positionCell.x - position.x;
		z = positionCell.z - position.z;
		float dist = sqrt((x * x) + (z * z));
		if (dist > cellMeshConfig.depthMax)
		{
			CalculateCurrentCell();
			EnableDisableCells();
		}*/
		auto mapCell = HFEngine::ECS.GetComponent<MapCell>(currentCell);
		if (!mapCell.PolygonBase.IsPointInside(glm::vec2(position.x - positionCell.x, position.z - positionCell.z)))
		{
			CalculateCurrentCell();
			CalculateEnabledCells();
			EnableDisableCells();
		}
	}

private:
	static const int enabledCellsCount = 10; // not counting current cell
	std::shared_ptr<System> mapCellCollector;
	std::set<GameObject> cells;
	GameObject currentCell;
	GameObject enableCellsList[enabledCellsCount];
	CellMeshConfig cellMeshConfig;

	void CalculateCurrentCell()
	{
		auto position = HFEngine::ECS.GetComponent<Transform>(GetGameObject()).GetWorldPosition();
		float dist, minDist = std::numeric_limits<float>::max();
		float x, z;
		currentCell = NULL_GAMEOBJECT;
		for (auto cell : cells)
		{
			auto mapCell = HFEngine::ECS.GetComponent<MapCell>(cell);
			auto positionCell = HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition();
			x = positionCell.x - position.x;
			z = positionCell.z - position.z;
			dist = sqrt((x * x) + (z * z));
			if (dist < minDist)
			{
				minDist = dist;
				currentCell = cell;
			}
		}
	}

	void CalculateEnabledCells()
	{
		auto currentCellPosition = HFEngine::ECS.GetComponent<Transform>(currentCell).GetWorldPosition();
		float dist;
		float x, z;
		std::deque<std::pair<GameObject, float>> cellDist;
		for (auto cell : cells)
		{
			auto mapCell = HFEngine::ECS.GetComponent<MapCell>(cell);
			auto positionCell = HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition();
			x = positionCell.x - currentCellPosition.x;
			z = positionCell.z - currentCellPosition.z;
			dist = sqrt((x * x) + (z * z));
			cellDist.push_back(std::pair<GameObject, float>(cell, dist));
		}
		if (cellDist.size() > 0)
		{
			std::sort(cellDist.begin(), cellDist.end(), [](const std::pair<GameObject, float>& cell1, const std::pair<GameObject, float>& cell2) {
				return cell1.second < cell2.second;
			});
		}
		for (int i = 0; i < enabledCellsCount; i++)
		{
			if (i >= cellDist.size())
			{
				enableCellsList[i] = NULL_GAMEOBJECT;
			}
			else
			{
				enableCellsList[i] = cellDist[i].first;
			}
		}
	}

	void EnableDisableCells()
	{
		for (auto cell : cells)
		{
			bool enabled = false;
			for (auto listCell : enableCellsList)
			{
				if (cell == listCell) enabled = true;
			}
			HFEngine::ECS.SetEnabledGameObject(cell, enabled);
		}
	}
};
