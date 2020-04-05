#include "../../Utility/Logger.h"

#include "HFEngine.h"
#include "TransformUpdateSystem.h"
#include "../GameObjectHierarchy.h"

#include "../Components.h"

extern GameObjectHierarchy gameObjectHierarchy;

void TransformUpdateSystem::Update(float dt)
{
	glm::mat4 identity(1.0f);
	auto root = gameObjectHierarchy.GetRoot();
	for (auto it = root.begin(); it != root.end(); it++)
	{
		UpdateGameObject(*it, identity, false);
	}
	for (auto const& gameObject : gameObjects)
	{
		if (!gameObjectHierarchy.IsPresent(gameObject))
		{
			glm::mat4 modelMat(1.0f);
			UpdateGameObject(gameObject, identity, false);
		}
	}
}

// TODO: maybe iterative instead of recursive?
void TransformUpdateSystem::UpdateGameObject(GameObject gameObject, glm::mat4& parentWorldTransform, bool parentDirty)
{
	auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
	bool nextDirty = transform.Update(parentWorldTransform, parentDirty);
	auto children = gameObjectHierarchy.GetChildren(gameObject);
	for (auto it = children.begin(); it != children.end(); it++)
	{
		glm::mat4 modelMat = transform.GetWorldTransform();
		UpdateGameObject(*it, modelMat, nextDirty);
	}
}

bool TransformUpdateSystem::GameObjectHasTransform(GameObject gameObject)
{
	return gameObjects.find(gameObject) != gameObjects.end();
}
