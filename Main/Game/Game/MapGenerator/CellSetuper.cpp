#include <algorithm>
#include <unordered_set>
#include <glm/gtx/rotate_vector.hpp>
#include "CellSetuper.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/BoxCollider.h"
#include "ECS/Components/CircleCollider.h"
#include "ECS/Components/Transform.h"
#include "HFEngine.h"
#include "Physics/Physics.h"
#include "Resourcing/Prefab.h"
#include "Physics/Raycaster.h"
#include "Physics/RaycastHit.h"
#include "Utility/Utility.h"
#include "Utility/Logger.h"
#include "Rendering/PrimitiveRenderer.h"
#include "Utility/Pathfinding.h"


void CellSetuper::Setup()
{
	MapCell& cellInfo = HFEngine::ECS.GetComponent<MapCell>(cell);

	structureContainer = HFEngine::ECS.CreateGameObject(cell, "Structures");
	obstacleContainer = HFEngine::ECS.CreateGameObject(cell, "Obstacles");
	enemiesContainer = HFEngine::ECS.CreateGameObject("Enemies"); // keep it in global space
	cellInfo.EnemyContainer = enemiesContainer;

	if (type == MapCell::Type::NORMAL)
	{
		// spawn main statue only on normal cell
		cellInfo.Statue = setupConfig.mainStatuePrefab->Instantiate(cell, { 0,0,0 }, { 0,25,0 });

		MakeZones();

		PrepareColliders();
		UpdateColliders();

		rotations.push_back(20.0f);
		rotations.push_back(340.0f);

		glm::quat boxRot;
		float width;
		float height;
		BoxCollider box;

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
			if (_debugLiteMode) objectsToGenerate = 1;

			for (int i = 0; i < objectsToGenerate; ++i)
			{
				if (zone.points.size() == largestZoneSize || zone.ind % 2 == 1 )
				{
					// skip structures in lite mode
					if (_debugLiteMode) continue;
					// it can be later improved to spawn only specific structure in lite mode

					int spawnTries = 10;
					while (spawnTries > 0)
					{
						std::shared_ptr<Prefab> structurePrefab;
						if (zone.points.size() == largestZoneSize)
						{
							if (spawnTries > 5)
							{
								structurePrefab = setupConfig.largePrefabs.at(
									(zone.points.size() * (i + 1) + spawnTries) % setupConfig.largePrefabs.size()
								);
							}
							else if (spawnTries > 3)
							{
								structurePrefab = setupConfig.mediumPrefabs.at(
									(zone.points.size() * (i + 1) + spawnTries) % setupConfig.mediumPrefabs.size()
								);
							}
							else
							{
								structurePrefab = setupConfig.smallPrefabs.at(
									(zone.points.size() * (i + 1) + spawnTries) % setupConfig.smallPrefabs.size()
								);
							}
						}
						else
						{
							if (spawnTries > 5)
							{
								structurePrefab = setupConfig.mediumPrefabs.at(
									(zone.points.size() * (i + 1) + spawnTries) % setupConfig.mediumPrefabs.size()
								);
							}
							else
							{
								structurePrefab = setupConfig.smallPrefabs.at(
									(zone.points.size() * (i + 1) + spawnTries) % setupConfig.smallPrefabs.size()
								);
							}
						}
						
						int selectedRotation = ((int)zone.center.x * (int)zone.center.y * spawnTries * (i + 1)) % rotations.size();
						float structureRotation = rotations[selectedRotation];// zone.center.x* zone.center.y;
						//structureRotation = (float)((int)zone.center.x * (int)zone.center.y  * spawnTries * (i + 1) % 100 - 50);
						boxRot = glm::quat(glm::vec3(0.0f, glm::radians(structureRotation), 0.0f));
						structurePrefab->Properties().GetFloat("width", width, 1.0f);
						structurePrefab->Properties().GetFloat("height", height, 1.0f);
						//structurePrefab->Properties().GetFloat("roty", structureRotation, 1.0f);

						box.SetWidthHeight(width, height);
						if (zone.points.size() > 0)
						{
							glm::vec2 position = DrawPointInZone(zone, box, boxRot, i* spawnTries);
							if (glm::length2(position) > 0.001f)
							{
								LogInfo("CellSetuper::Setup() Zone {} got spawned structure at: {}, {}", zone.ind, position.x, position.y);
								SpawnStructure(structurePrefab, position, structureRotation);
								spawnTries = 0;
								UpdateColliders();
							}
						}
						else
						{
							spawnTries = 0;
						}
						spawnTries -= 1;
					}
				}
				else
				{
					// skip obstacles in lite mode
					//if (_debugLiteMode) continue;
					// it can be later improved to spawn only specific obstacle in lite mode

					int spawnTries = 10;
					while (spawnTries > 0)
					{
						std::shared_ptr<Prefab> obstaclePrefab;
						obstaclePrefab = setupConfig.obstaclePrefabs.at(
						(zone.points.size() * (i + 1) + spawnTries) % setupConfig.obstaclePrefabs.size()
							);

						//if (_debugLiteMode) obstaclePrefab = setupConfig.obstaclePrefabs[1];

						float obstacleRotation = zone.center.x * zone.center.y;
						boxRot = glm::quat(glm::vec3(0.0f, glm::radians(obstacleRotation), 0.0f));
						obstaclePrefab->Properties().GetFloat("width", width, 1.0f);
						obstaclePrefab->Properties().GetFloat("height", height, 1.0f);
						box.SetWidthHeight(width, height);
						if (zone.points.size() > 0)
						{
							glm::vec2 position = DrawPointInZone(zone, box, boxRot, i);
							if (glm::length2(position) > 0.001f)
							{
								LogInfo("CellSetuper::Setup() Zone {} got spawned obstacle at: {}, {}", zone.ind, position.x, position.y);
								SpawnObstacle(obstaclePrefab, position, obstacleRotation);
								spawnTries = 0;
							}
						}
						else
						{
							spawnTries = 0;
						}
						spawnTries -= 1;
					}
					
				}
			}

			
		}

	}
	// create tutorial assets on startup cell (only in regular start mode)
	else if (type == MapCell::Type::STARTUP && !_debugLiteMode)
	{
		GameObject gateObject = cellInfo.Bridges[0].Gate;
		glm::vec3 gateWorldPosition = HFEngine::ECS.GetComponent<Transform>(gateObject).GetWorldPosition();
		glm::vec3 gatePosition = HFEngine::ECS.GetComponent<Transform>(gateObject).GetPosition();
		
		glm::vec3 roadFront = glm::normalize(gatePosition);
		glm::vec3 roadSide = glm::rotateY(roadFront, M_PI * 0.5f);

		GameObject tutorialContainer = HFEngine::ECS.CreateGameObject(cell, "TutorialAssets");

		setupConfig.cellTutorialConfig.WASD->Instantiate(tutorialContainer, roadSide * -6.0f);
		setupConfig.cellTutorialConfig.SpaceKey->Instantiate(tutorialContainer, roadFront * -6.6f);
		setupConfig.cellTutorialConfig.LMBKey->Instantiate(tutorialContainer, roadSide * 6.0f);

		float roadRotation = glm::atan(roadFront.x, roadFront.z);
		GameObject ghostPlayground = setupConfig.cellTutorialConfig.GhostPlayground->Instantiate(
			tutorialContainer,
			gatePosition - roadFront * 5.0f,
			{0.0f, glm::degrees(roadRotation) + 180.0f, 0.0f}
		);
		HFEngine::ECS.AddComponent<CellChild>(ghostPlayground, { cell });

		// spawn enemy toy in world space
		GameObject enemyToy = setupConfig.cellTutorialConfig.EnemyToy->Instantiate(
			gateWorldPosition - roadFront * 5.0f,
			{ 0.0f, glm::degrees(roadRotation), 0.0f }
			);
		HFEngine::ECS.AddComponent<CellChild>(enemyToy, { cell });
	}
	// create fence fires on boss cell
	else if (type == MapCell::Type::BOSS)
	{
		int bossNumber = int(glm::length2(HFEngine::ECS.GetComponent<Transform>(cell).GetPosition())) % 2;

		if (bossNumber == 0) // necromancer
		{
			CreateFenceFires(setupConfig.cellFenceFireConfig.necromancerFire);
		}
		else if (bossNumber == 1) // ragnaros
		{
			CreateFenceFires(setupConfig.cellFenceFireConfig.ragnarosFire);
		}
	}

	// clear temp colliders
	ClearTempColliders();
	UpdateColliders();

	// create pathfinding grid
	MakePathfindingGrid();



	// now spawn enemies
	if (type == MapCell::Type::NORMAL)
	{
		int zonesSum = 0;
		for (auto& zone : zones) zonesSum += zone.points.size();

		auto enemyPrefab = setupConfig.enemyPrefabs.at(zonesSum % setupConfig.enemyPrefabs.size());

		// choose specific enemy for lite mode
		if (_debugLiteMode) enemyPrefab = setupConfig.enemyPrefabs[1]; // 0 axer, 1 flyer

		CircleCollider enemyRadiusCollider;
		enemyPrefab->Properties().GetFloat("radius", enemyRadiusCollider.radius, 1.0f);

		std::vector<Zone*> pZones;
		for (auto& zone : zones) pZones.push_back(&zone);

		int enemiesCount = (int)glm::round(setupConfig.enemiesCountFactor * float(zonesSum) * 0.01f);

		//if (_debugLiteMode) enemiesCount = 0;

		while (enemiesCount > 0 && pZones.size() > 0)
		{
			for (auto pZone : pZones)
			{
				glm::vec2 position = DrawPointInZone(*pZone, enemyRadiusCollider, enemiesCount);
				if (glm::length2(position) > 0.001f)
				{
					float rotation = glm::length2(position) * pZone->points.size() * (enemiesCount + 1);
					LogInfo("CellSetuper::Setup() Zone {} got spawned enemy at: {}, {}", pZone->ind, position.x, position.y);
					SpawnEnemy(enemyPrefab, position, rotation);
					UpdateColliders();
					enemiesCount--;
					if (enemiesCount == 0) break;
				}
				else
				{
					pZones.erase(std::remove(pZones.begin(), pZones.end(), pZone), pZones.end());
				}
			}
		}
	}

	ClearTempObstacleColliders();

}


