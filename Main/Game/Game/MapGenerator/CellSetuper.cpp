#include <algorithm>
#include "CellSetuper.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "ECS/Components/Transform.h"
#include "HFEngine.h"
#include "Utility/Utility.h"
#include "Utility/Logger.h"


void CellSetuper::Setup()
{
	// spawn monument only on normal cell
	if (type == CellSetuper::Type::NORMAL)
	{
		// spawn main statue
		structuresConfig.mainStatuePrefab->Instantiate(cell, { 0,0,0 }, { 0,25,0 });
		MakeZones();
	}
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

	for (int i = 0; i < structuresConfig.gridSize; i += structuresConfig.gridStep)
	{
		for (int j = 0; j < structuresConfig.gridSize; j += structuresConfig.gridStep)
		{
			glm::vec2 point = { i - (structuresConfig.gridSize / 2), j - (structuresConfig.gridSize / 2) };
			float level = cellInfo.PolygonSmoothInner.GetEdgeCenterRatio(point);
			
			// check if it isn too far or too near
			if (level < structuresConfig.gridInnerLevel.x || level > structuresConfig.gridInnerLevel.y)
				continue;

			// check if not too near road
			if (getDistanceToRoad(point + cellPos) < structuresConfig.gridMinRoadDistance)
				continue;

			// point correct
			// get angle and target zone
			float angle = glm::atan(point.x, point.y);
			int zoneIndex = findTargetZoneIndex(angle);
			zones[zoneIndex].points.push_back(point);
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
