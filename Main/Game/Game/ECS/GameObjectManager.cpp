#include "GameObjectManager.h"
#include "GameObjectHierarchy.h"
#include "../Event/EventManager.h"
#include "../Event/Events.h"

GameObjectManager::GameObjectManager()
{
	for (GameObject gameObject = 0; gameObject < MAX_GAMEOBJECTS; ++gameObject)
	{
		availableGameObjects.push(gameObject);
	}
}

GameObject GameObjectManager::CreateGameObject(std::string name)
{
	assert(livingGameObjectCount < MAX_GAMEOBJECTS && "Too many game objects in existence.");

	GameObject id = availableGameObjects.front();
	availableGameObjects.pop();
	++livingGameObjectCount;
	SetName(id, name);

	return id;
}

void GameObjectManager::DestroyGameObject(GameObject gameObject)
{
	assert(gameObject < MAX_GAMEOBJECTS && "GameObject out of range.");

	signatures[gameObject].reset();
	disabledSignatures[gameObject].reset();
	availableGameObjects.push(gameObject);
	--livingGameObjectCount;

	Event ev(Events::GameObject::DESTROYED);
	ev.SetParam<GameObject>(Events::GameObject::GameObject, gameObject);
	EventManager::FireEvent(ev);
}

Signature GameObjectManager::SetEnabled(GameObject gameObject, bool enabled)
{
	bool isEnabled = disabledSignatures[gameObject].count() == 0;
	if (enabled)
	{
		if (!isEnabled)
		{
			signatures[gameObject] = disabledSignatures[gameObject];
			disabledSignatures[gameObject].reset();
		}
	}
	else
	{
		if (isEnabled)
		{
			disabledSignatures[gameObject] = signatures[gameObject];
			signatures[gameObject].reset();
		}
	}
	return signatures[gameObject];
}

bool GameObjectManager::IsEnabled(GameObject gameObject)
{
	return disabledSignatures[gameObject].count() == 0;
}

const char* GameObjectManager::GetName(GameObject gameObject)
{
	return names[gameObject];
}

void GameObjectManager::SetName(GameObject gameObject, std::string name)
{
	assert((name.length() < MAX_GAMEOBJECT_NAME_LENGTH) && "Setted name of game object is longer than its limit.");

	strcpy_s(names[gameObject], name.c_str());
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

int GameObjectManager::GetLivingGameObjectCount()
{
	return livingGameObjectCount;
}

std::optional<GameObject> GameObjectManager::GetGameObjectByName(std::string name)
{
	for (GameObject i = 0; i < MAX_GAMEOBJECTS; i++)
	{
		if (signatures[i].any() || disabledSignatures[i].any())
		{
			if (name.compare(names[i]) == 0)
			{
				return i;
			}
		}
	}

	return std::nullopt;
}

tsl::robin_set<GameObject> GameObjectManager::GetGameObjectsByName(std::string name)
{
	tsl::robin_set<GameObject> result;

	for (GameObject i = 0; i < MAX_GAMEOBJECTS; i++)
	{
		if (signatures[i].any() || disabledSignatures[i].any())
		{
			if (name.compare(names[i]) == 0)
			{
				result.insert(i);
			}
		}
	}

	return result;
}
