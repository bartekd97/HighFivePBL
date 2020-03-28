#include "ComponentArray.h"

/*template<typename T>
void ComponentArray<T>::InsertData(GameObject gameObject, T component)
{
	assert(gameObjectToIndexMap.find(gameObject) == gameObjectToIndexMap.end() && "Component added to same gameObject more than once.");

	// Put new entry at end
	size_t newIndex = size;
	gameObjectToIndexMap[gameObject] = newIndex;
	indexToGameObjectMap[newIndex] = gameObject;
	componentArray[newIndex] = component;
	++size;
}

template<typename T>
void ComponentArray<T>::RemoveData(GameObject gameObject)
{
	assert(gameObjectToIndexMap.find(gameObject) != gameObjectToIndexMap.end() && "Removing non-existent component.");

	// Copy element at end into deleted element's place to maintain density
	size_t indexOfRemovedGameObject = gameObjectToIndexMap[gameObject];
	size_t indexOfLastElement = size - 1;
	componentArray[indexOfRemovedGameObject] = componentArray[indexOfLastElement];

	// Update map to point to moved spot
	GameObject gameObjectOfLastElement = indexToGameObjectMap[indexOfLastElement];
	gameObjectToIndexMap[gameObjectOfLastElement] = indexOfRemovedGameObject;
	indexToGameObjectMap[indexOfRemovedGameObject] = gameObjectOfLastElement;

	gameObjectToIndexMap.erase(gameObject);
	indexToGameObjectMap.erase(indexOfLastElement);

	--size;
}

template<typename T>
T& ComponentArray<T>::GetData(GameObject gameObject)
{
	assert(gameObjectToIndexMap.find(gameObject) != gameObjectToIndexMap.end() && "Retrieving non-existent component.");

	return componentArray[gameObjectToIndexMap[gameObject]];
}

template<typename T>
void ComponentArray<T>::GameObjectDestroyed(GameObject gameObject)
{
	if (gameObjectToIndexMap.find(gameObject) != gameObjectToIndexMap.end())
	{
		RemoveData(gameObject);
	}
}
*/