#pragma once

#include <memory>

#include "ComponentManager.h"
#include "GameObjectManager.h"
#include "SystemManager.h"
#include "GameObjectHierarchy.h"
#include "ECSTypes.h"

extern GameObjectHierarchy gameObjectHierarchy;

class ECSCore
{
public:
	void Init();

	GameObject CreateGameObject(std::string name = "GameObject");

	GameObject CreateGameObject(GameObject parent, std::string name = "GameObject");

	void SetEnabledGameObject(GameObject gameObject, bool enabled);

	bool IsEnabledGameObject(GameObject gameObject);

	const char* GetNameGameObject(GameObject gameObject);

	void SetNameGameObject(GameObject gameObject, std::string name);

	std::optional<GameObject> GetGameObjectByName(std::string name);

	void DestroyGameObject(GameObject gameObject);

	void UpdateSystems(float dt);

	inline int GetLivingGameObjectsCount()
	{
		return gameObjectManager->GetLivingGameObjectCount();
	}

	template<typename T>
	void RegisterComponent()
	{
		componentManager->RegisterComponent<T>();
	}

	template<typename T>
	void AddComponent(GameObject gameObject, T component)
	{
		componentManager->AddComponent<T>(gameObject, component);

		auto signature = gameObjectManager->GetSignature(gameObject);
		signature.set(componentManager->GetComponentType<T>(), true);
		gameObjectManager->SetSignature(gameObject, signature);

		systemManager->GameObjectSignatureChanged(gameObject, signature);
	}

	template<typename T>
	void RemoveComponent(GameObject gameObject)
	{
		componentManager->RemoveComponent<T>(gameObject);

		auto signature = gameObjectManager->GetSignature(gameObject);
		signature.set(componentManager->GetComponentType<T>(), false);
		gameObjectManager->SetSignature(gameObject, signature);

		systemManager->GameObjectSignatureChanged(gameObject, signature);
	}

	template<typename T>
	T& GetComponent(GameObject gameObject)
	{
		return componentManager->GetComponent<T>(gameObject);
	}

	template<typename T>
	bool SearchComponent(GameObject gameObject)
	{
		return componentManager->SearchComponent<T>(gameObject);
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		return componentManager->GetComponentType<T>();
	}

	template<typename T>
	std::shared_ptr<T> RegisterSystem(bool autonomous = false)
	{
		return systemManager->RegisterSystem<T>(autonomous);
	}

	template<typename T>
	void SetSystemSignature(Signature signature)
	{
		systemManager->SetSignature<T>(signature);
	}

private:
	std::unique_ptr<ComponentManager> componentManager;
	std::unique_ptr<GameObjectManager> gameObjectManager;
	std::unique_ptr<SystemManager> systemManager;
};
