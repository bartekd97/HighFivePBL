#pragma once

#include <array>
#include <cassert>
#include <queue>

#include "ECSTypes.h"

class GameObjectManager
{
public:
	GameObjectManager();
	GameObject CreateGameObject();
	void DestroyGameObject(GameObject gameObject);
	void SetSignature(GameObject gameObject, Signature signature);
	Signature GetSignature(GameObject gameObject);
	int GetLivingGameObjectCount();

private:
	std::queue<GameObject> availableGameObjects{};
	std::array<Signature, MAX_GAMEOBJECTS> signatures{};
	uint32_t livingGameObjectCount{};
};
