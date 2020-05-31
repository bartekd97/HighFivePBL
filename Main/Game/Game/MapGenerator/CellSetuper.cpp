#include <algorithm>
#include "CellSetuper.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "ECS/Components/BoxCollider.h"
#include "ECS/Components/Transform.h"
#include "HFEngine.h"
#include "Physics/Physics.h"
#include "Resourcing/Prefab.h"
#include "Physics/Raycaster.h"
#include "Physics/RaycastHit.h"
#include "Utility/Utility.h"
#include "Scripting/Script.h"
#include "Utility/Logger.h"


void CellSetuper::Setup()
{
	structureContainer = HFEngine::ECS.CreateGameObject(cell, "Structures");
	obstacleContainer = HFEngine::ECS.CreateGameObject(cell, "Obstacles");

	// spawn monument only on normal cell
	if (type == CellSetuper::Type::NORMAL)
	{
		// spawn main statue
		setupConfig.mainStatuePrefab->Instantiate(cell, { 0,0,0 }, { 0,25,0 });

		MakeZones();

		// TODO: better zone selecting, maybe another function?
		for (auto& zone : zones)
		{
			// TODO: obstacle/structure selector
			// maybe with few choices in case when first one can't be spawned?
			// and maybe choose few pseudo-random points instead of center?
			// and add raycast


			int objectsToGenerate;
			if (zone.points.size() > 200)
			{
				objectsToGenerate = 2;
			}
			else
			{
				objectsToGenerate = 1;
			}

			for (int i = 0; i < objectsToGenerate; i++)
			{
				if (zone.ind == 1)
				{
					auto structurePrefab = setupConfig.structurePrefabs.at(
						zone.points.size() * objectsToGenerate % setupConfig.structurePrefabs.size()
					);
					float obstacleRotation = 0.0f;// zone.center.x* zone.center.y;
					glm::quat q(0.0f, 0.0f, 0.0f, 0.0f);
					float width;
					float height;
					structurePrefab->Properties().GetFloat("width", width, 1.0f);
					structurePrefab->Properties().GetFloat("height", height, 1.0f);
					BoxCollider box;
					box.SetWidthHeight(width, height);
					if (zone.points.size() > 0)
					{
						glm::vec2 position = DrawPointInZone(zone, box, q, i);
						LogInfo("CellSetuper::MakeZones() Cell '{}' zone sizes: {}", position.x, position.y);
						if (position != glm::vec2(0.0f))
						{
							TrySpawnObstacle(structurePrefab, position, obstacleRotation);
						}
					}
				}
				else
				{
					auto obstaclePrefab = setupConfig.obstaclePrefabs.at(
						zone.points.size() * objectsToGenerate % setupConfig.obstaclePrefabs.size()
					);
					float obstacleRotation = zone.center.x * zone.center.y;
					glm::quat q(0.0f, 0.0f, 0.0f, 0.0f);
					float width;
					float height;
					obstaclePrefab->Properties().GetFloat("width", width, 1.0f);
					obstaclePrefab->Properties().GetFloat("height", height, 1.0f);
					BoxCollider box;
					box.SetWidthHeight(width, height);
					if (zone.points.size() > 0)
					{
						glm::vec2 position = DrawPointInZone(zone, box, q, i);
						LogInfo("CellSetuper::MakeZones() Cell '{}' zone sizes: {}", position.x, position.y);
						if (position != glm::vec2(0.0f))
						{
							TrySpawnObstacle(obstaclePrefab, position, obstacleRotation);
						}
					}
					
				}
			}

			
		}

	}
}


// only obstacles should have "random" rotation around Y axis
// structures should have predefined, constant rotation
// because some models have been made just to be visible from one side(s)
bool CellSetuper::TrySpawnObstacle(std::shared_ptr<Prefab> prefab, glm::vec2 localPos, float rotation)
{
	// TODO: check if its possible to spawn this obstacle here
	// maybe use prefab properties to read it's size for raycast?
	prefab->Instantiate(obstacleContainer, { localPos.x, 0.0f, localPos.y }, {0.0f, rotation, 0.0f});
	return true;
}

