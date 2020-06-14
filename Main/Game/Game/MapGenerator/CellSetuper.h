#pragma once

#include "ECS/ECSTypes.h"
#include "Config.h"
#include "ECS/Components/BoxCollider.h"
#include "ECS/Components/CircleCollider.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "HFEngine.h"


class CellSetuper
{
public:
	struct Zone {
		// all in local positions
		std::vector<glm::vec2> points;
		glm::vec2 center;
		int ind;
	};
	struct ZoneRoad {
		glm::vec2 gatePosition;
		float angle;
	};

private:
	CellSetupConfig setupConfig;
	GameObject structureContainer;
	GameObject obstacleContainer;
	GameObject enemiesContainer;

public:
	const GameObject cell;
	const MapCell::Type type;

private:
	std::vector<Zone> zones;
	std::vector<GameObject> tempColliders;
	std::vector<float> rotations;
	int largestZoneSize;

public:
	CellSetuper(CellSetupConfig& setupConfig, GameObject cell)
		: setupConfig(setupConfig), cell(cell),
		type(HFEngine::ECS.GetComponent<MapCell>(cell).CellType) {}

	void Setup();

	bool _debugLiteMode = false;
private:;
	void SpawnStructure(std::shared_ptr<Prefab> prefab, glm::vec2 localPos, float rotation);
	void SpawnObstacle(std::shared_ptr<Prefab> prefab, glm::vec2 localPos, float rotation);
	void SpawnEnemy(std::shared_ptr<Prefab> prefab, glm::vec2 localPos, float rotation);

	void MakeZones();
	void MakePathfindingGrid();

	void PrepareColliders();
	void UpdateColliders();
	void ClearTempColliders();

	void CreateFenceFires(std::shared_ptr<Prefab> firePrefab);

	glm::vec2 DrawPointInZone(Zone& zone, const BoxCollider& boxCollider, glm::quat& rotation, int number);
	glm::vec2 DrawPointInZone(Zone& zone, const CircleCollider& circleCollider, int number);
};

