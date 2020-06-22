#pragma once

#include <glm/glm.hpp>

#include "../System.h"
#include "../Components.h"

class GarbageCollectorSystem : public System, public ISystemPostUpdate
{
public:
	void Init() override;
	void PostUpdate(float dt) override;
private:
	FrameCounter lastFrame;
	int howManyFramesBetween;
};
