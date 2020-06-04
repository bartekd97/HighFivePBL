#pragma once

#include "../System.h"
#include "Utility/TaskPool.h"

// requires ParticleContainer
// requires ParticleEmitter
// requires ParticleRenderer
class ParticleEmitterSystem : public System, public ISystemUpdate, public ISystemPostUpdate
{
private:
	TaskPool<1, ITaskPool::Priority::BELOW_NORMAL> particleEmitterWorker; // only 1 thread for particle system is supported DON'T CHANGE IT
	concurrency::concurrent_queue<GameObject> workerUpdateQueue;
	concurrency::concurrent_queue<GameObject> workerEmitQueue;
	bool doEmit = false;

	void WorkUpdateQueue(float dt);
	void WorkEmitQueue(float dt);

public:
	void Update(float dt) override;
	void PostUpdate(float dt) override;
};
