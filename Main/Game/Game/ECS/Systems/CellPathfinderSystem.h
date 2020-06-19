#pragma once

#include "../System.h"
#include "ECS/Components/CellPathfinder.h"
#include "Utility/TaskPool.h"

// requires CellChild
// requires CellPathfinder
class CellPathfinderSystem : public System, public ISystemUpdate
{
private:
	TaskPool<1, ITaskPool::Priority::NORMAL> pathfindingWorker;

	void WorkPathfinding(std::shared_ptr<CellPathfindingTask> task);
public:
	void Update(float dt) override;
};
