#pragma once

#include "../SystemPostUpdate.h"

// requires BoneAttacher
// requires SkinnedMeshRenderer in parent
class BoneAttacherSystem : public SystemPostUpdate
{
public:
	void PostUpdate(float dt) override;
};
