#include "GameLite.h"
#include "HFEngine.h"
#include "ECS/Components/ScriptContainer.h"
#include "MapGenerator/MapGenerator.h"


void GameLiteScene::OnLoad()
{
	//
	// SPAWN THINGS
	//

	MapGenerator generator;
	generator._debugLiteMode = true;
	generator.Generate();
	GameObject startupCell = generator.GetStartupCell();
	glm::vec3 startupPos = HFEngine::ECS.GetComponent<Transform>(startupCell).GetWorldPosition();
	auto playerPrefab = PrefabManager::GetPrefab("Player");
	auto player = playerPrefab->Instantiate(startupPos);

	auto prefabCircle = PrefabManager::GetPrefab("TestCircle");
	auto testCircleObject = prefabCircle->Instantiate(startupPos - glm::vec3(10.0f, 0.0f, 10.0f));
	HFEngine::ECS.SetNameGameObject(testCircleObject, "testCircle");

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

	HFEngine::MainCamera.SetScale(15.0f); // orig was 12.6f
}

void GameLiteScene::OnUnload()
{
}
