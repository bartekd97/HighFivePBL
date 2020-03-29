#pragma once

#include <memory>

#include "ComponentManager.h"
#include "GameObjectManager.h"
#include "SystemManager.h"
#include "ECSTypes.h"

class ECSCore
{
public:
	void Init()
	{
		componentManager = std::make_unique<ComponentManager>();
		gameObjectManager = std::make_unique<GameObjectManager>();
		systemManager = std::make_unique<SystemManager>();
	}

	GameObject CreateGameObject()
	{
		return gameObjectManager->CreateGameObject();
	}

	void DestroyGameObject(GameObject gameObject)
	{
		gameObjectManager->DestroyGameObject(gameObject);
		componentManager->GameObjectDestroyed(gameObject);
		systemManager->GameObjectDestroyed(gameObject);
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

	void UpdateSystems(float dt)
	{
		for (auto it = systemManager->updateQueue.begin(); it != systemManager->updateQueue.end(); ++it)
		{
			(*it)->Update(dt);
		}
	}

	void RenderSystems()
	{
		for (auto it = systemManager->renderQueue.begin(); it != systemManager->renderQueue.end(); ++it)
		{
			(*it)->Render();
		}
	}

private:
	std::unique_ptr<ComponentManager> componentManager;
	std::unique_ptr<GameObjectManager> gameObjectManager;
	std::unique_ptr<SystemManager> systemManager;
};
