#pragma once

#include <vector>
#include <memory>

#include "../../Scripting/ScriptManager.h"
#include "ECS/ECSTypes.h"

class Script;

class ScriptContainer
{
public:
	std::shared_ptr<Script> AddScript(GameObject gameObject, std::string name)
	{
		auto ptr = ScriptManager::InstantiateScript(gameObject, name);
		instances = ScriptManager::GetScripts(gameObject);
		return ptr;
	}

	inline std::vector<std::shared_ptr<Script>>* GetInstances()
	{
		return instances;
	}

private:
	std::vector<std::shared_ptr<Script>>* instances;
};
