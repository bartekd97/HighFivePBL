#pragma once

#include <cassert>
#include <memory>
#include <unordered_map>

#include "System.h"
#include "ECSTypes.h"

class SystemManager
{
public:
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		const char* typeName = typeid(T).name();

		assert(systems.find(typeName) == systems.end() && "Registering system more than once.");

		auto system = std::make_shared<T>();
		systems.insert({ typeName, system });
		systemsQueue.push_back(system);
		return system;
	}

	template<typename T>
	void SetSignature(Signature signature)
	{
		const char* typeName = typeid(T).name();

		assert(systems.find(typeName) != systems.end() && "System used before registered.");

		signatures.insert({ typeName, signature });
	}

	void GameObjectDestroyed(GameObject gameObject)
	{
		for (auto const& pair : systems)
		{
			auto const& system = pair.second;
			system->gameObjects.erase(gameObject);
		}
	}

	void GameObjectSignatureChanged(GameObject gameObject, Signature gameObjectSignature)
	{
		for (auto const& pair : systems)
		{
			auto const& type = pair.first;
			auto const& system = pair.second;
			auto const& systemSignature = signatures[type];

			if ((gameObjectSignature & systemSignature) == systemSignature)
			{
				system->gameObjects.insert(gameObject);
			}
			else
			{
				system->gameObjects.erase(gameObject);
			}
		}
	}

	std::vector<std::shared_ptr<System>> systemsQueue;
private:
	std::unordered_map<const char*, Signature> signatures{};
	std::unordered_map<const char*, std::shared_ptr<System>> systems{};
};
