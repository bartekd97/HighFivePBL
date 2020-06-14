#pragma once

#include "../Script.h"
#include "HFEngine.h"

#define GetTransform() HFEngine::ECS.GetComponent<Transform>(GetGameObject())
#define GetAnimator() HFEngine::ECS.GetComponent<SkinAnimator>(visualObject)
#define GetRigidBody() HFEngine::ECS.GetComponent<RigidBody>(GetGameObject())

class PlayerDeathController : public Script
{
public:
	void Start()
	{
		EventManager::AddListener(METHOD_LISTENER(Events::Gameplay::Player::DEATH, PlayerDeathController::OnPlayerDeath));
	}

	void OnPlayerDeath(Event& ev)
	{

	}
};
