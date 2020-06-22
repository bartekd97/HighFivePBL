#include "GameLite.h"
#include "HFEngine.h"
#include "ECS/Components/ScriptContainer.h"
#include "MapGenerator/MapGenerator.h"
#include "Audio/AudioManager.h"
#include "../SceneUtilities.h"


void GameLiteScene::OnLoad()
{
	//
	// SPAWN THINGS
	//
	AudioManager::StopBackground();
	AudioManager::PlayBackground("gameplayKorpecki", 0.1f);
	MapGenerator generator;
	generator._debugLiteMode = true;
	generator.Generate();
	GameObject startupCell = generator.GetStartupCell();
	glm::vec3 startupPos = HFEngine::ECS.GetComponent<Transform>(startupCell).GetWorldPosition();
	auto playerPrefab = PrefabManager::GetPrefab("Player");
	auto player = playerPrefab->Instantiate(startupPos);

	auto testGuiObject = HFEngine::ECS.CreateGameObject("TestGUI");
	HFEngine::ECS.AddComponent<ScriptContainer>(testGuiObject, {});
	auto& tgScriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(testGuiObject);
	tgScriptContainer.AddScript(testGuiObject, "GUIStatistics");

	//
	// SETUP ENVIRO
	//
	HFEngine::WorldLight.direction = { 0.5f, -1.0f, 0.75f };
	HFEngine::WorldLight.color = { 0.04f, 0.06f, 0.1f };
	HFEngine::WorldLight.ambient = { 0.005f, 0.01f, 0.02f };
	HFEngine::WorldLight.shadowIntensity = 1.0f;

	HFEngine::WorldLight.shadowmapMaxDistanceSteps = 50.0f;
	HFEngine::WorldLight.shadowmapScale = 1.25f;

	float camScale;
	//camScale = 15.0f;
	camScale = SceneUtilities::CalculateGameplayCameraSizeFromWindowSize();
	HFEngine::MainCamera.SetScale(camScale);
}

void GameLiteScene::OnUnload()
{
	HFEngine::ClearGameObjects();
}
