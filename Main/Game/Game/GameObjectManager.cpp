#include "GameObjectManager.h"

GameObjectManager::GameObjectManager()
{
	for (GameObject gameObject = 0; gameObject < MAX_GAMEOBJECTS; ++gameObject)
	{
		availableGameObjects.push(gameObject);
	}
}

GameObject GameObjectManager::CreateGameObject()
{
	assert(livingGameObjectCount < MAX_GAMEOBJECTS && "Too many game objects in existence.");

	GameObject id = availableGameObjects.front();
	availableGameObjects.pop();
	++livingGameObjectCount;

	return id;
}

void GameObjectManager::DestroyGameObject(GameObject gameObject)
{
	assert(gameObject < MAX_GAMEOBJECTS && "GameObject out of range.");

	signatures[gameObject].reset();
	availableGameObjects.push(gameObject);
	--livingGameObjectCount;
}

void GameObjectManager::SetSignature(GameObject gameObject, Signature signature)
{
	assert(gameObject < MAX_GAMEOBJECTS && "GameObject out of range.");

	signatures[gameObject] = signature;
}

Signature GameObjectManager::GetSignature(GameObject gameObject)
{
	assert(gameObject < MAX_GAMEOBJECTS && "GameObject out of range.");

	return signatures[gameObject];
}
