#pragma once

#include "../SystemUpdate.h"
#include "../../Event/Event.h"

class ScriptStartSystem : public SystemUpdate
{
public:
	void Init() override;
	void Update(float dt) override;
private:
	void OnScriptAdd(Event& ev);
	void OnGameObjectDestroyed(Event& ev);
	std::unordered_map<GameObject, std::vector<unsigned int>> scriptsToStart;
};
