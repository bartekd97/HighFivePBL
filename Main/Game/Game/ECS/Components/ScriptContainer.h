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

	template<typename T>
	std::shared_ptr<T> GetScript()
	{
		if (instances)
		{
			for (auto it = instances->begin(); it != instances->end(); it++)
			{
				if (typeid(T) == typeid(**it))
				{
					return std::static_pointer_cast<T>(*it);
				}
			}
		}
		return nullptr;
	}

private:
	std::vector<std::shared_ptr<Script>>* instances;
};
