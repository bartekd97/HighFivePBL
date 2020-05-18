#pragma once

#include <glm/glm.hpp>

#include "../SystemUpdate.h"
#include "../Components.h"
#include "MapCellCollectorSystem.h"

#include "../../MapGenerator/Config.h"

class GravitySystem : public SystemUpdate
{
public:
	void Update(float dt) override;
	void SetCollector(std::shared_ptr<MapCellCollectorSystem> mapCellCollectorSystem);
private:
	bool GetCellYLevel(glm::vec2& position, MapCell& cell, float& level);
	void LoadCells();
	int GetClosestCellIndex(glm::vec3& position);
	GameObject GetBridge(GameObject gameObject, int closestCell);
	float GetBridgeLevel(glm::vec3& position, GameObject bridge);

	std::vector<std::pair<glm::vec3, MapCell>> cells;
	std::shared_ptr<MapCellCollectorSystem> mapCellCollectorSystem;
	CellMeshConfig config;
};
