#pragma once

#include <memory>
#include <vector>
#include <string>
#include "../Utility/Logger.h"
#include "Script.h"

namespace ScriptManager
{
	void Initialize();

	std::shared_ptr<Script> InstantiateScript(GameObject gameObject, std::string scriptName);
	std::vector<std::shared_ptr<Script>>* GetScripts(GameObject gameObject);

	void SuspendAwake();
	void ResumeAwake();
};
