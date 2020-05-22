#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

#include "ScriptManager.h"
#include "../Event/EventManager.h"
#include "../Event/Events.h"
#include "Scripts/Scripts.h"
#include "../Utility/Utility.h"

#define REGISTER_SCRIPT(name) RegisterScriptProvider(#name, []() { return std::make_shared<name>(); })

namespace ScriptManager
{
	using ScriptId = std::uint32_t;

	std::unordered_map<GameObject, std::vector<std::shared_ptr<Script>>> instances;
	std::unordered_map<ScriptId, std::function<std::shared_ptr<Script>()>> scriptProviders;

	void RegisterScriptProvider(std::string scriptName, std::function<std::shared_ptr<Script>()> provider)
	{
		ScriptId id = Utility::HashString(scriptName.c_str(), scriptName.length());
		if (scriptProviders.contains(id))
		{
			LogWarning("ScriptManager::RegisterScriptProvider(): Loader for '{}' already registered.", scriptName);
			return;
		}
		scriptProviders[id] = provider;
	}

	// TODO: remove somehow event listeners that were placed by scripts
	// maybe wrapper function in script (so handles can be stored)?
	void OnGameObjectDestroyed(Event& ev)
	{
		GameObject gameObject = ev.GetParam<GameObject>(Events::GameObject::GameObject);
		auto it = instances.find(gameObject);
		if (it != instances.end())
		{
			it->second.clear();
		}
	}

	void Initialize()
	{
		REGISTER_SCRIPT(TestScript);
		REGISTER_SCRIPT(TestScript2);
		REGISTER_SCRIPT(PlayerController);
		REGISTER_SCRIPT(PlayerCameraFollower);
		REGISTER_SCRIPT(GhostController);
		REGISTER_SCRIPT(MapCellOptimizer);
		REGISTER_SCRIPT(TriggerTest);
		REGISTER_SCRIPT(GUIStatistics);

		EventManager::AddListener(Events::GameObject::DESTROYED, OnGameObjectDestroyed);
	}

	std::shared_ptr<Script> InstantiateScript(GameObject gameObject, std::string scriptName)
	{
		auto provider = scriptProviders.find(Utility::HashString(scriptName.c_str(), scriptName.length()));
		assert(provider != scriptProviders.end() && "Tried to instantiate non-existing script");
		std::shared_ptr<Script> instance = provider->second();
		instance->SetGameObject(gameObject);
		instances[gameObject].push_back(instance);
		instance->Awake();

		Event ev(Events::GameObject::Script::ADDED);
		ev.SetParam<GameObject>(Events::GameObject::GameObject, gameObject);
		ev.SetParam<unsigned int>(Events::GameObject::Script::Index, instances[gameObject].size() - 1);
		EventManager::FireEvent(ev);

		return instance;
	}

	std::vector<std::shared_ptr<Script>>* GetScripts(GameObject gameObject)
	{
		auto it = instances.find(gameObject);
		if (it != instances.end())
		{
			return &it->second;
		}
		return nullptr;
	}
}
