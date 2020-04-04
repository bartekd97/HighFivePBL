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

	GameObject CreateGameObject();

	GameObject CreateGameObject(GameObject parent);

	void DestroyGameObject(GameObject gameObject);

	void UpdateSystems(float dt);

	void RenderSystems();

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
	ComponentType GetComponentType()
	{
		return componentManager->GetComponentType<T>();
	}

	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		return systemManager->RegisterSystem<T>();
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
