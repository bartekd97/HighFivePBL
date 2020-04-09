#pragma once

#include "../ECS/ECSTypes.h"

class Script
{
public:
	virtual void Awake() {};
	virtual void Update(float dt) {};

	inline void SetGameObject(GameObject gameObject)
	{
		this->gameObject = gameObject;
	}

protected:
	Script() {}

	inline GameObject GetGameObject()
	{
		return gameObject;
	}
	
private:
	GameObject gameObject;

	Script(Script const&) = delete;
	void operator=(Script const&) = delete;
};
