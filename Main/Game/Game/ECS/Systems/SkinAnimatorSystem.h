#pragma once

#include "../SystemUpdate.h"

// requires SkinnedMeshRenderer
// requires SkinAnimator
class SkinAnimatorSystem : public SystemUpdate
{
public:
	void Update(float dt) override;
};
