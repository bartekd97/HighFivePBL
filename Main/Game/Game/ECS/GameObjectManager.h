#pragma once

#include <array>
#include <cassert>
#include <queue>

#include "ECSTypes.h"

class GameObjectManager
{
public:
	GameObjectManager();

	GameObject CreateGameObject(std::string name);

	void DestroyGameObject(GameObject gameObject);

	Signature SetEnabled(GameObject gameObject, bool enabled);

	bool IsEnabled(GameObject gameObject);

	void SetSignature(GameObject gameObject, Signature signature);

	const char* GetName(GameObject gameObject);

	void SetName(GameObject gameObject, std::string name);

	Signature GetSignature(GameObject gameObject);

	int GetLivingGameObjectCount();

private:
	std::queue<GameObject> availableGameObjects{};
	std::array<Signature, MAX_GAMEOBJECTS> signatures{};
	std::array<Signature, MAX_GAMEOBJECTS> disabledSignatures{};
	std::array<char[MAX_GAMEOBJECT_NAME_LENGTH], MAX_GAMEOBJECTS> names{};
	uint32_t livingGameObjectCount{};
};
