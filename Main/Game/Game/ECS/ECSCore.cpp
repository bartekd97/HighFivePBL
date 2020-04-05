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
	AddComponent<Transform>(
		created,
		{}
	);
	return created;
}

GameObject ECSCore::CreateGameObject(GameObject parent, std::string name)
{
	GameObject created = CreateGameObject(name);
	gameObjectHierarchy.AddGameObject(created, parent);
	return created;
}

void ECSCore::DestroyGameObject(GameObject gameObject)
{
	gameObjectManager->DestroyGameObject(gameObject);
	componentManager->GameObjectDestroyed(gameObject);
	systemManager->GameObjectDestroyed(gameObject);

	auto children = gameObjectHierarchy.GetChildren(gameObject);
	gameObjectHierarchy.RemoveGameObject(gameObject);
	for (auto it = children.begin(); it != children.end(); it++)
	{
		DestroyGameObject(*it);
	}
}

void ECSCore::SetEnabledGameObject(GameObject gameObject, bool enabled)
{
	auto signature = gameObjectManager->SetEnabled(gameObject, enabled);
	systemManager->GameObjectSignatureChanged(gameObject, signature);
	auto children = gameObjectHierarchy.GetChildren(gameObject);
	for (auto it = children.begin(); it != children.end(); it++)
	{
		signature = gameObjectManager->SetEnabled(*it, enabled);
		systemManager->GameObjectSignatureChanged(*it, signature);
	}
}

bool ECSCore::IsEnabledGameObject(GameObject gameObject)
{
	return gameObjectManager->IsEnabled(gameObject);
}

const char* ECSCore::GetNameGameObject(GameObject gameObject)
{
	return gameObjectManager->GetName(gameObject);
}

void ECSCore::SetNameGameObject(GameObject gameObject, std::string name)
{
	gameObjectManager->SetName(gameObject, name);
}

void ECSCore::UpdateSystems(float dt)
{
	for (auto it = systemManager->updateQueue.begin(); it != systemManager->updateQueue.end(); ++it)
	{
		(*it)->Update(dt);
	}
}
