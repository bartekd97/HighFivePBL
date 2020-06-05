#pragma once

#include "../System.h"
#include "Utility/TaskPool.h"

// requires SkinnedMeshRenderer
// requires SkinAnimator
class SkinAnimatorSystem : public System, public ISystemUpdate
{
private:
	TaskPool<1, ITaskPool::Priority::ABOVE_NORMAL> skinAimatorWorker;
	concurrency::concurrent_queue<GameObject> workerQueue;

	void WorkQueue(float dt);

public:
	void Update(float dt) override;
};
