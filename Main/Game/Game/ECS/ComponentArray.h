#pragma once

#include <array>
#include <cassert>
#include <unordered_map>
#include <span>

#include "ECSTypes.h"

class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void GameObjectDestroyed(GameObject gameObject) = 0;
};


template<typename T>
class ComponentArray : public IComponentArray
{
public:
	void InsertData(GameObject gameObject, T component)
	{
		assert(gameObjectToIndexMap.find(gameObject) == gameObjectToIndexMap.end() && "Component added to same gameObject more than once.");

		size_t newIndex = size;
		gameObjectToIndexMap[gameObject] = newIndex;
		indexToGameObjectMap[newIndex] = gameObject;
		componentArray[newIndex] = component;
		++size;
	}

	void RemoveData(GameObject gameObject)
	{
		assert(gameObjectToIndexMap.find(gameObject) != gameObjectToIndexMap.end() && "Removing non-existent component.");

		size_t indexOfRemovedGameObject = gameObjectToIndexMap[gameObject];
		size_t indexOfLastElement = size - 1;
		componentArray[indexOfRemovedGameObject] = componentArray[indexOfLastElement];

		GameObject gameObjectOfLastElement = indexToGameObjectMap[indexOfLastElement];
		gameObjectToIndexMap[gameObjectOfLastElement] = indexOfRemovedGameObject;
		indexToGameObjectMap[indexOfRemovedGameObject] = gameObjectOfLastElement;

		gameObjectToIndexMap.erase(gameObject);
		indexToGameObjectMap.erase(indexOfLastElement);

		--size;
	}

	T& GetData(GameObject gameObject)
	{
		assert(gameObjectToIndexMap.find(gameObject) != gameObjectToIndexMap.end() && "Retrieving non-existent component.");

		return componentArray[gameObjectToIndexMap[gameObject]];
	}

	std::span<T> GetAll()
	{
		return std::span<T>(componentArray.data(), size);
	}

	bool SearchData(GameObject gameObject)
	{
		return gameObjectToIndexMap.find(gameObject) != gameObjectToIndexMap.end();
	}

	void GameObjectDestroyed(GameObject gameObject) override
	{
		if (gameObjectToIndexMap.find(gameObject) != gameObjectToIndexMap.end())
		{
			RemoveData(gameObject);
		}
	}

private:
	std::array<T, MAX_GAMEOBJECTS> componentArray{};
	std::unordered_map<GameObject, size_t> gameObjectToIndexMap{};
	std::unordered_map<size_t, GameObject> indexToGameObjectMap{};
	size_t size{};
};
