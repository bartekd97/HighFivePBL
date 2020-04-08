#include "ScriptUpdateSystem.h"
#include "HFEngine.h"

void ScriptUpdateSystem::Update(float dt)
{
	for (auto const& gameObject : gameObjects)
	{
		auto& scriptComponent = HFEngine::ECS.GetComponent<ScriptComponent>(gameObject);
		(scriptComponent.GetInstance())->Update(dt);
	}
}