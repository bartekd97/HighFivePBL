#pragma once

#include "../Script.h"
#include "../../HFEngine.h"
#include "../../ECS/Components.h"
#include "../../InputManager.h"

class CharControllerTest : public Script
{
public:
	void Awake()
	{
	}

	void Start()
	{
		auto cam = HFEngine::ECS.GetGameObjectByName("CameraObject");
		assert(cam.has_value() && "CharController: couldn't resolve camera object");
		cameraObject = cam.value();

		auto& transform = HFEngine::ECS.GetComponent<Transform>(GetGameObject());
		auto& camTransform = HFEngine::ECS.GetComponent<Transform>(cameraObject);
		cameraOffset = transform.GetPosition() - camTransform.GetPosition();
	}

	void Update(float dt)
	{
		auto& transform = HFEngine::ECS.GetComponent<Transform>(GetGameObject());
		auto& rigidBody = HFEngine::ECS.GetComponent<RigidBody>(GetGameObject());

		glm::vec3 direction(0.0f);
		if (InputManager::GetKeyStatus(GLFW_KEY_A)) direction.x = -1.0f;
		else if (InputManager::GetKeyStatus(GLFW_KEY_D)) direction.x = 1.0f;

		if (InputManager::GetKeyStatus(GLFW_KEY_W)) direction.z = -1.0f;
		else if (InputManager::GetKeyStatus(GLFW_KEY_S)) direction.z = 1.0f;

		if (InputManager::GetKeyStatus(GLFW_KEY_SPACE)) speed = 1120.0f;
		else speed = 20.0f;

		if (glm::length2(direction) > 0.5f)
		{
			transform.SetRotation(glm::vec3(0.0f, std::atan2(direction.x, direction.z) * 180.0 / M_PI, 0.0f));
		}
		rigidBody.Move(transform.GetPosition() + (direction * speed * dt));
	}

	void LateUpdate(float dt)
	{
		auto& transform = HFEngine::ECS.GetComponent<Transform>(GetGameObject());
		auto& camTransform = HFEngine::ECS.GetComponent<Transform>(cameraObject);
		camTransform.SetPosition(transform.GetPosition() - cameraOffset);
	}

private:
	float speed = 20.0f;
	GameObject cameraObject;
	glm::vec3 cameraOffset;
};
