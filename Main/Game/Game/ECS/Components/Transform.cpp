#include "Transform.h"

Transform::Transform()
{
	SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
	dirty = true;
}

glm::vec3 Transform::GetPosition()
{
	return position;
}

glm::quat Transform::GetRotation()
{
	return rotation;
}

glm::vec3 Transform::GetRotationEuler()
{
	return rotationEuler;
}

glm::vec3 Transform::GetScale()
{
	return scale;
}

glm::vec3 Transform::GetWorldPosition()
{
	return worldPosition;
}

glm::quat Transform::GetWorldRotation()
{
	return worldRotation;
}

glm::quat Transform::GetWorldRotationEuler()
{
	return worldRotationEuler;
}

glm::vec3 Transform::GetWorldScale()
{
	return worldScale;
}

glm::vec3 Transform::GetFront()
{
	return front;
}

glm::vec3 Transform::GetUp()
{
	return up;
}

glm::vec3 Transform::GetRight()
{
	return right;
}

glm::vec3 Transform::GetWorldFront()
{
	return worldFront;
}

glm::vec3 Transform::GetWorldUp()
{
	return worldUp;
}

glm::vec3 Transform::GetWorldRight()
{
	return worldRight;
}

glm::mat4 Transform::GetLocalTransform()
{
	return localTransform;
}

glm::mat4 Transform::GetWorldTransform()
{
	return worldTransform;
}

bool Transform::GetDirty()
{
	return dirty;
}


void Transform::SetPosition(glm::vec3 position)
{
	this->position = position;
	dirty = true;
}

void Transform::SetRotation(glm::quat rotation)
{
	this->rotation = rotation;
	this->rotationEuler = glm::vec3(
		glm::degrees(glm::pitch(rotation)), // x
		glm::degrees(glm::yaw(rotation)), // y
		glm::degrees(glm::roll(rotation)) // z
	);
	dirty = true;
}

void Transform::SetRotation(glm::vec3 euler)
{
	this->rotationEuler = euler;
	this->rotation = glm::quat(glm::vec3(
		glm::radians(euler.x), // pitch
		glm::radians(euler.y), // yaw
		glm::radians(euler.z) // roll
	));
	dirty = true;
}

void Transform::SetScale(glm::vec3 scale)
{
	this->scale = scale;
	dirty = true;
}

void Transform::TranslateSelf(float distance, glm::vec3 direction)
{
	position += direction * distance;
	dirty = true;
}

void Transform::RotateSelf(float angle, glm::vec3 axis)
{
	rotation = glm::rotate(rotation, glm::radians(angle), axis * rotation);
	this->rotationEuler = glm::vec3(
		glm::degrees(glm::pitch(rotation)), // x
		glm::degrees(glm::yaw(rotation)), // y
		glm::degrees(glm::roll(rotation)) // z
	);
	dirty = true;
}


bool Transform::Update(glm::mat4 parentWorldTransform, bool parentDirty)
{
	if (parentDirty == false && dirty == false)
		return false;

	glm::mat4 translateMat, rotateMat, scaleMat;

	if (dirty) // local dirty
	{
		translateMat = glm::translate(glm::mat4(1.0), position);
		rotateMat = glm::mat4_cast(rotation);
		scaleMat = glm::scale(glm::mat4(1.0), scale);

		localTransform = translateMat * rotateMat * scaleMat;
		right = rotation * glm::vec3(1, 0, 0);
		up = rotation * glm::vec3(0, 1, 0);
		front = rotation * glm::vec3(0, 0, -1); // https://shot511.github.io/2017-09-25-uklady-wspolrzednych/
	}

	glm::vec3 skew;
	glm::vec4 perspective;

	worldTransform = parentWorldTransform * localTransform;
	glm::decompose(
		worldTransform,
		worldScale,
		worldRotation,
		worldPosition,
		skew,
		perspective
	);
	worldRotationEuler = glm::vec3(
		glm::degrees(glm::pitch(worldRotation)), // x
		glm::degrees(glm::yaw(worldRotation)), // y
		glm::degrees(glm::roll(worldRotation)) // z
	);
	worldRight = worldRotation * glm::vec3(1, 0, 0);
	worldUp = worldRotation * glm::vec3(0, 1, 0);
	worldFront = worldRotation * glm::vec3(0, 0, -1); // https://shot511.github.io/2017-09-25-uklady-wspolrzednych/

	dirty = false;
	return true;
}
