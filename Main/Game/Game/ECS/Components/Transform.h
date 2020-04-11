#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "ECS/ECSTypes.h"

class Transform
{
private:
	GameObject gameObject;

public:
	Transform();
	Transform(GameObject gameObject);

	inline glm::vec3 GetPosition() { return this->position; }
	inline glm::quat GetRotation() { return this->rotation; }
	inline glm::vec3 GetRotationEuler() { return this->rotationEuler; }
	inline glm::vec3 GetScale() { return this->scale; }
	inline glm::vec3 GetWorldPosition() { return this->worldPosition; }
	inline glm::quat GetWorldRotation() { return this->worldRotation; }
	inline glm::quat GetWorldRotationEuler() { return this->worldRotationEuler; }
	inline glm::vec3 GetWorldScale() { return this->worldScale; }

	inline glm::vec3 GetFront() { return this->front; }
	inline glm::vec3 GetUp() { return this->up; }
	inline glm::vec3 GetRight() { return this->right; }
	inline glm::vec3 GetWorldFront() { return this->worldFront; }
	inline glm::vec3 GetWorldUp() { return this->worldUp; }
	inline glm::vec3 GetWorldRight() { return this->worldRight; }

	inline glm::mat4 GetLocalTransform() { return this->localTransform; }
	inline glm::mat4 GetWorldTransform() { return this->worldTransform; }



	inline void SetPosition(glm::vec3 position) {
		this->position = position;
		UpdateSelf();
	}
	inline void SetRotation(glm::quat rotation) {
		this->rotation = rotation;
		this->rotationEuler = glm::vec3(
			glm::degrees(glm::pitch(rotation)), // x
			glm::degrees(glm::yaw(rotation)), // y
			glm::degrees(glm::roll(rotation)) // z
			);
		UpdateSelf();
	}
	inline void SetRotation(glm::vec3 euler) {
		this->rotationEuler = euler;
		this->rotation = glm::quat(glm::vec3(
			glm::radians(euler.x), // pitch
			glm::radians(euler.y), // yaw
			glm::radians(euler.z) // roll
			));
		UpdateSelf();
	}
	inline void SetScale(glm::vec3 scale) {
		this->scale = scale;
		UpdateSelf();
	}




	inline void SetPositionRotation(glm::vec3 position, glm::quat rotation) {
		this->position = position;
		this->rotation = rotation;
		this->rotationEuler = glm::vec3(
			glm::degrees(glm::pitch(rotation)), // x
			glm::degrees(glm::yaw(rotation)), // y
			glm::degrees(glm::roll(rotation)) // z
			);
		UpdateSelf();
	}
	inline void SetPositionRotation(glm::vec3 position, glm::vec3 euler) {
		this->position = position;
		this->rotationEuler = euler;
		this->rotation = glm::quat(glm::vec3(
			glm::radians(euler.x), // pitch
			glm::radians(euler.y), // yaw
			glm::radians(euler.z) // roll
			));
		UpdateSelf();
	}




	inline void SetPositionRotationScale(glm::vec3 position, glm::quat rotation, glm::vec3 scale) {
		this->position = position;
		this->rotation = rotation;
		this->rotationEuler = glm::vec3(
			glm::degrees(glm::pitch(rotation)), // x
			glm::degrees(glm::yaw(rotation)), // y
			glm::degrees(glm::roll(rotation)) // z
			);
		this->scale = scale;
		UpdateSelf();
	}
	inline void SetPositionRotationScale(glm::vec3 position, glm::vec3 euler, glm::vec3 scale) {
		this->position = position;
		this->rotationEuler = euler;
		this->rotation = glm::quat(glm::vec3(
			glm::radians(euler.x), // pitch
			glm::radians(euler.y), // yaw
			glm::radians(euler.z) // roll
			));
		this->scale = scale;
		UpdateSelf();
	}



	inline void TranslateSelf(float distance, glm::vec3 direction) {
		this->position += direction * distance;
		UpdateSelf();
	}
	inline void RotateSelf(float angle, glm::vec3 axis) {
		this->rotation = glm::rotate(this->rotation, glm::radians(angle), axis * this->rotation);
		this->rotationEuler = glm::vec3(
			glm::degrees(glm::pitch(this->rotation)), // x
			glm::degrees(glm::yaw(this->rotation)), // y
			glm::degrees(glm::roll(this->rotation)) // z
			);
		UpdateSelf();
	}


private:
	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	glm::quat rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
	glm::vec3 rotationEuler = { 0.0f, 0.0f, 0.0f };
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	glm::vec3 worldPosition;
	glm::quat worldRotation;
	glm::vec3 worldRotationEuler;
	glm::vec3 worldScale;

	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldFront;
	glm::vec3 worldUp;
	glm::vec3 worldRight;

	glm::mat4 localTransform;
	glm::mat4 worldTransform;

	glm::mat4 parentWorldTransform;


	void UpdateFromParent(glm::mat4& parentWorldTransform);
	void UpdateSelf();
};
