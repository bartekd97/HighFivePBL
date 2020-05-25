#pragma once

#include "../SystemUpdate.h"

// requires ParticleContainer
// requires ParticleEmitter
class ParticleEmitterSystem : public SystemUpdate
{
public:
	void Update(float dt) override;
};
