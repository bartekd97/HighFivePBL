#pragma once

#include <any>
#include <memory>
#include <unordered_map>

#include "ComponentArray.h"
#include "ECSTypes.h"
#include "Components/ScriptComponent.h"
#include "../Scripting/ScriptManager.h"

class ComponentManager
{
public:
	template<typename T>
	void RegisterComponent()
	{
		const char* typeName = typeid(T).name();

		assert(componentTypes.find(typeName) == componentTypes.end() && "Registering component type more than once.");

		componentTypes.insert({ typeName, nextComponentType });
		componentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

		++nextComponentType;
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		const char* typeName = typeid(T).name();

		assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered before use.");

		return componentTypes[typeName];
	}

	template<typename T>
	void AddComponent(GameObject gameObject, T component)
	{
		GetComponentArray<T>()->InsertData(gameObject, component);
	}

	template<>
	void AddComponent(GameObject gameObject, ScriptComponent component)
	{
		component.SetInstance(ScriptManager::InstantiateScript(gameObject, component.name));
		GetComponentArray<ScriptComponent>()->InsertData(gameObject, component);
		(component.GetInstance())->Start();
	}

	template<typename T>
	void RemoveComponent(GameObject gameObject)
	{
		GetComponentArray<T>()->RemoveData(gameObject);
	}

	template<typename T>
	T& GetComponent(GameObject gameObject)
	{
		return GetComponentArray<T>()->GetData(gameObject);
	}

	void GameObjectDestroyed(GameObject gameObject)
	{
		for (auto const& pair : componentArrays)
		{
			auto const& component = pair.second;

			component->GameObjectDestroyed(gameObject);
		}
	}
	

private:
	std::unordered_map<const char*, ComponentType> componentTypes{};
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> componentArrays{};
	ComponentType nextComponentType{};

	template<typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray()
	{
		const char* typeName = typeid(T).name();

		assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered before use.");

		return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeName]);
	}
};
