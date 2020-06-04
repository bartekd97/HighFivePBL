#include "CellPathfinderSystem.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "ECS/Components/Transform.h"
#include "HFEngine.h"
#include "Utility/Pathfinding.h"

void CellPathfinderSystem::WorkPathfinding(std::shared_ptr<CellPathfindingTask> task)
{
	if (task == nullptr) return;

	task->status = CellPathfindingTask::Status::PROCESSED;
	task->path = task->grid->FindPath(
		task->startWorldPos,
		task->endWorldPos,
		task->cellPos
		);
	task->status = CellPathfindingTask::Status::COMPLETED;
}

void CellPathfinderSystem::Update(float dt)
{
	auto it = gameObjects.begin();
	while (it != gameObjects.end())
	{
		auto gameObject = *(it++);

		auto& pathfinder = HFEngine::ECS.GetComponent<CellPathfinder>(gameObject);

		if (pathfinder.needCallback && pathfinder.onPathReady && pathfinder.IsPathReady())
		{
			pathfinder.onPathReady(pathfinder.GetPath());
			pathfinder.needCallback = false;
		}

		if (!pathfinder.needQueue || !pathfinder.CanQueue())
			continue;

		auto cell = HFEngine::ECS.GetComponent<CellChild>(gameObject).cell;

		auto task = std::make_shared<CellPathfindingTask>();
		task->grid = HFEngine::ECS.GetComponent<MapCell>(cell).PathFindingGrid;
		task->startWorldPos = HFEngine::ECS.GetComponent<Transform>(gameObject).GetWorldPosition();
		task->endWorldPos = pathfinder.targetPosition;
		task->cellPos = HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition();
		task->status = CellPathfindingTask::Status::QUEUED;

		pathfinder.needQueue = false;
		pathfinder.needCallback = true;
		pathfinder.task = task;

		pathfindingWorker.AddTask(std::bind(&CellPathfinderSystem::WorkPathfinding, this, task));
	}
}
