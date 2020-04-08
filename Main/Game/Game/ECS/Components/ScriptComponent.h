#pragma once

//#include "../../Scripting/Script.h"

class Script;

class ScriptComponent
{
	friend class ComponentManager;
public:
	std::string name;

	inline std::shared_ptr<Script> GetInstance()
	{
		return instance;
	}

protected:
	std::shared_ptr<Script> instance;

private:
	inline void SetInstance(std::shared_ptr<Script> instance)
	{
		this->instance = instance;
	}
};
