#pragma once

#include "../System.h"
#include "Utility/TaskPool.h"

// requires ParticleContainer
// requires ParticleEmitter
class ParticleEmitterSystem : public System, public ISystemUpdate, public ISystemPostUpdate
{
private:
	TaskPool<1> particleEmitterWorker;
	concurrency::concurrent_queue<GameObject> workerUpdateQueue;
	concurrency::concurrent_queue<GameObject> workerEmitQueue;

public:
	void Update(float dt) override;
	void PostUpdate(float dt) override;
};