// only obstacles should have "random" rotation around Y axis
// structures should have predefined, constant rotation
// because some models have been made just to be visible from one side(s)
void CellSetuper::SpawnStructure(std::shared_ptr<Prefab> prefab, glm::vec2 localPos, float rotation)
{
	prefab->Instantiate(structureContainer, { localPos.x, 0.0f, localPos.y }, { 0.0f, rotation, 0.0f });
}
void CellSetuper::SpawnObstacle(std::shared_ptr<Prefab> prefab, glm::vec2 localPos, float rotation)
{
	static float width, height;
	Collider col;
	col.type = Collider::ColliderTypes::STATIC;
	col.shape = Collider::ColliderShapes::BOX;
	BoxCollider boxCol;
	prefab->Properties().GetFloat("width", width);
	prefab->Properties().GetFloat("height", height);

	GameObject obstacle = prefab->Instantiate(obstacleContainer, { localPos.x, 0.0f, localPos.y }, {0.0f, rotation, 0.0f});

	boxCol.SetWidthHeight(width, height);

	GameObject tmpCollider = HFEngine::ECS.CreateGameObject(obstacle);
	HFEngine::ECS.AddComponent<Collider>(tmpCollider, col);
	HFEngine::ECS.AddComponent<BoxCollider>(tmpCollider, boxCol);
	tempObstacleColliders.push_back(tmpCollider);
}

