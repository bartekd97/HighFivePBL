#include "Prefab.h"
#include "HFEngine.h"

#define SetupGameObject(object, position, rotation, scale) Transform& __trans = HFEngine::ECS.GetComponent<Transform>(object); \
	__trans.SetPosition(position); \
	__trans.SetRotation(rotation); \
	__trans.SetScale(scale);

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
}

GameObject Prefab::Instantiate(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	GameObject object = HFEngine::ECS.CreateGameObject(name);
	scale *= defaultScale;
	SetupGameObject(object, position, rotation, scale);
	SetupComponents(object, components);
	MakeChildren(object, children);
	return object;
}

GameObject Prefab::Instantiate(GameObject parent, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	GameObject object = HFEngine::ECS.CreateGameObject(parent, name);
	scale *= defaultScale;
	SetupGameObject(object, position, rotation, scale);
	SetupComponents(object, components);
	MakeChildren(object, children);
	return object;
}