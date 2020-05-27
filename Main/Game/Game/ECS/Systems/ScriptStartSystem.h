#pragma once

#include "../System.h"
#include "../../Event/Event.h"

class ScriptStartSystem : public System, public ISystemUpdate
{
public:
	void Init() override;
	void Update(float dt) override;
private:
	void OnScriptAdd(Event& ev);
	void OnGameObjectDestroyed(Event& ev);
	std::unordered_map<GameObject, std::vector<unsigned int>> scriptsToStart;
};
