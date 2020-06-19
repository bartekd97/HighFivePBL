#include "ScriptUpdateSystem.h"
#include "HFEngine.h"
#include "../Components/ScriptContainer.h"

void ScriptUpdateSystem::Update(float dt)
{
	auto copy = gameObjects;
	for (auto const& gameObject : copy)
	{
		if (!HFEngine::ECS.SearchComponent<ScriptContainer>(gameObject))
			continue;
		auto& scriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(gameObject);
		auto scripts = scriptContainer.GetInstances();
		if (scripts == nullptr) continue;
		for (auto it = scripts->begin(); it != scripts->end(); it++)
		{
			(*it)->Update(dt);
		}
	}
}