void CellSetuper::MakeZones()
{
	MapCell& cellInfo = HFEngine::ECS.GetComponent<MapCell>(cell);
	glm::vec2 cellPos = {
		HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition().x,
		HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition().z
	};

	std::vector<ZoneRoad> roads;
	for (auto bridge : cellInfo.Bridges)
	{
		ZoneRoad road;
		road.gatePosition = {
			HFEngine::ECS.GetComponent<Transform>(bridge.Gate).GetWorldPosition().x,
			HFEngine::ECS.GetComponent<Transform>(bridge.Gate).GetWorldPosition().z
		};
		road.angle = glm::atan(road.gatePosition.x - cellPos.x, road.gatePosition.y - cellPos.y);
		roads.push_back(road);
	}
	std::sort(roads.begin(), roads.end(), [](ZoneRoad a, ZoneRoad b) {
		return a.angle < b.angle;
		});

	auto getDistanceToRoad = [&](glm::vec2 point) {
		float minDistance = 99999.9f;
		for (auto& road : roads) {
			minDistance = glm::min(
				minDistance,
				Utility::GetDistanceBetweenPointAndSegment(point, cellPos, road.gatePosition)
				);
		}
		return minDistance;
	};
	auto findTargetZoneIndex = [&](float angle) {
		for (int i = 1; i < roads.size(); i++)
			if (roads[i - 1].angle < angle && angle < roads[i].angle)
				return i;
		return 0;
	};

	zones.resize(roads.size());

	for (int i = 0; i < setupConfig.gridSize; i += setupConfig.gridStep)
	{
		for (int j = 0; j < setupConfig.gridSize; j += setupConfig.gridStep)
		{
			glm::vec2 point = { i - (setupConfig.gridSize / 2), j - (setupConfig.gridSize / 2) };
			float level = cellInfo.PolygonSmoothInner.GetEdgeCenterRatio(point);
			
			// check if it isn too far or too near
			if (level < setupConfig.gridInnerLevel.x || level > setupConfig.gridInnerLevel.y)
				continue;

			// check if not too near road
			if (getDistanceToRoad(point + cellPos) < setupConfig.gridMinRoadDistance)
				continue;

			// point correct
			// get angle and target zone
			float angle = glm::atan(point.x, point.y);
			int zoneIndex = findTargetZoneIndex(angle);
			zones[zoneIndex].points.push_back(point);
			zones[zoneIndex].ind = zoneIndex;
		}
	}


	// calc zones center
	for (auto& zone : zones)
	{
		if (zone.points.size() == 0)
			continue;

		glm::vec2 sum = glm::vec2(0.0f);
		for (auto p : zone.points)
			sum += p;
		zone.center = sum / float(zone.points.size());
	}


#ifdef _DEBUG
	std::vector<int> _sizes;
	for (auto& zone : zones)
		_sizes.push_back(zone.points.size());
	LogInfo("CellSetuper::MakeZones() Cell '{}' zone sizes: {}", cellInfo.CellSiteIndex, _sizes);
#endif
}

glm::vec2 CellSetuper::DrawPointInZone(Zone& zone, const BoxCollider& boxCollider, glm::quat& rotation, int number)
{
	zone.points.size();
	glm::vec3 pos;
	int iter_available = 200;
	int randomNumber = ((int)zone.center.x * (int)zone.center.y * zones.size() * number * 7 +1) % zone.points.size();
	RaycastHit out;
	do
	{
		randomNumber = (randomNumber * (int)zone.center.x * (int)zone.center.y * zones.size() * number * 7 + 1) % zone.points.size();
		pos = glm::vec3(zone.points[randomNumber].x, 0.0f, zone.points[randomNumber].y);
		iter_available--;
	} while (iter_available > 0 && Physics::Raycast(pos, rotation, boxCollider, out) == true);

	if (Physics::Raycast(pos, rotation, boxCollider, out) == false)
	{
		return zone.points[randomNumber];

	}
	else
	{
		return glm::vec2(0.0f);

	}


	/*if (Physics::Raycast(glm::vec3(zone.points[randomNumber].x, 0.0f, zone.points[randomNumber].y), rotation, boxCollider, out) == false)
	{
		return zone.points[randomNumber];
	}
	else
	{
		return NULL;
	}*/

}
