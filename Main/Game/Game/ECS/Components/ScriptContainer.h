#pragma once

#include <vector>
#include <memory>

#include "ECS/ECSTypes.h"

class Script;

class ScriptContainer
{
public:
	void AddScript(GameObject gameObject, std::string name);

	std::vector<std::shared_ptr<Script>>* GetInstances();

private:
	std::vector<std::shared_ptr<Script>>* instances;
};
