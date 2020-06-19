#pragma once

#include "../System.h"
#include "../../Event/EventManager.h"
#include "../../Event/Events.h"

class TriggerColliderCollectorSystem : public System
{
public:
	void Init() override;
private:
	void OnGameObjectDestroyed(Event& e);
};
