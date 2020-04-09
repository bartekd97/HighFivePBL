#pragma once

#include <vector>
#include <memory>

#include "../../Scripting/ScriptManager.h"
#include "ECS/ECSTypes.h"

class Script;

class ScriptContainer
{
public:
	void AddScript(GameObject gameObject, std::string name)
	{
		ScriptManager::InstantiateScript(gameObject, name);
		instances = ScriptManager::GetScripts(gameObject);
	}

	inline std::vector<std::shared_ptr<Script>>* GetInstances()
	{
		return instances;
	}

private:
	std::vector<std::shared_ptr<Script>>* instances;
};
