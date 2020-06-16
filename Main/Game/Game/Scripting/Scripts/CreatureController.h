#pragma once

#include "../Script.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/RigidBody.h"

#define GetTransform() HFEngine::ECS.GetComponent<Transform>(GetGameObject())
#define GetRigidBody() HFEngine::ECS.GetComponent<RigidBody>(GetGameObject())

class CreatureController : public Script
{
protected:
	float moveSpeed;
	float maxHealth;
	bool isFlying = false;

protected:
	float health;
	float currentMoveSpeed = 0.0f;
	float slow = 0.0f;
public:
	CreatureController()
	{
		RegisterFloatParameter("moveSpeed", &moveSpeed);
		RegisterFloatParameter("maxHealth", &maxHealth);
		RegisterIntParameter("isFlying", (int*)&isFlying); // TODO: bool parameter
	}

	virtual void TakeDamage(float dmg) = 0;

	inline void SetSlow(float slow)
	{
		this->slow = slow;
	}

	inline float GetSlow()
	{
		return slow;
	}

	inline bool GetIsFlying()
	{
		return isFlying;
	}

	void Move(const glm::vec3& direction, float dt)
	{
		auto& transform = GetTransform();
		auto& rigidBody = GetRigidBody();
		rigidBody.Move(transform.GetPosition() + (direction * (currentMoveSpeed - slow) * dt));
	}
};

#undef GetTransform()
#undef GetRigidBody()