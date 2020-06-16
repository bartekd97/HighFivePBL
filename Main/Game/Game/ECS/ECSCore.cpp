#include <stack>

#include "ECSCore.h"
#include "Components/Transform.h"

GameObjectHierarchy gameObjectHierarchy;

void ECSCore::Init()
{
	componentManager = std::make_unique<ComponentManager>();
	gameObjectManager = std::make_unique<GameObjectManager>();
	systemManager = std::make_unique<SystemManager>();
}

GameObject ECSCore::CreateGameObject(std::string name)
{
	auto created = gameObjectManager->CreateGameObject(name);
	AddComponent<Transform>(created, Transform(created));
	return created;
}

GameObject ECSCore::CreateGameObject(GameObject parent, std::string name)
{
	auto created = gameObjectManager->CreateGameObject(name);
	gameObjectHierarchy.AddGameObject(created, parent);
	AddComponent<Transform>(created, Transform(created));
	return created;
}

void ECSCore::DestroyGameObject(GameObject gameObject)
{
	gameObjectManager->DestroyGameObject(gameObject);
	componentManager->GameObjectDestroyed(gameObject);
	systemManager->GameObjectDestroyed(gameObject);

	auto children = gameObjectHierarchy.GetChildren(gameObject);
	for (auto it = children.begin(); it != children.end(); it++)
	{
		DestroyGameObject(*it);
	}
	gameObjectHierarchy.RemoveGameObject(gameObject);
}

void ECSCore::SetEnabledGameObject(GameObject gameObject, bool enabled)
{
	auto parent = gameObjectHierarchy.GetParent(gameObject);
	if (parent.has_value() && enabled && !gameObjectManager->IsEnabled(parent.value())) // TODO: children too?
	{
		return;
	}

	std::vector<GameObject> all;
	all.push_back(gameObject);
	int i = 0, max = 1;

	while (i < max)
	{
		std::vector<GameObject> children = gameObjectHierarchy.GetChildren(all[i]);
		for (auto child : children) all.push_back(child);
		max += children.size();
		i++;
	}

	for (auto& object : all)
	{
		auto signature = gameObjectManager->SetEnabled(object, enabled);
		systemManager->GameObjectSignatureChanged(object, signature);
	}
}

void ECSCore::ClearGameObjects()
{
	for (GameObject i = 0; i < MAX_GAMEOBJECTS; i++)
	{
		if (gameObjectManager->IsValid(i)) DestroyGameObject(i);
	}
}

bool ECSCore::IsEnabledGameObject(GameObject gameObject)
{
	return gameObjectManager->IsEnabled(gameObject);
}

bool ECSCore::IsValidGameObject(GameObject gameObject)
{
	return gameObjectManager->IsValid(gameObject);
}

const char* ECSCore::GetNameGameObject(GameObject gameObject)
{
	return gameObjectManager->GetName(gameObject);
}

void ECSCore::SetNameGameObject(GameObject gameObject, std::string name)
{
	gameObjectManager->SetName(gameObject, name);
}

std::optional<GameObject> ECSCore::GetGameObjectByName(std::string name)
{
	return gameObjectManager->GetGameObjectByName(name);
}

tsl::robin_set<GameObject> ECSCore::GetGameObjectsByName(std::string name)
{
	return gameObjectManager->GetGameObjectsByName(name);
}

std::vector<GameObject> ECSCore::GetByNameInChildren(GameObject parent, std::string name)
{
	return gameObjectHierarchy.GetByNameInChildren(parent, name);
}

std::shared_ptr<System> ECSCore::GetSystemByTypeName(const char* typeName)
{
	return systemManager->GetSystemByTypeName(typeName);
}

void ECSCore::UpdateSystems(float dt)
{
	for (auto it = systemManager->updateQueue.begin(); it != systemManager->updateQueue.end(); ++it)
	{
		(*it)->Update(dt);
	}
}
void ECSCore::PostUpdateSystems(float dt)
{
	for (auto it = systemManager->postUpdateQueue.begin(); it != systemManager->postUpdateQueue.end(); ++it)
	{
		(*it)->PostUpdate(dt);
	}
}
