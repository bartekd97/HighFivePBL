#pragma once

#include "../IScene.h"

class GameScene : public IScene
{
public:
	virtual void Load() override;
	virtual void Unload() override;
};