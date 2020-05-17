#pragma once

#include "../ECS/ECSTypes.h"

class Script
{
public:

	virtual void Awake() {};
	virtual void Start() {};
	virtual void Update(float dt) {};
	virtual void LateUpdate(float dt) {};

	inline void SetGameObject(GameObject gameObject)
	{
		this->gameObject = gameObject;
	}

	bool SetFloat(std::string name, float value)
	{
		if (floatParameters.find(name) == floatParameters.end()) return false;
		*floatParameters[name] = value;
		unsettedParams -= 1;
		return  true;
	}

	bool SetVec3(std::string name, glm::vec3 value)
	{
		if (vec3Parameters.find(name) == vec3Parameters.end()) return false;
		*vec3Parameters[name] = value;
		unsettedParams -= 1;
		return  true;
	}

	bool SetString(std::string name, std::string value)
	{
		if (stringParameters.find(name) == stringParameters.end()) return false;
		*stringParameters[name] = value;
		unsettedParams -= 1;
		return  true;
	}

	inline int GetUnsettedParams()
	{
		return unsettedParams;
	}

protected:
	Script()
	{
		unsettedParams = 0;
		gameObject = NULL_GAMEOBJECT;
	}

	void RegisterFloatParameter(std::string name, float* floatPtr)
	{
		floatParameters[name] = floatPtr;
		unsettedParams += 1;
	}

	void RegisterVec3Parameter(std::string name, glm::vec3* vec3Ptr)
	{
		vec3Parameters[name] = vec3Ptr;
		unsettedParams += 1;
	}

	void RegisterStringParameter(std::string name, std::string* stringPtr)
	{
		stringParameters[name] = stringPtr;
		unsettedParams += 1;
	}

	inline GameObject GetGameObject()
	{
		return gameObject;
	}
	
private:
	GameObject gameObject;
	std::unordered_map<std::string, float*> floatParameters;
	std::unordered_map<std::string, glm::vec3*> vec3Parameters;
	std::unordered_map<std::string, std::string*> stringParameters;
	int unsettedParams;

	Script(Script const&) = delete;
	void operator=(Script const&) = delete;
};
