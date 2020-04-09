#include "ScriptStartSystem.h"
#include "HFEngine.h"
#include "../Components/ScriptContainer.h"
#include "../../Event/EventManager.h"
#include "../../Event/Events.h"

void ScriptStartSystem::Init()
{
	EventManager::AddListener(METHOD_LISTENER(Events::GameObject::Script::ADDED, ScriptStartSystem::OnScriptAdd));
	EventManager::AddListener(METHOD_LISTENER(Events::GameObject::DESTROYED, ScriptStartSystem::OnGameObjectDestroyed));
}

void ScriptStartSystem::Update(float dt)
{
	if (gameObjects.size() > 0)
	{
		for (auto const& gameObject : gameObjects)
		{
			auto& scriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(gameObject);
			auto scripts = scriptContainer.GetInstances();
			for (auto it = scriptsToStart[gameObject].begin(); it != scriptsToStart[gameObject].end(); it++)
			{
				scripts->at(*it)->Start();
			}
			scriptsToStart[gameObject].clear();
		}
		gameObjects.clear();
	}
}

void ScriptStartSystem::OnScriptAdd(Event& ev)
{
	auto gameObject = ev.GetParam<GameObject>(Events::GameObject::GameObject);
	auto index = ev.GetParam<unsigned int>(Events::GameObject::Script::Index);
	if (gameObjects.find(gameObject) == gameObjects.end())
	{
		gameObjects.insert(gameObject);
	}
	scriptsToStart[gameObject].push_back(index);
}

void ScriptStartSystem::OnGameObjectDestroyed(Event& ev)
{
	auto gameObject = ev.GetParam<GameObject>(Events::GameObject::GameObject);
	if (gameObjects.find(gameObject) != gameObjects.end())
	{
		scriptsToStart[gameObject].clear();
		gameObjects.erase(gameObject);
	}
}
