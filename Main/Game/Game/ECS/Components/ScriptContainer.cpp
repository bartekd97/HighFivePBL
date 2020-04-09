#include "ScriptContainer.h"
#include "../../Scripting/ScriptManager.h"

void ScriptContainer::AddScript(GameObject gameObject, std::string name)
{
	auto instance = ScriptManager::InstantiateScript(gameObject, name);
	instance->Awake();
	instances = ScriptManager::GetScripts(gameObject);
}

std::vector<std::shared_ptr<Script>>* ScriptContainer::GetInstances()
{
	return instances;
}