void CellSetuper::SpawnEnemy(std::shared_ptr<Prefab> prefab, glm::vec2 localPos, float rotation)
{
	glm::vec2 cellPos = {
		HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition().x,
		HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition().z
	};
	GameObject enemy = prefab->Instantiate(enemiesContainer,
		{ cellPos.x + localPos.x, 0.0f, cellPos.y + localPos.y },
		{ 0.0f, rotation, 0.0f }
	);
	HFEngine::ECS.AddComponent<CellChild>(enemy, { cell });
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

	largestZoneSize = 0;
	// calc zones center
	for (auto& zone : zones)
	{
		if (zone.points.size() == 0)
			continue;

		glm::vec2 sum = glm::vec2(0.0f);
		for (auto p : zone.points)
		{
			sum += p;
#ifdef HF_DEBUG_RENDER
			//PrimitiveRenderer::DrawStickyPoint({ cellPos.x + p.x, 0.0f, cellPos.y + p.y });
#endif // HF_DEBUG_RENDER
		}
		zone.center = sum / float(zone.points.size());

		if(zone.points.size() > largestZoneSize)
			largestZoneSize = zone.points.size();
	}


#ifdef HF_DEBUG_RENDER
	std::vector<int> _sizes;
	for (auto& zone : zones)
		_sizes.push_back(zone.points.size());
	LogInfo("CellSetuper::MakeZones() Cell '{}' zone sizes: {}", cellInfo.CellSiteIndex, _sizes);
#endif
}




