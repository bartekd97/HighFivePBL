#pragma once

#include <unordered_set>
#include "../System.h"
#include "Event/Events.h"


class NextFrameDestroySystem : public System, public ISystemUpdate
{
private:
	std::unordered_set<GameObject> requestedToNextFrameDestroy;

	void OnNextFrameDestroyRequested(Event& event);
public:
	void Init() override;
	void Update(float dt) override;
};