#include "../../Utility/Logger.h"

#include "HFEngine.h"
#include "TransformUpdateSystem.h"
#include "../GameObjectHierarchy.h"

#include "../Components.h"

extern GameObjectHierarchy gameObjectHierarchy;

void TransformUpdateSystem::Update(float dt)
{
	auto root = gameObjectHierarchy.GetRoot();
	for (auto it = root.begin(); it != root.end(); it++)
	{
		UpdateGameObject(*it);
	}
}

void TransformUpdateSystem::UpdateGameObject(GameObject gameObject)
{
	// TODO: shouldnt transform be built in?
	if (GameObjectHasTransform(gameObject))
	{
		auto transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
		if (transform.dirty)
		{
			auto children = gameObjectHierarchy.GetChildren(gameObject);
			for (auto it = children.begin(); it != children.end(); it++)
			{
				if (GameObjectHasTransform(*it))
				{
					auto& childTransform = HFEngine::ECS.GetComponent<Transform>(*it);

					// TODO: spoko, wiem ¿e tak siê tego nie liczy, to demko tylko xD
					childTransform.worldPosition = transform.worldPosition + transform.position;
					childTransform.worldRotation = transform.worldRotation + transform.rotation;
					childTransform.worldScale = transform.worldScale * transform.scale;
					childTransform.dirty = true;
				}
				UpdateGameObject(*it);
			}
			transform.dirty = false;
		}
	}
}

bool TransformUpdateSystem::GameObjectHasTransform(GameObject gameObject)
{
	return gameObjects.find(gameObject) != gameObjects.end();
}