void CellSetuper::PrepareColliders()
{
	Collider col;
	col.type = Collider::ColliderTypes::STATIC;
	
	// create tmp circle collider in the middle
	GameObject circleTmpCollider = HFEngine::ECS.CreateGameObject(cell);
	col.shape = Collider::ColliderShapes::CIRCLE;
	HFEngine::ECS.AddComponent<Collider>(circleTmpCollider, col);
	HFEngine::ECS.AddComponent<CircleCollider>(circleTmpCollider, { setupConfig.gridInnerRadius });
	tempColliders.push_back(circleTmpCollider);

	// create tmp road colliders;
	MapCell& cellInfo = HFEngine::ECS.GetComponent<MapCell>(cell);
	glm::vec2 cellPos = {
		HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition().x,
		HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition().z
	};
	col.shape = Collider::ColliderShapes::BOX;
	for (auto bridge : cellInfo.Bridges)
	{
		glm::vec3 gatePosition = 
			HFEngine::ECS.GetComponent<Transform>(bridge.Gate).GetWorldPosition() -
			HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition();

		glm::vec3 roadPosition = gatePosition / 2.0f;
		float angle = glm::degrees(glm::atan(gatePosition.x, gatePosition.z));
		float width = glm::length(roadPosition) * 2.0f;
		
		BoxCollider bc;
		bc.SetWidthHeight(width, setupConfig.gridMinRoadDistance);
		GameObject roadTmpCollider = HFEngine::ECS.CreateGameObject(cell);
		HFEngine::ECS.GetComponent<Transform>(roadTmpCollider).SetPositionRotation(
			roadPosition, {0.0f, angle + 90.0f, 0.0f}
			);
		HFEngine::ECS.AddComponent<Collider>(roadTmpCollider, col);
		HFEngine::ECS.AddComponent<BoxCollider>(roadTmpCollider, bc);
		tempColliders.push_back(roadTmpCollider);
	}

}

void CellSetuper::UpdateColliders()
{
	// THIS BELOW DOESNT WORK FOR SOME REASON
	auto allCellColliders = HFEngine::ECS.GetGameObjectsWithComponentInChildren<Collider>(cell, true);
	Physics::ProcessGameObjects(allCellColliders, true);
	// true to disable others and do calculations only for cell colliders;
}

void CellSetuper::ClearTempColliders()
{
	for (auto c : tempColliders)
		HFEngine::ECS.DestroyGameObject(c);

	tempColliders.clear();
}

void CellSetuper::ClearTempObstacleColliders()
{
	for (auto c : tempObstacleColliders)
		HFEngine::ECS.DestroyGameObject(c);

	tempObstacleColliders.clear();
}

void CellSetuper::CreateFenceFires(std::shared_ptr<Prefab> firePrefab)
{
	float offset = setupConfig.cellFenceFireConfig.fireFenceOffset;

	GameObject fenceFireContainer = HFEngine::ECS.CreateGameObject(cell, "FenceFire");
	GameObject fenceContainer = HFEngine::ECS.GetByNameInChildren(cell, "Fence")[0];
	auto highFences = HFEngine::ECS.GetByNameInChildren(fenceContainer, "HighFence");

	std::unordered_set<GameObject> gateFences;
	MapCell& cellInfo = HFEngine::ECS.GetComponent<MapCell>(cell);
	for (auto bridge : cellInfo.Bridges)
	{
		using GD = std::pair<GameObject, float>; // fence and it's distance to gate
		std::vector<GD> thisGateFences;
		glm::vec3 gatePosition = HFEngine::ECS.GetComponent<Transform>(bridge.Gate).GetPosition();
		for (auto fenceObject : highFences)
		{
			glm::vec3 fencePosition = HFEngine::ECS.GetComponent<Transform>(fenceObject).GetPosition();
			float dist = glm::distance2(gatePosition, fencePosition);
			thisGateFences.emplace_back(GD{fenceObject, dist});
		}
		std::sort(thisGateFences.begin(), thisGateFences.end(), [](GD& a, GD& b) {
			return a.second < b.second;
			});

		gateFences.insert(thisGateFences[0].first);
		gateFences.insert(thisGateFences[1].first);
	}


	for (auto fenceObject : highFences)
	{
		if (gateFences.contains(fenceObject))
			continue;

		glm::vec3 fencePosition = HFEngine::ECS.GetComponent<Transform>(fenceObject).GetPosition();
		glm::vec3 fenceFront = HFEngine::ECS.GetComponent<Transform>(fenceObject).GetFront();
		glm::vec3 fenceRotation = HFEngine::ECS.GetComponent<Transform>(fenceObject).GetRotationEuler();

		glm::vec3 firePosition = fencePosition - fenceFront * offset;
		firePrefab->Instantiate(fenceFireContainer, firePosition, fenceRotation);
	}
}


