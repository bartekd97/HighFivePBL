#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

class Transform
{
public:
	Transform();
	Transform(glm::vec3 position);

	glm::vec3 GetPosition();
	glm::quat GetRotation();
	glm::vec3 GetRotationEuler();
	glm::vec3 GetScale();
	glm::vec3 GetWorldPosition();
	glm::quat GetWorldRotation();
	glm::quat GetWorldRotationEuler();
	glm::vec3 GetWorldScale();

	glm::vec3 GetFront();
	glm::vec3 GetUp();
	glm::vec3 GetRight();
	glm::vec3 GetWorldFront();
	glm::vec3 GetWorldUp();
	glm::vec3 GetWorldRight();

	glm::mat4 GetLocalTransform();
	glm::mat4 GetWorldTransform();

	bool GetDirty();


	void SetPosition(glm::vec3 position);
	void SetRotation(glm::quat rotation);
	void SetRotation(glm::vec3 euler);
	void SetScale(glm::vec3 scale);

	void TranslateSelf(float distance, glm::vec3 direction);
	void RotateSelf(float angle, glm::vec3 axis);

	bool Update(glm::mat4 parentWorldTransform, bool parentDirty);

private:
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 rotationEuler;
	glm::vec3 scale;
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

	bool dirty;

};
