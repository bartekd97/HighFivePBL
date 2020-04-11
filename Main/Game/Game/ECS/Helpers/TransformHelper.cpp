#include "TransformHelper.h"

#include "HFEngine.h"

namespace TransformHelper
{
	void UpdateGameObject(GameObject gameObject, glm::mat4& parentWorldTransform, bool parentDirty)
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

	void Update(GameObject gameObject)
	{
		glm::mat4 identity(1.0f);
		UpdateGameObject(gameObject, identity, false);
	}

	void UpdateFromRoot(GameObject gameObject)
	{
		std::optional<GameObject> parent;
		GameObject last = gameObject;
		do
		{
			parent = gameObjectHierarchy.GetParent(last);
			if (parent.has_value())
			{
				last = parent.value();
			}
		} while (parent.has_value());
		glm::mat4 identity(1.0f);
		UpdateGameObject(last, identity, false);
	}

}