void CellSetuper::MakePathfindingGrid()
{
	auto grid = PathfindingGrid::Create(setupConfig.gridSize, setupConfig.gridSize);

	MapCell& cellInfo = HFEngine::ECS.GetComponent<MapCell>(cell);
	glm::vec2 cellPos = {
		HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition().x,
		HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition().z
	};

	CircleCollider nodeCollider;
	nodeCollider.radius = 0.7f;
	RaycastHit out;

	PathfindingGrid::PathNode node;
	for (int i = 0; i < setupConfig.gridSize; i ++)
	{
		for (int j = 0; j < setupConfig.gridSize; j ++)
		{
			node.position = { i - (setupConfig.gridSize / 2), j - (setupConfig.gridSize / 2) };
			node.index = { i, j };
			node.isAvailable = true;
			node.gCost = 2147483647;
			node.CalculateFCost();
			node.cameFromNode = NULL;
			// check availability
			{
				// check if it isn too far or too near
				float level = cellInfo.PolygonSmoothInner.GetEdgeCenterRatio(node.position);
				if (level > 0.9f) {
					node.isAvailable = false;
				}
				else {
					// now check raycast
					glm::vec3 wpos = { cellPos.x + node.position.x, 0.0f, cellPos.y + node.position.y };
					if (Physics::Raycast(wpos, nodeCollider, out))
						node.isAvailable = false;
				}
			}

			// and now set it
			grid->SetNode(i, j, node);
		}
	}

	// assign grid to cell
	cellInfo.PathFindingGrid = grid;

#ifdef HF_DEBUG_RENDER
	for (int i = 0; i < setupConfig.gridSize; i++)
	{
		for (int j = 0; j < setupConfig.gridSize; j++)
		{
			const auto& node = grid->GetNode(i, j);
			if (node.isAvailable)
				PrimitiveRenderer::DrawStickyPoint({ cellPos.x + node.position.x, 0.0f, cellPos.y + node.position.y });
		}
	}
#endif
}


glm::vec2 CellSetuper::DrawPointInZone(Zone& zone, const BoxCollider& boxCollider, glm::quat& rotation, int number)
{
	glm::vec3 pos;
	glm::vec2 cellPos = {
		HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition().x,
		HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition().z
	};

	int iter_available = glm::min(100, (int)zone.points.size());
	int someSeed = ((int)glm::abs(zone.center.x * zone.center.y) + zone.points.size())* zones.size() + (int)(glm::length2(zone.center) * number);
	//int randomNumber = ((int)zone.center.x * (int)zone.center.y * zones.size() * number * 7 +1) % zone.points.size();
	int randomNumber = (someSeed + number) % zone.points.size();

	tsl::robin_set<int> usedNumbers;
	RaycastHit out;
	do
	{
		//randomNumber = (randomNumber * (int)zone.center.x * (int)zone.center.y * zones.size() * number * 7 + 1) % zone.points.size();
		randomNumber = (randomNumber * (int)glm::length2(zone.center) + number) % zone.points.size();
		while (usedNumbers.contains(randomNumber))
			randomNumber = (randomNumber + 1) % zone.points.size();
		usedNumbers.insert(randomNumber);
		
		pos = glm::vec3(
			zone.points[randomNumber].x + cellPos.x,
			0.0f,
			zone.points[randomNumber].y + cellPos.y
		);
		iter_available--;
	} while (iter_available > 0 && Physics::Raycast(pos, rotation, boxCollider, out) == true);


	if (iter_available > 0)
	{
		return zone.points[randomNumber];
	}
	else
	{
		return glm::vec2(0.0f);
	}
}


glm::vec2 CellSetuper::DrawPointInZone(Zone& zone, const CircleCollider& circleCollider, int number)
{
	glm::vec3 pos;
	glm::vec2 cellPos = {
		HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition().x,
		HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition().z
	};

	int iter_available = glm::min(20, (int)zone.points.size());
	int someSeed = ((int)glm::abs(zone.center.x * zone.center.y) + zone.points.size()) * zones.size() + (int)(glm::length2(zone.center) * number);
	int randomNumber = (someSeed + number) % zone.points.size();

	tsl::robin_set<int> usedNumbers;
	RaycastHit out;
	do
	{
		randomNumber = (randomNumber * (int)glm::length2(zone.center) + number) % zone.points.size();
		while (usedNumbers.contains(randomNumber))
			randomNumber = (randomNumber + 1) % zone.points.size();
		usedNumbers.insert(randomNumber);

		pos = glm::vec3(
			zone.points[randomNumber].x + cellPos.x,
			0.0f,
			zone.points[randomNumber].y + cellPos.y
			);
		iter_available--;
	} while (iter_available > 0 && Physics::Raycast(pos, circleCollider, out) == true);


	if (iter_available > 0)
	{
		return zone.points[randomNumber];
	}
	else
	{
		return glm::vec2(0.0f);
	}
}