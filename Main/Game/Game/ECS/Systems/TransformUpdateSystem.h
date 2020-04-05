#pragma once

#include "../SystemUpdate.h"

class TransformUpdateSystem : public SystemUpdate
{
public:
	void Update(float dt) override;
private:
	void UpdateGameObject(GameObject gameObject, glm::mat4& parentWorldTransform, bool parentDirty);
	bool GameObjectHasTransform(GameObject gameObject);
};
