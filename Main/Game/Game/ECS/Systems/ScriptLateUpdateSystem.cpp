#include "ScriptLateUpdateSystem.h"
#include "HFEngine.h"
#include "../Components/ScriptContainer.h"

void ScriptLateUpdateSystem::Update(float dt)
{
	for (auto const& gameObject : gameObjects)
	{
		auto& scriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(gameObject);
		auto scripts = scriptContainer.GetInstances();
		if (scripts == nullptr) continue;
		for (auto it = scripts->begin(); it != scripts->end(); it++)
		{
			(*it)->LateUpdate(dt);
		}
	}
}
