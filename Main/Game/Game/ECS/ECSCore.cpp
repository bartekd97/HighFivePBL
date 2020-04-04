#include "ECSCore.h"

GameObjectHierarchy gameObjectHierarchy;

void ECSCore::Init()
{
	componentManager = std::make_unique<ComponentManager>();
	gameObjectManager = std::make_unique<GameObjectManager>();
	systemManager = std::make_unique<SystemManager>();
}

GameObject ECSCore::CreateGameObject()
{
	return gameObjectManager->CreateGameObject();
}

GameObject ECSCore::CreateGameObject(GameObject parent)
{
	GameObject created = gameObjectManager->CreateGameObject();
	gameObjectHierarchy.AddGameObject(created, parent);
	return created;
}

void ECSCore::DestroyGameObject(GameObject gameObject)
{
	gameObjectManager->DestroyGameObject(gameObject);
	componentManager->GameObjectDestroyed(gameObject);
	systemManager->GameObjectDestroyed(gameObject);
	gameObjectHierarchy.RemoveGameObject(gameObject);
}

void ECSCore::UpdateSystems(float dt)
{
	for (auto it = systemManager->updateQueue.begin(); it != systemManager->updateQueue.end(); ++it)
	{
		(*it)->Update(dt);
	}
}

void ECSCore::RenderSystems()
{
	for (auto it = systemManager->renderQueue.begin(); it != systemManager->renderQueue.end(); ++it)
	{
		(*it)->Render();
	}
}
