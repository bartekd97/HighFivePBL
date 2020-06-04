#include <assert.h>
#include <unordered_map>
#include "SceneManager.h"
#include "IScene.h"
#include "Utility/Logger.h"
#include "Event/Events.h"
#include "HFEngine.h"


// variables
namespace SceneManager
{
	std::unordered_map<std::string, std::shared_ptr<IScene>> scenes;
	std::shared_ptr<IScene> currentScene;
	std::shared_ptr<IScene> requestedScene;

	bool Initialized = false;
}

// privates
namespace SceneManager
{
	void FrameStart(Event& event)
	{
		if (requestedScene == nullptr) return;

		if (currentScene)
			currentScene->Unload();

		for (GameObject gameObject = 0; gameObject < MAX_GAMEOBJECTS; ++gameObject)
			if (HFEngine::ECS.IsValidGameObject(gameObject))
				HFEngine::ECS.DestroyGameObject(gameObject);

		currentScene = requestedScene;
		currentScene->Load();
		requestedScene = nullptr;
	}
}


// publics
void SceneManager::Initialize()
{
	if (Initialized)
	{
		LogWarning("SceneManager::Initialize(): Already initialized");
		return;
	}

	EventManager::AddListener(FUNCTION_LISTENER(Events::General::FRAME_START, SceneManager::FrameStart));

	Initialized = true;
	LogInfo("SceneManager initialized.");
}

void SceneManager::RegisterScene(std::string name, std::shared_ptr<IScene> scene)
{
	assert(!scenes.contains(name) && "Scene already registered");
	scenes[name] = scene;

	LogInfo("SceneManager::RegisterScene(): Registered '{}' scene.", name);
}

void SceneManager::RequestLoadScene(std::string name)
{
	assert(scenes.contains(name) && "Scene doesn't exists");

	if (currentScene == scenes[name])
	{
		LogWarning("SceneManager::RequestLoadScene(): Trying to request '{}' scene, which is already current scene.", name);
		return;
	}

	if (requestedScene != nullptr)
	{
		LogWarning("SceneManager::RequestLoadScene(): Other scene already has been requested");
		return;
	}

	requestedScene = scenes[name];
	LogInfo("SceneManager::RequestLoadScene(): Requested '{}' scene to load in next frame.", name);
}
