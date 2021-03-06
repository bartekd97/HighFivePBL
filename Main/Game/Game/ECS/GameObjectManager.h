#pragma once

#include <array>
#include <cassert>
#include <queue>
#include <optional>
#include <tsl/robin_set.h>

#include "ECSTypes.h"

class GameObjectManager
{
public:
	GameObjectManager();

	GameObject CreateGameObject(std::string name);

	void DestroyGameObject(GameObject gameObject);

	Signature SetEnabled(GameObject gameObject, bool enabled);

	bool IsEnabled(GameObject gameObject);

	bool IsValid(GameObject gameObject);

	void SetSignature(GameObject gameObject, Signature signature);

	const char* GetName(GameObject gameObject);

	void SetName(GameObject gameObject, std::string name);

	std::optional<GameObject> GetGameObjectByName(std::string name);

	tsl::robin_set<GameObject> GetGameObjectsByName(std::string name);

	Signature GetSignature(GameObject gameObject);

	int GetLivingGameObjectCount();

private:
	std::queue<GameObject> availableGameObjects{};
	std::array<Signature, MAX_GAMEOBJECTS> signatures{};
	std::array<Signature, MAX_GAMEOBJECTS> disabledSignatures{};
	std::array<char[MAX_GAMEOBJECT_NAME_LENGTH], MAX_GAMEOBJECTS> names{};
	uint32_t livingGameObjectCount{};
};
