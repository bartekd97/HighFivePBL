#pragma once

#include "ECS/ECSTypes.h"
#include "Config.h"
#include "ECS/Components/BoxCollider.h"
#include "ECS/Components/CircleCollider.h"


class CellSetuper
{
public:
	enum class Type {
		STARTUP,
		NORMAL,
		BOSS
	};

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
	const Type type;


private:
	std::vector<Zone> zones;
	std::vector<GameObject> tempColliders;
	int largestZoneSize;

public:
	CellSetuper(CellSetupConfig& setupConfig, GameObject cell, Type type)
		: setupConfig(setupConfig), cell(cell), type(type) {}

	void Setup();

	//glm::vec2 FindClosestNode(float xPosition, float yPosition);

private:;
	void SpawnStructure(std::shared_ptr<Prefab> prefab, glm::vec2 localPos, float rotation);
	void SpawnObstacle(std::shared_ptr<Prefab> prefab, glm::vec2 localPos, float rotation);
	void SpawnEnemy(std::shared_ptr<Prefab> prefab, glm::vec2 localPos, float rotation);

	void MakeZones();
	void MakePathfindingGrid();

	void PrepareColliders();
	void UpdateColliders();
	void ClearTempColliders();

	glm::vec2 DrawPointInZone(Zone& zone, const BoxCollider& boxCollider, glm::quat& rotation, int number);
	glm::vec2 DrawPointInZone(Zone& zone, const CircleCollider& circleCollider, int number);
};

