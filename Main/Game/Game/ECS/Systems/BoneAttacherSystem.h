#pragma once

#include "../System.h"

// requires BoneAttacher
// requires SkinnedMeshRenderer in parent
class BoneAttacherSystem : public System, public ISystemPostUpdate
{
public:
	void PostUpdate(float dt) override;
};
