#pragma once

#include "../Utility/Logger.h"
#include "Script.h"

namespace ScriptManager
{
	void Initialize();

	std::shared_ptr<Script> InstantiateScript(GameObject gameObject, std::string scriptName);
};
