#pragma once

#include "../System.h"
#include "Utility/TaskPool.h"

// requires SkinnedMeshRenderer
// requires SkinAnimator
class SkinAnimatorSystem : public System, public ISystemUpdate
{
private:
	TaskPool<1> skinAimatorWorker;
	concurrency::concurrent_queue<GameObject> workerQueue;

	void WorkQueue(float dt);

public:
	void Update(float dt) override;
};
