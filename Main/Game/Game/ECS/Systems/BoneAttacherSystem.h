#pragma once

#include "../SystemUpdate.h"

// requires BoneAttacher
// requires SkinnedMeshRenderer in parent
class BoneAttacherSystem : public SystemUpdate
{
public:
	void Update(float dt) override;
};
