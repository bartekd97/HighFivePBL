#pragma once

#include "../SystemUpdate.h"
#include "Utility/TaskPool.h"

// requires SkinnedMeshRenderer
// requires SkinAnimator
class SkinAnimatorSystem : public SystemUpdate
{
private:
	TaskPool<1> skinAimatorWorker;
	concurrency::concurrent_queue<GameObject> workerQueue;

	void WorkQueue(float dt);

public:
	void Update(float dt) override;
};
