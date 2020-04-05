#pragma once

#include "../SystemUpdate.h"

class TransformUpdateSystem : public SystemUpdate
{
public:
	void Update(float dt) override;
private:
	void UpdateGameObject(GameObject gameObject);
	bool GameObjectHasTransform(GameObject gameObject);
};
