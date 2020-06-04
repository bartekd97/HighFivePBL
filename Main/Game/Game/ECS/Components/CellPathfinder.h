#pragma once

#include <deque>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

#define PathReadyMethodPointer(method) std::bind(&method, this, std::placeholders::_1)

class PathfindingGrid;

struct CellPathfindingTask
{
	enum class Status
	{
		NONE,
		QUEUED,
		PROCESSED,
		COMPLETED
	};

	Status status;
	glm::vec3 startWorldPos;
	glm::vec3 endWorldPos;
	glm::vec3 cellPos; // cell world position
	std::shared_ptr<PathfindingGrid> grid;
	std::deque<glm::vec3> path;
};

class CellPathfinder
{
	friend class CellPathfinderSystem;

public:
	std::function<void(const std::deque<glm::vec3>&)> onPathReady;

private:
	std::shared_ptr<CellPathfindingTask> task;
	bool needQueue = false;
	bool needCallback = false;
	glm::vec3 targetPosition;

public:
	inline CellPathfindingTask::Status GetPathStatus()
	{
		return task == nullptr ? CellPathfindingTask::Status::NONE : task->status;
	}

	inline bool IsPathReady()
	{
		return GetPathStatus() == CellPathfindingTask::Status::COMPLETED;
	}

	inline const std::deque<glm::vec3>& GetPath()
	{
		static const std::deque<glm::vec3> _empty;
		return IsPathReady() ? task->path : _empty;
	}

	inline const glm::vec3 GetCurrentTargetPosition()
	{
		return targetPosition;
	}

	inline bool CanQueue()
	{
		return task == nullptr || task->status == CellPathfindingTask::Status::COMPLETED;
	}

	inline void QueuePath(glm::vec3& targetWorldPosition)
	{
		needQueue = true;
		targetPosition = targetWorldPosition;
	}

	inline void ClearPath()
	{
		task = nullptr;
	}
};