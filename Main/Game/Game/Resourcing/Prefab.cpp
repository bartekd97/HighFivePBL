#include "Prefab.h"
#include "HFEngine.h"
#include "Scripting/ScriptManager.h"

#define SetupGameObject(object, position, rotation, scale) HFEngine::ECS.GetComponent<Transform>(object).SetPositionRotationScale(position, rotation, scale)

namespace {
	inline void SetupComponents(GameObject object, Prefab::PrefabComponents& components)
	{
		for (auto c : components)
			c->Create(object);
	}
	void MakeChildren(GameObject parent, std::vector<Prefab::PrefabChild>& children)
	{
		for (auto c : children)
		{
			GameObject object = HFEngine::ECS.CreateGameObject(parent, c.name);
			SetupGameObject(object, c.position, c.rotation, c.scale);
			SetupComponents(object, c.components);
			MakeChildren(object, c.children);
		}
	}

	inline void WarmComponentLoaders(Prefab::PrefabComponents& components)
	{
		for (auto c : components)
			c->Warm();
	}
	void WarmChildrenLoaders(std::vector<Prefab::PrefabChild>& children)
	{
		for (auto c : children)
		{
			WarmComponentLoaders(c.components);
			WarmChildrenLoaders(c.children);
		}
	}
}

GameObject Prefab::Instantiate(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	ScriptManager::SuspendAwake();
	GameObject object = HFEngine::ECS.CreateGameObject(name);
	scale *= defaultScale;
	SetupGameObject(object, position, rotation, scale);
	SetupComponents(object, components);
	MakeChildren(object, children);
	ScriptManager::ResumeAwake();
	return object;
}

GameObject Prefab::Instantiate(GameObject parent, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	ScriptManager::SuspendAwake();
	GameObject object = HFEngine::ECS.CreateGameObject(parent, name);
	scale *= defaultScale;
	SetupGameObject(object, position, rotation, scale);
	SetupComponents(object, components);
	MakeChildren(object, children);
	ScriptManager::ResumeAwake();
	return object;
}

void Prefab::MakeWarm()
{
	if (isWarm) return;

	WarmComponentLoaders(components);
	WarmChildrenLoaders(children);

	isWarm = true;
}
