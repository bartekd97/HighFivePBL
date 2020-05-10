#include "Transform.h"
#include "HFEngine.h"

Transform::Transform() : gameObject(NULL_GAMEOBJECT) {}
Transform::Transform(GameObject gameObject) : gameObject(gameObject) {
	std::optional<GameObject> parent = gameObjectHierarchy.GetParent(gameObject);
	if (parent.has_value())
		parentWorldTransform = HFEngine::ECS.GetComponent<Transform>(parent.value()).GetWorldTransform();
	else
		parentWorldTransform = glm::mat4(1.0f);
	UpdateSelf();
}

void Transform::UpdateFromParent(glm::mat4& parentWorldTransform) {
	this->parentWorldTransform = parentWorldTransform;
	UpdateSelf();
}

void Transform::UpdateSelf() {
	assert(gameObject != NULL_GAMEOBJECT && "Updating generic transform");
	// local transform
	{
		glm::mat4 translateMat = glm::translate(glm::mat4(1.0), position);
		glm::mat4 rotateMat = glm::mat4_cast(rotation);
		glm::mat4 scaleMat = glm::scale(glm::mat4(1.0), scale);

		localTransform = translateMat * rotateMat * scaleMat;
		right = rotation * glm::vec3(1, 0, 0);
		up = rotation * glm::vec3(0, 1, 0);
		front = rotation * glm::vec3(0, 0, -1); // https://shot511.github.io/2017-09-25-uklady-wspolrzednych/
	}

	worldTransform = parentWorldTransform * localTransform;
	// decompose world
	{
		glm::vec3 skew;
		glm::vec4 perspective;
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
	}

	// update frame number
	lastFrameUpdate = HFEngine::CURRENT_FRAME_NUMBER;

	// update children
	{
		auto children = gameObjectHierarchy.GetChildren(gameObject);
		for (auto it = children.begin(); it != children.end(); it++)
		{
			HFEngine::ECS.GetComponent<Transform>(*it).UpdateFromParent(worldTransform);
		}
	}

}